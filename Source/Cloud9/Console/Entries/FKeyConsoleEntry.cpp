// Copyright (c) 2024 Alexei Gladkikh

#include "FKeyConsoleEntry.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "GameFramework/InputSettings.h"

void FKeyConsoleEntry::Initialize(IConsoleCommand* CCom, UWorld* World)
{
	FConsoleEntry::Initialize(CCom, World);
	let Args = GetValueAsString();
	ExecuteConsoleCommand(Args, World);
}

bool FKeyConsoleEntry::FromConsoleArgs(const TArray<FString>& Args)
{
	using namespace ETContainer;

	if (let Key = ConvertFromArgs(Args))
	{
		var InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());

		if (InputSettings->DoesAxisExist(*Name))
		{
			var Mapping = TArray<FInputAxisKeyMapping>();
			InputSettings->GetAxisMappingByName(*Name, Mapping);
			Mapping | ForEach{[InputSettings](let& It) { InputSettings->RemoveAxisMapping(It); }};
			InputSettings->AddAxisMapping(FInputAxisKeyMapping(*Name, *Key), true);
		}
		else if (InputSettings->DoesActionExist(*Name))
		{
			var Mapping = TArray<FInputActionKeyMapping>();
			InputSettings->GetActionMappingByName(*Name, Mapping);
			Mapping | ForEach{[InputSettings](let& It) { InputSettings->RemoveActionMapping(It); }};
			InputSettings->AddActionMapping(FInputActionKeyMapping(*Name, *Key), true);
		}

		InputSettings->SaveKeyMappings();

		ValueRef = *Key;
	}

	return false;
}

bool FKeyConsoleEntry::IsArgsValid(const TArray<FString>& Args) const { return ConvertFromArgs(Args).IsSet(); }

FString FKeyConsoleEntry::GetValueAsString() { return ValueRef.ToString(); }

TOptional<FKey> FKeyConsoleEntry::ConvertFromArgs(const TArray<FString>& Args)
{
	if (Args.Num() == 1)
	{
		if (let Key = FKey(*Args[0]); Key.IsValid())
		{
			return Key;
		}
	}

	return {};
}

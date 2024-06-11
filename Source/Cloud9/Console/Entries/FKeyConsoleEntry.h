// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "GameFramework/InputSettings.h"

struct FInputAxisKeyMapping;
class UInputSettings;

class FKeyConsoleEntry final : public FConsoleEntry
{
public:
	FKeyConsoleEntry(FKey& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual void Initialize(IConsoleCommand* CCom, UWorld* World) override
	{
		FConsoleEntry::Initialize(CCom, World);
		let Args = GetValueAsString();
		ExecuteConsoleCommand(Args, World);
	}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override
	{
		using namespace ETContainer;

		if (Args.Num() == 1)
		{
			if (let NewKey = FKey(*Args[0]); NewKey.IsValid())
			{
				var InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());

				if (InputSettings->DoesAxisExist(*Name))
				{
					var Mapping = TArray<FInputAxisKeyMapping>();
					InputSettings->GetAxisMappingByName(*Name, Mapping);
					Mapping | ForEach{[InputSettings](let& It) { InputSettings->RemoveAxisMapping(It); }};
					InputSettings->AddAxisMapping(FInputAxisKeyMapping(*Name, NewKey), true);
				}
				else if (InputSettings->DoesActionExist(*Name))
				{
					var Mapping = TArray<FInputActionKeyMapping>();
					InputSettings->GetActionMappingByName(*Name, Mapping);
					Mapping | ForEach{[InputSettings](let& It) { InputSettings->RemoveActionMapping(It); }};
					InputSettings->AddActionMapping(FInputActionKeyMapping(*Name, NewKey), true);
				}

				InputSettings->SaveKeyMappings();

				ValueRef = NewKey;
			}
		}

		return false;
	}

	virtual FString GetValueAsString() override
	{
		return ValueRef.ToString();
	}

private:
	FKey& ValueRef;
};

// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "Cloud9DeveloperSettings.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Extensions/FString.h"
#include "Cloud9/Tools/Extensions/UObject.h"
#include "Cloud9/Tools/Extensions/WhenOrNone.h"

// ReSharper disable once CppPossiblyUninitializedMember
UCloud9DeveloperSettings::UCloud9DeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsShowMouseCursor = 0;
	bIsDrawDeprojectedCursorLine = 0;
	bIsDrawHitCursorLine = 0;
	NetGraph = 0;
	Volume = 0.1;
}

const UCloud9DeveloperSettings* UCloud9DeveloperSettings::Get()
{
	static let Settings = StaticClass()->GetDefaultObject<UCloud9DeveloperSettings>();
	Settings->InitializeCVars();
	return Settings;
}

void UCloud9DeveloperSettings::Save()
{
	UpdateDefaultConfigFile();
	log(Display, "%s", this | EUObject::Stringify{} | EFString::ToCStr{});
}

template <typename TValue>
auto UCloud9DeveloperSettings::RegisterConsoleVariable(TValue& ValueRef, const TCHAR* Name, const TCHAR* Help)
{
	static_assert(
		TIsSame<TValue, int>::Value ||
		TIsSame<TValue, float>::Value ||
		TIsSame<TValue, bool>::Value ||
		TIsSame<TValue, FString>::Value,
		"TValue must be int, float, bool or FString"
	);

	let ConsoleManager = &IConsoleManager::Get();

	let CVar = ConsoleManager->RegisterConsoleVariableRef(Name, ValueRef, Help);

	CVar->AsVariable()->SetOnChangedCallback(
		FConsoleVariableDelegate::CreateLambda([this](auto Arg) { Save(); })
	);

	return CVar;
}

void UCloud9DeveloperSettings::InitializeCVars()
{
	static var bIsConsoleInitialized = false;

	if (not bIsConsoleInitialized)
	{
		bIsConsoleInitialized = true;

		RegisterConsoleVariable(
			bIsShowMouseCursor,
			TEXT("r.IsShowMouseCursor"),
			TEXT("Whether to draw line from character to GetHitUnderCursor point")
		);

		RegisterConsoleVariable(
			bIsDrawDeprojectedCursorLine,
			TEXT("r.IsDrawDeprojectedCursorLine"),
			TEXT("Whether to draw line from character to deprojected mouse cursor")
		);

		RegisterConsoleVariable(
			bIsDrawHitCursorLine,
			TEXT("r.bIsDrawHitCursorLine"),
			TEXT("Whether to show mouse cursor on screen or not in game")
		);

		RegisterConsoleVariable(
			NetGraph,
			TEXT("r.NetGraph"),
			TEXT("Whether to show FPS and other specific debug info")
		);

		RegisterConsoleVariable(
			CameraVerticalSpeedLag,
			TEXT("r.CameraVerticalSpeedLag"),
			TEXT("Configure how smoothly does the camera change its position vertically")
		);

		RegisterConsoleVariable(
			Volume,
			TEXT("r.Volume"),
			TEXT("Basic game volume")
		);

		log(Display, "%s", this | EUObject::Stringify{} | EFString::ToCStr{});
	}
}

#if WITH_EDITOR
void UCloud9DeveloperSettings::PostInitProperties()
{
	InitializeCVars();
	Super::PostInitProperties();
	if (IsTemplate())
	{
		ImportConsoleVariableValues();
	}
}

void UCloud9DeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	InitializeCVars();
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
	{
		ExportValuesToConsoleVariables(PropertyChangedEvent.Property);
	}
}
#endif

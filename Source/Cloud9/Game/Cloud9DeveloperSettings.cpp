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

#include "Cloud9/Tools/Extensions/FString.h"
#include "Cloud9/Tools/Extensions/UObject.h"

FString UCloud9DeveloperSettings::ShowMouseCursorName = "r.ShowMouseCursor";
FString UCloud9DeveloperSettings::DrawDeprojectedCursorLineName = "r.DrawDeprojectedCursorLine";
FString UCloud9DeveloperSettings::DrawHitCursorLineName = "r.DrawHitCursorLine";
FString UCloud9DeveloperSettings::DrawExplosionSphereName = "r.DrawExplosionSphere";
FString UCloud9DeveloperSettings::DrawHitScanName = "r.DrawHitScan";
FString UCloud9DeveloperSettings::PrintHitScanInfoName = "r.PrintHitScanInfo";
FString UCloud9DeveloperSettings::NetGraphName = "r.NetGraph";
FString UCloud9DeveloperSettings::AutoSelectWeaponName = "r.AutoSelectWeapon";
FString UCloud9DeveloperSettings::InfiniteAmmoName = "r.InfiniteAmmo";
FString UCloud9DeveloperSettings::CheatsName = "r.Cheats";
FString UCloud9DeveloperSettings::SelfAimEnabledName = "r.SelfAimEnabled";
FString UCloud9DeveloperSettings::CameraVerticalSpeedLagName = "r.CameraVerticalSpeedLag";
FString UCloud9DeveloperSettings::VolumeName = "r.Volume";

// ReSharper disable once CppPossiblyUninitializedMember
UCloud9DeveloperSettings::UCloud9DeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsDrawHitCursorLine = 0;
	bIsDrawDeprojectedCursorLine = 0;
	bIsShowMouseCursor = 0;
	bIsDrawExplosionSpheres = 0;
	bIsDrawHitScan = 0;
	bIsPrintHitScanInfo = 0;
	CameraVerticalSpeedLag = 0.0;
	NetGraph = 0;
	bIsAutoSelectWeapon = 0;
	bIsInfiniteAmmo = 0;
	bIsCheatsEnabled = 0;
	bIsSelfAimEnabled = 0;
	Volume = 0.1;
	WeaponAirSpreadScale = 1.0f;
	JumpImpulse = 301.993377;
	WeaponDebugMaxInaccuracy = 0;
	WeaponDebugInaccuracyOnlyUp = 0;
}

UCloud9DeveloperSettings* UCloud9DeveloperSettings::Get()
{
	static var Settings = StaticClass()->GetDefaultObject<UCloud9DeveloperSettings>();
	Settings->InitializeCVars();
	return Settings;
}

void UCloud9DeveloperSettings::Save()
{
	UpdateDefaultConfigFile();
	log(Display, "%s", this | EUObject::Stringify{} | EFString::ToCStr{});
}

template <typename ValueType>
auto UCloud9DeveloperSettings::RegisterConsoleVariable(ValueType& ValueRef, const TCHAR* Name, const TCHAR* Help)
{
	static_assert(
		TIsSame<ValueType, int>::Value ||
		TIsSame<ValueType, float>::Value ||
		TIsSame<ValueType, bool>::Value ||
		TIsSame<ValueType, FString>::Value,
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
			*ShowMouseCursorName,
			TEXT("Whether to draw line from character to GetHitResultUnderCursor point")
		);

		RegisterConsoleVariable(
			bIsDrawDeprojectedCursorLine,
			*DrawDeprojectedCursorLineName,
			TEXT("Whether to draw line from character to deprojected mouse cursor")
		);

		RegisterConsoleVariable(
			bIsDrawHitCursorLine,
			*DrawHitCursorLineName,
			TEXT("Whether to show mouse cursor on screen or not in game")
		);

		RegisterConsoleVariable(
			bIsDrawExplosionSpheres,
			*DrawExplosionSphereName,
			TEXT("Whether to draw debug explosions spheres")
		);

		RegisterConsoleVariable(
			bIsDrawHitScan,
			*DrawHitScanName,
			TEXT("Whether to draw debug hit scan lines")
		);

		RegisterConsoleVariable(
			bIsPrintHitScanInfo,
			*PrintHitScanInfoName,
			TEXT("Whether to print hit scan info")
		);

		RegisterConsoleVariable(
			NetGraph,
			*NetGraphName,
			TEXT("Whether to show FPS and other specific debug info")
		);

		RegisterConsoleVariable(
			bIsAutoSelectWeapon,
			*AutoSelectWeaponName,
			TEXT("Select weapon after picking it up")
		);

		RegisterConsoleVariable(
			bIsInfiniteAmmo,
			*InfiniteAmmoName,
			TEXT("Infinite Weapon Ammo")
		);

		RegisterConsoleVariable(
			bIsCheatsEnabled,
			*CheatsName,
			TEXT("Enable cheats")
		);

		RegisterConsoleVariable(
			bIsSelfAimEnabled,
			*SelfAimEnabledName,
			TEXT("Enable self aim")
		);

		RegisterConsoleVariable(
			CameraVerticalSpeedLag,
			*CameraVerticalSpeedLagName,
			TEXT("Configure how smoothly does the camera change its position vertically")
		);

		RegisterConsoleVariable(
			Volume,
			*VolumeName,
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

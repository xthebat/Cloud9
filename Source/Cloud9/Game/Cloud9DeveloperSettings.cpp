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
FString UCloud9DeveloperSettings::NoInaccuracyName = "r.NoInaccuracy";
FString UCloud9DeveloperSettings::JumpImpulseName = "r.JumpImpulse";
FString UCloud9DeveloperSettings::WeaponAirSpreadScaleName = "r.WeaponAirSpreadScale";
FString UCloud9DeveloperSettings::WeaponDebugMaxInaccuracyName = "r.WeaponDebugMaxInaccuracy";
FString UCloud9DeveloperSettings::WeaponDebugInaccuracyOnlyUpName = "r.WeaponDebugInaccuracyOnlyUp";
FString UCloud9DeveloperSettings::WeaponRecoilDecayCoefficientName = "r.WeaponRecoilDecayCoefficient";
FString UCloud9DeveloperSettings::DrawShotDirectionAxisName = "r.DrawShotDirectionAxis";
FString UCloud9DeveloperSettings::WeaponDebugDamageInfoName = "r.WeaponDebugDamageInfo";
FString UCloud9DeveloperSettings::TaggingScaleName = "r.TaggingScale";
FString UCloud9DeveloperSettings::DecalLifeSpanName = "r.DecalLifeSpan";
FString UCloud9DeveloperSettings::DecalFadeScreenSizeName = "r.DecalFadeScreenSizeName";
FString UCloud9DeveloperSettings::VolumeName = "r.Volume";

// ReSharper disable once CppPossiblyUninitializedMember
UCloud9DeveloperSettings::UCloud9DeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsDrawHitCursorLine = 0;
	IsDrawDeprojectedCursorLine = 0;
	IsShowMouseCursor = 0;
	IsDrawExplosionSpheres = 0;
	IsDrawHitScan = 0;
	IsPrintHitScanInfo = 0;
	CameraVerticalSpeedLag = 0.0;
	NetGraph = 0;
	IsAutoSelectWeapon = 0;
	IsInfiniteAmmo = 0;
	IsCheatsEnabled = 0;
	IsSelfAimEnabled = 0;
	Volume = 0.1;
	WeaponAirSpreadScale = 1.0f;
	JumpImpulse = 301.993377;
	IsNoInaccuracy = 0;
	WeaponDebugMaxInaccuracy = 0;
	WeaponDebugInaccuracyOnlyUp = 0;
	WeaponRecoilDecayCoefficient = 2.0f;
	WeaponDebugDamageInfo = 0;
	DrawShotDirectionAxis = 0;
	TaggingScale = 1.0f;
	DecalLifeSpan = 20.0f;
	DecalFadeScreenSize = 0.0f;
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
			IsShowMouseCursor,
			*ShowMouseCursorName,
			TEXT("Whether to draw line from character to GetHitResultUnderCursor point")
		);

		RegisterConsoleVariable(
			IsDrawDeprojectedCursorLine,
			*DrawDeprojectedCursorLineName,
			TEXT("Whether to draw line from character to deprojected mouse cursor")
		);

		RegisterConsoleVariable(
			IsDrawHitCursorLine,
			*DrawHitCursorLineName,
			TEXT("Whether to show mouse cursor on screen or not in game")
		);

		RegisterConsoleVariable(
			IsDrawExplosionSpheres,
			*DrawExplosionSphereName,
			TEXT("Whether to draw debug explosions spheres")
		);

		RegisterConsoleVariable(
			IsDrawHitScan,
			*DrawHitScanName,
			TEXT("Whether to draw debug hit scan lines")
		);

		RegisterConsoleVariable(
			IsPrintHitScanInfo,
			*PrintHitScanInfoName,
			TEXT("Whether to print hit scan info")
		);

		RegisterConsoleVariable(
			NetGraph,
			*NetGraphName,
			TEXT("Whether to show FPS and other specific debug info")
		);

		RegisterConsoleVariable(
			IsAutoSelectWeapon,
			*AutoSelectWeaponName,
			TEXT("Select weapon after picking it up")
		);

		RegisterConsoleVariable(
			IsInfiniteAmmo,
			*InfiniteAmmoName,
			TEXT("Infinite Weapon Ammo")
		);

		RegisterConsoleVariable(
			IsCheatsEnabled,
			*CheatsName,
			TEXT("Enable cheats")
		);

		RegisterConsoleVariable(
			IsSelfAimEnabled,
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

		RegisterConsoleVariable(
			IsNoInaccuracy,
			*NoInaccuracyName,
			TEXT("Remove inaccuracy spread while shooting")
		);

		RegisterConsoleVariable(
			JumpImpulse,
			*JumpImpulseName,
			TEXT("sv_jump_impulse")
		);

		RegisterConsoleVariable(
			WeaponAirSpreadScale,
			*WeaponAirSpreadScaleName,
			TEXT("weapon_air_spread_scale")
		);

		RegisterConsoleVariable(
			WeaponDebugMaxInaccuracy,
			*WeaponDebugMaxInaccuracyName,
			TEXT("weapon_debug_max_inaccuracy")
		);

		RegisterConsoleVariable(
			WeaponDebugInaccuracyOnlyUp,
			*WeaponDebugInaccuracyOnlyUpName,
			TEXT("weapon_debug_inaccuracy_only_up")
		);

		RegisterConsoleVariable(
			WeaponRecoilDecayCoefficient,
			*WeaponRecoilDecayCoefficientName,
			TEXT("weapon_recoil_decay_coefficient")
		);

		RegisterConsoleVariable(
			DrawShotDirectionAxis,
			*DrawShotDirectionAxisName,
			TEXT("Draw debug orthonormal axis of shot direction")
		);

		RegisterConsoleVariable(
			WeaponDebugDamageInfo,
			*WeaponDebugDamageInfoName,
			TEXT("Print debug info about damage to character on hit")
		);

		RegisterConsoleVariable(
			TaggingScale,
			*TaggingScaleName,
			TEXT("Scalar for player tagging modifier when hit. Lower values for greater tagging")
		);

		RegisterConsoleVariable(
			DecalLifeSpan,
			*DecalLifeSpanName,
			TEXT("Life span of the decals")
		);

		RegisterConsoleVariable(
			DecalFadeScreenSize,
			*DecalFadeScreenSizeName,
			TEXT("Decal size to fade off screen")
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

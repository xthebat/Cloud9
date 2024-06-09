﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9/Tools/Cloud9StringLibrary.h"
#include "Cloud9/Tools/Extensions/FString.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/UObject.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/Console.h"

class UInputSettings;
// ReSharper disable once CppPossiblyUninitializedMember
UCloud9DeveloperSettings::UCloud9DeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsDrawHitCursorLine = 0;
	IsDrawDeprojectedCursorLine = 0;
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
	MainMenuMusicVolume = 0.005;
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

	BindMoveForward = "W";
	BindMoveBackward = "S";
	BindMoveLeft = "A";
	BindMoveRight = "D";
	BindCrouch = "LeftControl";
	BindWalk = "LeftShift";
	BindJump = "SpaceBar";
	BindSlot1 = "One";
	BindSlot2 = "Two";
	BindSlot3 = "Three";
	BindSlot4 = "Four";
	BindSlot5 = "Five";
	BindReload = "R";
	BindPrimary = "LeftMouseButton";
	BindSecondary = "RightMouseButton";
	BindUse = "E";

	CrosshairSize = 64;
	CrosshairLength = 0.4f;
	CrosshairWidth = 5.0f;
	CrosshairGap = 0.15f;
	CrosshairColor = {0.0f, 1.0f, 0.0f};

	Sensitivity = 1.0f;
	IsWindowsInputEnabled = 0;
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
	OBJECT_DISPLAY("%s", this | EUObject::Stringify{} | EFString::ToCStr{});
	OnChanged.Broadcast();
}

void UCloud9DeveloperSettings::InitializeCVars()
{
	static var bIsConsoleInitialized = false;

	if (not bIsConsoleInitialized)
	{
		bIsConsoleInitialized = true;

		RegisterConsoleVariable(
			IsDrawDeprojectedCursorLine,
			DrawDeprojectedCursorLineName,
			TEXT("Whether to draw line from character to deprojected mouse cursor")
		);

		RegisterConsoleVariable(
			IsDrawHitCursorLine,
			DrawHitCursorLineName,
			TEXT("Whether to show mouse cursor on screen or not in game"));

		RegisterConsoleVariable(
			IsDrawExplosionSpheres,
			DrawExplosionSphereName,
			TEXT("Whether to draw debug explosions spheres"));

		RegisterConsoleVariable(IsDrawHitScan, DrawHitScanName,TEXT("Whether to draw debug hit scan lines"));
		RegisterConsoleVariable(IsPrintHitScanInfo, PrintHitScanInfoName,TEXT("Whether to print hit scan info"));
		RegisterConsoleVariable(NetGraph, NetGraphName,TEXT("Whether to show FPS and other specific debug info"));
		RegisterConsoleVariable(IsAutoSelectWeapon, AutoSelectWeaponName,TEXT("Select weapon after picking it up"));
		RegisterConsoleVariable(IsInfiniteAmmo, InfiniteAmmoName,TEXT("Infinite Weapon Ammo"));
		RegisterConsoleVariable(IsCheatsEnabled, CheatsName,TEXT("Enable cheats"));
		RegisterConsoleVariable(IsSelfAimEnabled, SelfAimEnabledName,TEXT("Enable self aim"));

		RegisterConsoleVariable(
			CameraVerticalSpeedLag,
			CameraVerticalSpeedLagName,
			TEXT("Configure how smoothly does the camera change its position vertically")
		);

		RegisterConsoleVariable(Volume, VolumeName,TEXT("Basic game volume"));
		RegisterConsoleVariable(MainMenuMusicVolume, MainMenuMusicVolumeName,TEXT("Main menu music volume"));
		RegisterConsoleVariable(IsNoInaccuracy, NoInaccuracyName,TEXT("Remove inaccuracy spread while shooting"));
		RegisterConsoleVariable(JumpImpulse, JumpImpulseName, TEXT("sv_jump_impulse"));
		RegisterConsoleVariable(WeaponAirSpreadScale, WeaponAirSpreadScaleName, TEXT("weapon_air_spread_scale"));

		RegisterConsoleVariable(
			WeaponDebugMaxInaccuracy,
			WeaponDebugMaxInaccuracyName,
			TEXT("weapon_debug_max_inaccuracy")
		);

		RegisterConsoleVariable(
			WeaponDebugInaccuracyOnlyUp,
			WeaponDebugInaccuracyOnlyUpName,
			TEXT("weapon_debug_inaccuracy_only_up")
		);

		RegisterConsoleVariable(
			WeaponRecoilDecayCoefficient,
			WeaponRecoilDecayCoefficientName,
			TEXT("weapon_recoil_decay_coefficient")
		);

		RegisterConsoleVariable(
			DrawShotDirectionAxis,
			DrawShotDirectionAxisName,
			TEXT("Draw debug orthonormal axis of shot direction")
		);

		RegisterConsoleVariable(
			WeaponDebugDamageInfo,
			WeaponDebugDamageInfoName,
			TEXT("Print debug info about damage to character on hit")
		);

		RegisterConsoleVariable(
			TaggingScale,
			TaggingScaleName,
			TEXT("Scalar for player tagging modifier when hit. Lower values for greater tagging")
		);

		RegisterConsoleVariable(DecalLifeSpan, DecalLifeSpanName,TEXT("Life span of the decals"));
		RegisterConsoleVariable(DecalFadeScreenSize, DecalFadeScreenSizeName,TEXT("Decal size to fade off screen"));

		RegisterConsoleVariable(BindMoveForward, BindMoveForwardName,TEXT("Move forward bind key"));
		RegisterConsoleVariable(BindMoveBackward, BindMoveBackwardName,TEXT("Move backward bind key"));
		RegisterConsoleVariable(BindMoveLeft, BindMoveLeftName,TEXT("Move left bind key"));
		RegisterConsoleVariable(BindMoveRight, BindMoveRightName,TEXT("Move right bind key"));
		RegisterConsoleVariable(BindCrouch, BindCrouchName,TEXT("Crouch bind key"));
		RegisterConsoleVariable(BindWalk, BindWalkName,TEXT("Walk bind key"));
		RegisterConsoleVariable(BindJump, BindJumpName,TEXT("Jump bind key"));
		RegisterConsoleVariable(BindSlot1, BindSlot1Name,TEXT("Slot1 bind key"));
		RegisterConsoleVariable(BindSlot2, BindSlot2Name,TEXT("Slot2 bind key"));
		RegisterConsoleVariable(BindSlot3, BindSlot3Name,TEXT("Slot3 bind key"));
		RegisterConsoleVariable(BindSlot4, BindSlot4Name,TEXT("Slot4 bind key"));
		RegisterConsoleVariable(BindSlot5, BindSlot5Name,TEXT("Slot5 bind key"));
		RegisterConsoleVariable(BindReload, BindReloadName,TEXT("Reload bind key"));
		RegisterConsoleVariable(BindPrimary, BindPrimaryName,TEXT("Primary bind key"));
		RegisterConsoleVariable(BindSecondary, BindSecondaryName,TEXT("Secondary bind key"));
		RegisterConsoleVariable(BindUse, BindUseName,TEXT("Use bind key"));

		RegisterConsoleVariable(CrosshairSize, CrosshairSizeName,TEXT("Crosshair size in pixels"));
		RegisterConsoleVariable(CrosshairLength, CrosshairLengthName,TEXT("Crosshair length"));
		RegisterConsoleVariable(CrosshairWidth, CrosshairWidthName,TEXT("Crosshair width"));
		RegisterConsoleVariable(CrosshairGap, CrosshairGapName,TEXT("Crosshair gap"));
		RegisterConsoleVariable(CrosshairColor, CrosshairColorName,TEXT("Crosshair color"));

		RegisterConsoleVariable(Sensitivity, SensitivityName,TEXT("Mouse crosshair sensitivity"));
		RegisterConsoleVariable(IsWindowsInputEnabled, IsWindowsInputEnabledName,TEXT("Use windows mouse sensitivity"));

		OBJECT_DISPLAY("%s", this | EUObject::Stringify{} | EFString::ToCStr{});
	}
}

bool UCloud9DeveloperSettings::SetVariableValueByName(const FString& Name, const FString& Value)
{
	let ConsoleManager = &IConsoleManager::Get();
	if (let Variable = ConsoleManager->FindConsoleVariable(*Name))
	{
		if (not Variable->IsVariableFloat())
		{
			Variable->Set(*Value);
			return true;
		}

		let NewValue = UCloud9StringLibrary::SanitizeFloatString(Value);
		Variable->Set(*NewValue);
		return true;
	}

	if (let Command = ConsoleManager->FindConsoleObject(*Name)->AsCommand())
	{
		if (VectorRefs.Find(Name))
		{
			if (FVector Vector; Vector.InitFromString(Value))
			{
				TArray<FString> Args;
				Args.Add(FString::Printf(TEXT("%f"), Vector.X));
				Args.Add(FString::Printf(TEXT("%f"), Vector.Y));
				Args.Add(FString::Printf(TEXT("%f"), Vector.Z));
				return Command->Execute(Args, GetWorld(), *GLog);
			}
		}

		if (KeyRefs.Find(Name))
		{
			if (let Key = FKey(*Value); Key.IsValid())
			{
				return Command->Execute({Key.ToString()}, GetWorld(), *GLog);
			}
		}
	}

	return false;
}

FString UCloud9DeveloperSettings::GetVariableValueByName(const FString& Name)
{
	let ConsoleManager = &IConsoleManager::Get();
	if (let Variable = ConsoleManager->FindConsoleVariable(*Name))
	{
		return Variable->GetString();
	}

	if (ConsoleManager->FindConsoleObject(*Name)->AsCommand())
	{
		if (let VectorPtr = VectorRefs.Find(Name))
		{
			return (*VectorPtr)->ToString();
		}

		if (let KeyPtr = KeyRefs.Find(Name))
		{
			return (*KeyPtr)->ToString();
		}
	}

	return UndefinedConsoleValue;
}

IConsoleObject* UCloud9DeveloperSettings::RegisterConsoleVariable(
	FVector& ValueRef, const FString& Name, const FString& Help)
{
	using namespace ETContainer;
	let ConsoleManager = &IConsoleManager::Get();
	let CCom = ConsoleManager->RegisterConsoleCommand(
		*Name, *Help,
		FConsoleCommandWithArgsDelegate::CreateLambda([this, Name, &ValueRef](const TArray<FString>& Args)
		{
			if (Args.Num() == 3)
			{
				FVector Vector = FVector::ZeroVector;
				let String = FString::Printf(TEXT("X=%s Y=%s Z=%s"), *Args[0], *Args[1], *Args[2]);
				if (Vector.InitFromString(String))
				{
					ValueRef = Vector;
					Save();
				}
			}
			else if (Args.Num() == 0)
			{
				let Console = GEngine->GameViewport->ViewportConsole;
				Console->OutputText(*FString::Printf(TEXT("%s %s"), *Name, *ValueRef.ToString()));
			}
			else
			{
				let Console = GEngine->GameViewport->ViewportConsole;
				Console->OutputText(FString::Printf(TEXT("Invalid arguments for %s"), *Name));
			}
		})
	);
	VectorRefs.Add(Name, &ValueRef);
	return CCom;
}

IConsoleObject* UCloud9DeveloperSettings::RegisterConsoleVariable(
	FKey& ValueRef, const FString& Name, const FString& Help)
{
	using namespace ETContainer;
	let ConsoleManager = &IConsoleManager::Get();
	let CCom = ConsoleManager->RegisterConsoleCommand(
		*Name, *Help,
		FConsoleCommandWithArgsDelegate::CreateLambda([this, Name, &ValueRef](const TArray<FString>& Args)
		{
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
					Save();
				}
			}
			else if (Args.Num() == 0)
			{
				let AddQuotes = [](const FString& Value) { return FString::Printf(TEXT("\"%s\""), *Value); };

				let InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
				let Console = GEngine->GameViewport->ViewportConsole;

				if (InputSettings->DoesAxisExist(*Name))
				{
					var Mapping = TArray<FInputAxisKeyMapping>();
					InputSettings->GetAxisMappingByName(*Name, Mapping);
					let Keys = Mapping
						| Transform{[&AddQuotes](let& It) { return AddQuotes(It.Key.ToString()); }}
						| JoinToString{", "};
					Console->OutputText(*FString::Printf(TEXT("%s %s"), *Name, *Keys));
				}
				else if (InputSettings->DoesActionExist(*Name))
				{
					var Mapping = TArray<FInputActionKeyMapping>();
					InputSettings->GetActionMappingByName(*Name, Mapping);
					let Keys = Mapping
						| Transform{[&AddQuotes](let& It) { return AddQuotes(It.Key.ToString()); }}
						| JoinToString{", "};
					Console->OutputText(*FString::Printf(TEXT("%s %s"), *Name, *Keys));
				}
			}
			else
			{
				let Console = GEngine->GameViewport->ViewportConsole;
				Console->OutputText(FString::Printf(TEXT("Invalid arguments for %s"), *Name));
			}
		})
	);
	KeyRefs.Add(Name, &ValueRef);
	// Execute bind command to update InputSettings file
	CCom->Execute({ValueRef.GetFName().ToString()}, GetWorld(), *GLog);
	return CCom;
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

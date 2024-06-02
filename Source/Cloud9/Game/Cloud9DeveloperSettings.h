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

#pragma once

#include "CoreMinimal.h"

#include "Cloud9/Tools/Concepts.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Macro/Common.h"

#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"

#include "Cloud9DeveloperSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloud9SettingsChanged);

USTRUCT(BlueprintType)
struct FUnUsedStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IntField0 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatField1 = 0.0f;
};

UENUM()
enum class EUnUsedEnum : int32
{
	Special = 0,
	Anything = 1,
	Everything = 2,
	Whatever = 3,
};

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Various Developer Settings"))
class CLOUD9_API UCloud9DeveloperSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
	static inline FString ShowMouseCursorName = "r.ShowMouseCursor";
	static inline FString DrawDeprojectedCursorLineName = "r.DrawDeprojectedCursorLine";
	static inline FString DrawHitCursorLineName = "r.DrawHitCursorLine";
	static inline FString DrawExplosionSphereName = "r.DrawExplosionSphere";
	static inline FString DrawHitScanName = "r.DrawHitScan";
	static inline FString PrintHitScanInfoName = "r.PrintHitScanInfo";
	static inline FString NetGraphName = "r.NetGraph";
	static inline FString AutoSelectWeaponName = "r.AutoSelectWeapon";
	static inline FString InfiniteAmmoName = "r.InfiniteAmmo";
	static inline FString CheatsName = "r.Cheats";
	static inline FString SelfAimEnabledName = "r.SelfAimEnabled";
	static inline FString CameraVerticalSpeedLagName = "r.CameraVerticalSpeedLag";
	static inline FString NoInaccuracyName = "r.NoInaccuracy";
	static inline FString JumpImpulseName = "r.JumpImpulse";
	static inline FString WeaponAirSpreadScaleName = "r.WeaponAirSpreadScale";
	static inline FString WeaponDebugMaxInaccuracyName = "r.WeaponDebugMaxInaccuracy";
	static inline FString WeaponDebugInaccuracyOnlyUpName = "r.WeaponDebugInaccuracyOnlyUp";
	static inline FString WeaponRecoilDecayCoefficientName = "r.WeaponRecoilDecayCoefficient";
	static inline FString DrawShotDirectionAxisName = "r.DrawShotDirectionAxis";
	static inline FString WeaponDebugDamageInfoName = "r.WeaponDebugDamageInfo";
	static inline FString TaggingScaleName = "r.TaggingScale";
	static inline FString DecalLifeSpanName = "r.DecalLifeSpan";
	static inline FString DecalFadeScreenSizeName = "r.DecalFadeScreenSizeName";
	static inline FString VolumeName = "r.Volume";
	static inline FString MainMenuMusicVolumeName = "r.MainMenuMusicVolume";

	static inline FString BindMoveForwardName = "MoveForward";
	static inline FString BindMoveBackwardName = "MoveBackward";
	static inline FString BindMoveLeftName = "MoveLeft";
	static inline FString BindMoveRightName = "MoveRight";
	static inline FString BindCrouchName = "Crouch";
	static inline FString BindWalkName = "Walk";
	static inline FString BindJumpName = "Jump";
	static inline FString BindSlot1Name = "Slot1";
	static inline FString BindSlot2Name = "Slot2";
	static inline FString BindSlot3Name = "Slot3";
	static inline FString BindSlot4Name = "Slot4";
	static inline FString BindSlot5Name = "Slot5";
	static inline FString BindReloadName = "Reload";
	static inline FString BindPrimaryName = "Primary";
	static inline FString BindSecondaryName = "Secondary";
	static inline FString BindUseName = "Use";

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawHitCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawDeprojectedCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsShowMouseCursor;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawExplosionSpheres;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawHitScan;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsPrintHitScanInfo;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float CameraVerticalSpeedLag;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 NetGraph;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsAutoSelectWeapon;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsInfiniteAmmo;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsCheatsEnabled;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsSelfAimEnabled;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Sound)
	float Volume;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Sound)
	float MainMenuMusicVolume;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category=Sound)
	int32 IsNoInaccuracy;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float WeaponAirSpreadScale;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float JumpImpulse;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 WeaponDebugMaxInaccuracy;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 WeaponDebugInaccuracyOnlyUp;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float WeaponRecoilDecayCoefficient;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 DrawShotDirectionAxis;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 WeaponDebugDamageInfo;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float TaggingScale;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float DecalLifeSpan;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float DecalFadeScreenSize;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	EUnUsedEnum UnUsedEnum;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	FUnUsedStruct UnUsedStruct;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindMoveForward;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindMoveBackward;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindMoveLeft;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindMoveRight;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindCrouch;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindWalk;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindJump;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSlot1;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSlot2;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSlot3;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSlot4;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSlot5;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindReload;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindPrimary;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindSecondary;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Bind)
	FKey BindUse;

	// static functions
	UFUNCTION(BlueprintCallable, Category=Settings, DisplayName=GetCloud9DeveloperSettings)
	static UCloud9DeveloperSettings* Get();

	// functions
	UFUNCTION(BlueprintCallable)
	void Save();

	template <typename ValueType>
	void SetVariableValue(const FString& Name, ValueType Value)
	{
		static_assert(
			TIsSame<ValueType, int>::Value ||
			TIsSame<ValueType, float>::Value ||
			TIsSame<ValueType, bool>::Value ||
			TIsSame<ValueType, FString>::Value,
			"TValue must be int, float, bool or FString"
		);

		let ConsoleManager = &IConsoleManager::Get();
		var Variable = ConsoleManager->FindConsoleVariable(*Name);
		Variable->Set(Value);
	}

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnCloud9SettingsChanged OnChanged;

	void InitializeCVars();

	template <typename ValueType> requires Concepts::is_any_of<ValueType, int, float, bool, FString>
	IConsoleObject* RegisterConsoleVariable(ValueType& ValueRef, const FString& Name, const FString& Help)
	{
		let ConsoleManager = &IConsoleManager::Get();
		let CVar = ConsoleManager->RegisterConsoleVariableRef(*Name, ValueRef, *Help);
		CVar->AsVariable()->SetOnChangedCallback(
			FConsoleVariableDelegate::CreateLambda([this](IConsoleVariable* Arg) { Save(); })
		);
		return CVar;
	}

	IConsoleObject* RegisterConsoleVariable(FKey& ValueRef, const FString& Name, const FString& Help)
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
			})
		);
		// Execute bind command to update InputSettings file
		CCom->Execute({ValueRef.GetFName().ToString()}, GetWorld(), *GLog);
		return CCom;
	}
};

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

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Console/Entries/FVectorConsoleEntry.h"

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

UCLASS(Config=Game, defaultconfig, BlueprintType, Blueprintable)
class CLOUD9_API UCloud9DeveloperSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
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
	static inline FString DecalFadeScreenSizeName = "r.DecalFadeScreenSize";
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

	static inline FString CrosshairSizeName = "r.CrosshairSize";
	static inline FString CrosshairLengthName = "r.CrosshairLength";
	static inline FString CrosshairWidthName = "r.CrosshairWidth";
	static inline FString CrosshairGapName = "r.CrosshairGap";
	static inline FString CrosshairColorName = "r.CrosshairColor";

	static inline FString SensitivityName = "r.Sensitivity";
	static inline FString IsWindowsInputEnabledName = "r.IsWindowsInputEnabled";

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawHitCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 IsDrawDeprojectedCursorLine;

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

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Crosshair)
	int32 CrosshairSize;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Crosshair)
	float CrosshairLength;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Crosshair)
	float CrosshairWidth;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Crosshair)
	float CrosshairGap;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Crosshair)
	FVector CrosshairColor;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Mouse)
	float Sensitivity;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Mouse)
	int32 IsWindowsInputEnabled;

	UFUNCTION(BlueprintCallable, Category=Settings, DisplayName=GetCloud9DeveloperSettings)
	static UCloud9DeveloperSettings* Get();

	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION(BlueprintCallable)
	bool SetVariableValueByName(const FString& Name, const FString& Value);

	UFUNCTION(BlueprintCallable)
	FString GetVariableValueByName(const FString& Name);

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnCloud9SettingsChanged OnChanged;

	TMap<FString, TSharedPtr<FConsoleEntry>> ConsoleEntries;

	void InitializeCVars();

	template <typename ConsoleEntryType>
	bool RegisterConsoleVariable(const TSharedRef<ConsoleEntryType>& ConsoleEntry);

	bool RegisterConsoleVariable(int32& ValueRef, const FString& Name, const FString& Help);
	bool RegisterConsoleVariable(float& ValueRef, const FString& Name, const FString& Help);
	bool RegisterConsoleVariable(FVector& ValueRef, const FString& Name, const FString& Help);
	bool RegisterConsoleVariable(FKey& ValueRef, const FString& Name, const FString& Help);
};

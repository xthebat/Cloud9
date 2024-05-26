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

#include "Cloud9/Tools/Macro/Common.h"

#include "Cloud9DeveloperSettings.generated.h"

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
	static FString ShowMouseCursorName;
	static FString DrawDeprojectedCursorLineName;
	static FString DrawHitCursorLineName;
	static FString DrawExplosionSphereName;
	static FString DrawHitScanName;
	static FString PrintHitScanInfoName;
	static FString NetGraphName;
	static FString AutoSelectWeaponName;
	static FString InfiniteAmmoName;
	static FString CheatsName;
	static FString SelfAimEnabledName;
	static FString CameraVerticalSpeedLagName;
	static FString NoInaccuracyName;
	static FString JumpImpulseName;
	static FString WeaponAirSpreadScaleName;
	static FString WeaponDebugMaxInaccuracyName;
	static FString WeaponDebugInaccuracyOnlyUpName;
	static FString WeaponRecoilDecayCoefficientName;
	static FString DrawShotDirectionAxisName;
	static FString WeaponDebugDamageInfoName;
	static FString TaggingScaleName;
	static FString DecalLifeSpanName;
	static FString DecalFadeScreenSizeName;
	static FString VolumeName;

	static inline FString MainMenuMusicVolumeName = "r.MainMenuMusicVolume";

	// properties
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

protected: // functions
	void InitializeCVars();

	template <typename TValue>
	auto RegisterConsoleVariable(TValue& ValueRef, const TCHAR* Name, const TCHAR* Help);
};

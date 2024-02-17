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

#include "Cloud9DeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FUnUsedStruct
{
	GENERATED_BODY()

public:
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

public: // properties
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsDrawHitCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsDrawDeprojectedCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsShowMouseCursor;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsDrawExplosionSpheres;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsDrawHitScan;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsPrintHitScanInfo;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	float CameraVerticalSpeedLag;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 NetGraph;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsAutoSelectWeapon;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsInfiniteAmmo;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	int32 bIsCheatsEnabled;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Sound)
	float Volume;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	EUnUsedEnum UnUsedEnum;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	FUnUsedStruct UnUsedStruct;

public: // static functions
	UFUNCTION(BlueprintCallable, Category=Settings, DisplayName=GetCloud9DeveloperSettings)
	static const UCloud9DeveloperSettings* Get();

public: // functions
	UFUNCTION(BlueprintCallable)
	void Save();

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected: // functions
	void InitializeCVars();

	template <typename TValue>
	auto RegisterConsoleVariable(TValue& ValueRef, const TCHAR* Name, const TCHAR* Help);
};

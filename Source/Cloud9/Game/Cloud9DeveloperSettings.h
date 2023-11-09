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
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug, meta=(
		ConsoleVariable="r.IsDrawHitCursorLine",
		DisplayName="IsDrawHitCursorLine",
		ToolTip="Whether to draw line from character to GetHitUnderCursor point"))
	int32 bIsDrawHitCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug, meta=(
		ConsoleVariable="r.IsDrawDeprojectedCursorLine",
		DisplayName="IsDrawDeprojectedCursorLine",
		ToolTip="Whether to draw line from character to deprojected mouse cursor"))
	int32 bIsDrawDeprojectedCursorLine;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug, meta=(
		ConsoleVariable="r.IsShowMouseCursor",
		DisplayName="IsShowMouseCursor",
		ToolTip="Whether to show mouse cursor on screen or not in game"))
	int32 bIsShowMouseCursor;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug, meta=(
		ConsoleVariable="r.NetGraph",
		DisplayName="NetGraph",
		ToolTip="Whether to show FPS and other specific debug info"))
	int32 NetGraph;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	EUnUsedEnum UnUsedEnum;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category=Debug)
	FUnUsedStruct UnUsedStruct;

public: // static
	UFUNCTION(BlueprintCallable, Category=Settings, DisplayName=GetCloud9DeveloperSettings)
	static const UCloud9DeveloperSettings* Get();

public: // function
	void InitializeCVars();

	// UFUNCTION(BlueprintCallable)
	// void Save();

	UFUNCTION(BlueprintCallable)
	void Log() const;

	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};

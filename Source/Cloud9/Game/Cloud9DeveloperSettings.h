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

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Save Game Settings"))
class CLOUD9_API UCloud9DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Settings)
	static const UCloud9DeveloperSettings* GetCloud9DeveloperSettings();

	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION(BlueprintCallable)
	void Log() const;

	// Debug

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawHitCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawDeprojectedCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsShowMouseCursor;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int NetGraph;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	EUnUsedEnum UnUsedEnum;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FUnUsedStruct UnUsedStruct;
};

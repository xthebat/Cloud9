// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9KeyLibrary.generated.h"

UCLASS()
class CLOUD9_API UCloud9KeyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FKey TextToKey(const FText& Text);

	UFUNCTION(BlueprintCallable)
	static FText KeyToText(const FKey& Key);

	UFUNCTION(BlueprintCallable)
	static bool IsKeyNameValid(const FText& Text);
};

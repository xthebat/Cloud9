// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9ArrayLibrary.generated.h"

UCLASS()
class CLOUD9_API UCloud9ArrayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static TArray<FString> SortArrayOfString(const TArray<FString>& Array);

	UFUNCTION(BlueprintCallable)
	static TArray<int32> SortArrayOfInt32(const TArray<int32>& Array);
};

// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9StringLibrary.generated.h"

UCLASS()
class CLOUD9_API UCloud9StringLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FString FloatToString(float Value, int32 Precision = 3);

	UFUNCTION(BlueprintCallable)
	static FString IntToString(int32 Value);

	UFUNCTION(BlueprintCallable)
	static FString VectorToString(const FVector& Vector, int32 Precision = 3);

	UFUNCTION(BlueprintCallable)
	static float StringToFloat(const FString& String);

	UFUNCTION(BlueprintCallable)
	static float StringToInt(const FString& String);

	UFUNCTION(BlueprintCallable)
	static FVector StringToVector(const FString& String);

	UFUNCTION(BlueprintCallable)
	static bool IsStringContainsFloat(const FString& String);

	UFUNCTION(BlueprintCallable)
	static bool IsStringContainsInt(const FString& String);

	UFUNCTION(BlueprintCallable)
	static bool IsStringContainsVector(const FString& String);

	UFUNCTION(BlueprintCallable)
	static bool IsStringContainsKey(const FString& String);

	UFUNCTION(BlueprintCallable)
	static FString BoolToIntString(bool Value);

	UFUNCTION(BlueprintCallable)
	static bool IntStringToBool(const FString& String);

	static FString SanitizeFloatString(const FString& String);
};

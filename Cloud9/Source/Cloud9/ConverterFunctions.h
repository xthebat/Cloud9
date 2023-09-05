// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ConverterFunctions.generated.h"

/**
 * 
 */
UCLASS()
class CLOUD9_API UConverterFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag);

	UFUNCTION(BlueprintCallable)
	static float CollisionVolumeScale(UStaticMesh* StaticMesh);
};

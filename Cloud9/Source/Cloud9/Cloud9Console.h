// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9Console.generated.h"

class ACloud9GameMode;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class CLOUD9_API ACloud9Console : public AActor
{
	GENERATED_BODY()

public:
	ACloud9Console();
	
	UFUNCTION(BlueprintCallable)
	static UWorld* GetMyWorld();

	UFUNCTION(BlueprintCallable)
	static ACloud9GameMode* GetMyGameMode();
};

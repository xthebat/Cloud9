// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "Cloud9SpectatorPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(config=Game, Blueprintable)
class CLOUD9_API ACloud9SpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

	static inline const FName SpringArmComponentName = TEXT("SpringArmComponent");
	static inline const FName CameraComponentName = TEXT("CameraComponent");

	ACloud9SpectatorPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	UCameraComponent* CameraComponent;
};

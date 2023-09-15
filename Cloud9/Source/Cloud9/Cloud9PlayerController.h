// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cloud9PlayerController.generated.h"

class ACloud9Character;

UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACloud9PlayerController();

protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void CrouchPressed();

	void WalkReleased();
	void CrouchReleased();
};



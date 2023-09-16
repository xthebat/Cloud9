// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cloud9PlayerController.generated.h"

class ACloud9Console;
class ACloud9Character;

UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACloud9PlayerController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Classes)
	TSubclassOf<ACloud9Console> ConsoleClass;

	UPROPERTY(BlueprintReadOnly)
	ACloud9Console* Console;
	
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void CrouchPressed();

	void WalkReleased();
	void CrouchReleased();

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
};

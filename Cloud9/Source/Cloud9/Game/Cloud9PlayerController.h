// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cloud9PlayerController.generated.h"

class UCloud9ConsoleComponent;
class ACloud9Character;

UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACloud9PlayerController();

	// UFUNCTION(BlueprintPure)
	// UCloud9ConsoleComponent* GetConsole() const;

private:  // variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Console, meta = (AllowPrivateAccess = "true"))
	UCloud9ConsoleComponent* Console;
	
protected:  // override section
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

protected:  // new function
	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void CrouchPressed();

	void WalkReleased();
	void CrouchReleased();
};

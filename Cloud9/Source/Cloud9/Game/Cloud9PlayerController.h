// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cloud9PlayerController.generated.h"

class UCloud9KeyboardController;
class UCloud9MouseController;
class UCloud9ConsoleComponent;
class ACloud9Character;


UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACloud9PlayerController();

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Console, meta = (AllowPrivateAccess = "true"))
	UCloud9ConsoleComponent* Console;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controllers, meta = (AllowPrivateAccess = "true"))
	UCloud9MouseController* MouseController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controllers, meta = (AllowPrivateAccess = "true"))
	UCloud9KeyboardController* KeyboardController;
};
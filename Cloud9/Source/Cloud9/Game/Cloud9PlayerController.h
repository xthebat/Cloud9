// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Cloud9PlayerController.generated.h"

class UCloud9ConsoleComponent;
class ACloud9Character;

enum class EMouseMode
{
	Aiming = 0,
	Rotation = 1,
};

UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACloud9PlayerController();

private:  // variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Console, meta = (AllowPrivateAccess = "true"))
	UCloud9ConsoleComponent* Console;

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float RotateSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MinZoomHeight;
	
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MaxZoomHeight;
	
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsSmoothZoom;

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float SmoothZoomSpeed;

private:
	FVector2D MousePositionBase;
	float TargetZoomHeight;
	EMouseMode MouseMode;
	
protected:  // override section
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

protected:  // new function
	ACloud9Character* GetMyPawn() const;
	FVector2D GetMousePosition() const;
	bool IsZoomInProcess() const;
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void CrouchPressed();

	void WalkReleased();
	void CrouchReleased();

	void CameraZoom(float Value);
	void CameraRotationPressed();
	void CameraRotationReleased();
};

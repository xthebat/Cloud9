// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9PlayerController.h"

#include "Cloud9Character.h"
#include "Engine/World.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ACloud9PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ACloud9PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACloud9PlayerController::MoveRight);

	InputComponent->BindAction("Walk", IE_Pressed, this, &ACloud9PlayerController::WalkPressed);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ACloud9PlayerController::CrouchPressed);

	InputComponent->BindAction("Walk", IE_Released, this, &ACloud9PlayerController::WalkReleased);
	InputComponent->BindAction("Crouch", IE_Released, this, &ACloud9PlayerController::CrouchReleased);
}

void ACloud9PlayerController::MoveForward(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn<ACloud9Character>()->AddMovementInput(FVector::XAxisVector, Value);
}

void ACloud9PlayerController::MoveRight(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn<ACloud9Character>()->AddMovementInput(FVector::YAxisVector, Value);
}

void ACloud9PlayerController::WalkPressed() { GetPawn<ACloud9Character>()->Sneak(); }

void ACloud9PlayerController::CrouchPressed() { GetPawn<ACloud9Character>()->Crouch(false); }

void ACloud9PlayerController::WalkReleased() { GetPawn<ACloud9Character>()->UnSneak(); }

void ACloud9PlayerController::CrouchReleased() { GetPawn<ACloud9Character>()->UnCrouch(false); }

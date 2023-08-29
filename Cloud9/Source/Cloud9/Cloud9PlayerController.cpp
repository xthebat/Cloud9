// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cloud9PlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
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

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
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
	const auto MyPawn = Cast<ACloud9Character>(GetPawn());

	if (IsValid(MyPawn))
	{
		if (MyPawn->bIsWalk)
		{
			Value *= 0.52;
		}
		else if (MyPawn->bIsCrouch)
		{
			Value *= 0.34;
		}

		MyPawn->AddMovementInput({1, 0, 0}, Value);
	}
}

void ACloud9PlayerController::MoveRight(float Value)
{
	const auto MyPawn = Cast<ACloud9Character>(GetPawn());

	if (IsValid(MyPawn))
	{
		if (MyPawn->bIsWalk)
		{
			Value *= 0.52;
		}
		else if (MyPawn->bIsCrouch)
		{
			Value *= 0.34;
		}

		MyPawn->AddMovementInput({0, 1, 0}, Value);
	}
}

void ACloud9PlayerController::WalkPressed()
{
	Cast<ACloud9Character>(GetPawn())->bIsWalk = true;
}

void ACloud9PlayerController::CrouchPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Hello"));
	Cast<ACloud9Character>(GetPawn())->bIsCrouch = true;
}

void ACloud9PlayerController::WalkReleased()
{
	Cast<ACloud9Character>(GetPawn())->bIsWalk = false;
}

void ACloud9PlayerController::CrouchReleased()
{
	Cast<ACloud9Character>(GetPawn())->bIsCrouch = false;
}

void ACloud9PlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ACloud9Character* MyPawn = Cast<ACloud9Character>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(
					this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void ACloud9PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ACloud9PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ACloud9PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ACloud9PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

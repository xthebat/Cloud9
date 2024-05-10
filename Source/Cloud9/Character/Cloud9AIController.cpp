// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9AIController.h"

#include "Cloud9/Tools/Extensions/UWorld.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

ACloud9AIController::ACloud9AIController()
{
	BaseLocation = FVector::ZeroVector;
	MovingOffset = FVector::ZeroVector;
	ChangeDirectionDelay = 0.5f;
}

void ACloud9AIController::SetMovingOffset(FVector Value)
{
	GetWorld() | EUWorld::ClearTimer{TimerHandle};
	SetMovingOffsetEx(Value);
}

void ACloud9AIController::SetMovingOffsetEx(FVector Value)
{
	MovingOffset = Value;
	Move(BaseLocation + MovingOffset);
}

void ACloud9AIController::SetChangeDirectionDelay(float Value)
{
	ChangeDirectionDelay = Value;
}

void ACloud9AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (not MovingOffset.IsZero())
	{
		TimerHandle = GetWorld() | EUWorld::AsyncAfter{
			[this] { SetMovingOffsetEx(-MovingOffset); },
			ChangeDirectionDelay
		};
	}
}

void ACloud9AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AssertOrVoid(InPawn, Error, "Pawn is invalid");
	BaseLocation = InPawn->GetActorLocation();
}

void ACloud9AIController::OnUnPossess()
{
	Super::OnUnPossess();
	Destroy();
}

void ACloud9AIController::Move(FVector Destination)
{
	MoveToLocation(
		Destination,
		-1,
		false,
		false,
		false,
		true);
}

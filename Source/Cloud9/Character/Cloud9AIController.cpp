// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9AIController.h"

#include "Cloud9/Tools/Extensions/UObject.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "GameFramework/Character.h"

ACloud9AIController::ACloud9AIController()
{
	BaseLocation = FVector::ZeroVector;
	MovingOffset = FVector::ZeroVector;
	ChangeDirectionDelay = 0.5f;
}

void ACloud9AIController::BeginPlay()
{
	Super::BeginPlay();
	let MyCharacter = GetCharacter();
	CheckIsValid(MyCharacter, Error, "Character is invalid")
	BaseLocation = MyCharacter->GetActorLocation();
}

void ACloud9AIController::SetMovingOffset(FVector Value)
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
		this | EUObject::AsyncAfter{
			[this] { SetMovingOffset(-MovingOffset); },
			ChangeDirectionDelay
		};
	}
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

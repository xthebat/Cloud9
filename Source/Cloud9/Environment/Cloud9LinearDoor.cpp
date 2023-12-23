// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.


#include "Cloud9LinearDoor.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/FTransform.h"
#include "Cloud9/Tools/Extensions/FVector.h"

ACloud9LinearDoor::ACloud9LinearDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsOpen = false;
	Speed = 1.0f;
	Distance = 0.0f;
	Direction = EDirection::Right;

	bIsMoving = false;
	DirectionVector = FVector::ZeroVector;

	SetMobility(EComponentMobility::Movable);
}

void ACloud9LinearDoor::Open()
{
	if (not bIsOpen)
	{
		Toggle();
	}
}

void ACloud9LinearDoor::Close()
{
	if (bIsOpen)
	{
		Toggle();
	}
}

void ACloud9LinearDoor::Toggle()
{
	bIsMoving = true;
	Shift = (TargetPosition - OriginPosition) * Speed;
}

void ACloud9LinearDoor::BeginPlay()
{
	Super::BeginPlay();

	let Transform = GetActorTransform();

	DirectionVector = Transform
		| EFTransform::InverseTransformVector{this | EAActor::ToDirectionVector{Direction}}
		| EFVector::Normalize{};

	if (DirectionVector.IsZero())
	{
		SetActorTickEnabled(false);
		log(Error, "Door '%s' can't moved due to incorrect direction vector", *GetName());
	}

	if (Speed <= 0.0f)
	{
		SetActorTickEnabled(false);
		log(Warning, "Door '%s' can't moved due to speed <= 0.0f", *GetName());
	}

	if (Distance <= 0.0f)
	{
		var Origin = FVector::ZeroVector;
		var Extents = FVector::ZeroVector;
		GetActorBounds(true, Origin, Extents, true);
		Distance = (2.0f * Extents * DirectionVector).Size();
	}

	let ActualDistance = Distance - Extent;

	log(Display, "Door '%s' distance = '%f'", *GetName(), ActualDistance);

	let Sign = bIsOpen ? -1 : 1;
	OriginPosition = Transform.GetLocation();
	TargetPosition = OriginPosition + Sign * DirectionVector * ActualDistance;
}

void ACloud9LinearDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		var NewPosition = GetActorLocation() + Shift * DeltaTime;

		if (FVector::DistSquared(OriginPosition, GetActorLocation()) > FMath::Square(Distance - Extent))
		{
			bIsOpen = not bIsOpen;
			bIsMoving = false;
			NewPosition = TargetPosition;
			TargetPosition = OriginPosition;
			OriginPosition = NewPosition;
		}

		SetActorLocation(NewPosition);
	}
}

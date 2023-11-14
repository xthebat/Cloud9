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

#include "Cloud9/Cloud9.h"

ACloud9LinearDoor::ACloud9LinearDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsOpen = false;
	Direction = FVector::ZeroVector;
	Speed = 1.0f;
	Distance = 0.0f;
	bIsMoving = false;

	SetMobility(EComponentMobility::Movable);
}

void ACloud9LinearDoor::Open()
{
	if (!bIsOpen)
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

	let Sign = bIsOpen ? -1 : 1;
	OriginPosition = GetActorLocation();
	TargetPosition = OriginPosition + Sign * Direction * Distance;

	if (Speed == 0.0f || Distance == 0.0f || Direction.IsZero())
	{
		SetActorTickEnabled(false);
		UE_LOG(LogCloud9, Warning, TEXT("Door %s can't moved due to incorrect properties"), *GetName());
	}
}

void ACloud9LinearDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		var NewPosition = GetActorLocation() + Shift * DeltaTime;

		if (FVector::DistSquared(OriginPosition, GetActorLocation()) > FMath::Square(Distance))
		{
			bIsOpen = !bIsOpen;
			bIsMoving = false;
			NewPosition = TargetPosition;
			TargetPosition = OriginPosition;
			OriginPosition = NewPosition;
		}

		SetActorLocation(NewPosition);
	}
}

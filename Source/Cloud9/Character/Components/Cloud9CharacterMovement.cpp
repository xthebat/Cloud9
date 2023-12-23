﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9CharacterMovement.h"
#include "Cloud9/Character/Cloud9Character.h"

UCloud9CharacterMovement::UCloud9CharacterMovement()
{
	RotationLag = 30.0f;
	TargetRotator = FRotator::ZeroRotator;

	MaxWalkSpeed = 500.0f;
	MaxSneakSpeed = 270.0f;
	MaxWalkSpeedCrouched = 170.0f;

	var& NavAgentProperties = GetNavAgentPropertiesRef();
	NavAgentProperties.bCanCrouch = true;
	NavAgentProperties.bCanSwim = false;
	NavAgentProperties.bCanFly = false;
}

ACloud9Character* UCloud9CharacterMovement::GetCloud9CharacterOwner() const
{
	return IsValid(CharacterOwner) ? Cast<ACloud9Character>(CharacterOwner) : nullptr;
}

bool UCloud9CharacterMovement::IsSneaking() const
{
	let Owner = GetCloud9CharacterOwner();
	return IsValid(Owner) && Owner->bIsSneaking;
}

void UCloud9CharacterMovement::Sneak() const
{
	if (let Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		Owner->bIsSneaking = true;
	}
}

void UCloud9CharacterMovement::UnSneak() const
{
	if (let Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		Owner->bIsSneaking = false;
	}
}

void UCloud9CharacterMovement::Rotate(FRotator Rotator, bool Instant)
{
	TargetRotator = Rotator;

	if (let Owner = GetCloud9CharacterOwner(); Instant && IsValid(Owner))
	{
		Owner->SetActorRotation(TargetRotator);
	}
}

float UCloud9CharacterMovement::GetMaxSpeed() const
{
	let MaxSpeed = Super::GetMaxSpeed();
	return IsSneaking() ? FMath::Min(MaxSneakSpeed, MaxSpeed) : MaxSpeed;
}

void UCloud9CharacterMovement::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (let Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		var NewRotation = TargetRotator;
		if (RotationLag != 0.0f)
		{
			let ActorRotation = Owner->GetActorRotation();
			NewRotation = FMath::Lerp(ActorRotation, TargetRotator, DeltaTime / RotationLag * RotationLagScale);
		}
		Owner->SetActorRotation(NewRotation);
	}
}

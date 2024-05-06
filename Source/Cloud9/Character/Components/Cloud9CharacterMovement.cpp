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

#include "Cloud9CharacterMovement.h"

#include "Cloud9InventoryComponent.h"

#include "Cloud9/Tools/Math.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Character/Cloud9Character.h"

UCloud9CharacterMovement::UCloud9CharacterMovement()
{
	RotationLag = 30.0f;
	TargetRotator = FRotator::ZeroRotator;

	SpeedRun = MaxPlayerSpeed::NoWeapon;
	SpeedVip = MaxPlayerSpeed::Vip;
	SpeedShield = MaxPlayerSpeed::Shield;
	SpeedHasHostage = MaxPlayerSpeed::HasHostage;
	SpeedStopped = MaxPlayerSpeed::Stopped;
	SpeedObserver = MaxPlayerSpeed::Observer;

	SpeedDuckModifier = 0.34f;
	SpeedWalkModifier = 0.52f;
	SpeedClimbModifier = 0.34f;

	FlinchVelocityModifier = 1.0f;

	MaxWalkSpeed = SpeedRun;
	MaxWalkSpeedCrouched = MaxWalkSpeed * SpeedDuckModifier;
	MaxAcceleration = GetMovementValue(AccelerationWalkScaleCoefficient, AccelerationDuckScaleCoefficient);

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

bool UCloud9CharacterMovement::IsOnLadder() const
{
	// TODO: Implement Ladder
	return false;
}

float UCloud9CharacterMovement::GetMovementValue(float WalkScale, float DuckScale) const
{
	var MaxSpeed = MaxPlayerSpeed::NoWeapon;

	if (let Character = GetOwner<ACloud9Character>(); IsValid(Character))
	{
		if (let Inventory = Character->GetInventoryComponent(); IsValid(Inventory))
		{
			if (let SelectedWeapon = Inventory->GetSelectedWeapon(); IsValid(SelectedWeapon))
			{
				MaxSpeed = SelectedWeapon->GetWeaponInfo()->MaxPlayerSpeed;
			}
		}
	}

	if (IsCrouching())
	{
		return MaxSpeed * SpeedDuckModifier * DuckScale;
	}

	if (IsOnLadder())
	{
		MaxSpeed *= SpeedClimbModifier;
	}

	MaxSpeed *= WalkScale;

	return IsSneaking() ? MaxSpeed * SpeedWalkModifier : MaxSpeed;
}

void UCloud9CharacterMovement::UpdateFlinchVelocityModifier(float FlinchModSmall, float FlinchModLarge)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	// logic and coefficients from cstrike15

	// apply the minimum large flinch amount on the first hit and on subsequent hits, 
	// apply a portion of the small amount - less as we apply more
	let NewFlinchModeLarge = FlinchModLarge - (1.0f - FlinchVelocityModifier) * FlinchModSmall;
	FlinchVelocityModifier = FMath::Min(FlinchVelocityModifier, FMath::Min(FlinchModLarge, NewFlinchModeLarge));

	var FlinchModifier = FlinchVelocityModifier;

	// scale this by a global convar value
	FlinchModifier *= Settings->TaggingScale;

	// get the player's current max speed based on their weapon
	let InventoryComponent = GetCloud9CharacterOwner()->GetInventoryComponent();
	let SelectedWeapon = InventoryComponent->GetSelectedWeapon();
	let WeaponMaxSpeed = IsValid(SelectedWeapon)
		                     ? SelectedWeapon->GetWeaponInfo()->MaxPlayerSpeed
		                     : MaxPlayerSpeed::Knife;

	// this is the value we use to scale the above fFlinchModifier - 
	// knives receive less; AKs receive a bit more, etc.
	constexpr let WeaponScaleMin = 0.15f;
	let WeaponScaleMax = (WeaponMaxSpeed - 120.0f) / (MaxPlayerSpeed::Knife - 120.0f);
	let LocalWeaponScale = FMath::Max(WeaponScaleMin, WeaponScaleMax) * 0.8f + 0.08f;
	FlinchModifier = FlinchModifier * LocalWeaponScale;

	// the held weapon also determines what the tagging cap should be
	// since it's accumulative; we want to be able to cap it, so we don't keep getting more
	// tagged the more someone shoots us
	constexpr let RatioMin = 1.0f;
	let RatioMax = (WeaponMaxSpeed - 80.0f) / (MaxPlayerSpeed::Knife - 80.0f);
	let Ratio = FMath::Min(RatioMin, RatioMax) * 1.2f - 0.08f;
	let ClampMin = FMath::Max(0.2f, Ratio / 4.0f);
	let ClampMax = FlinchModLarge > 0.65f ? FlinchModLarge : 0.65f;
	FlinchModifier = FMath::Clamp(FlinchModifier, ClampMin, ClampMax);

	// this function only allows more flinch (smaller values) to be applied, not less
	FlinchVelocityModifier = FMath::Min(FlinchVelocityModifier, FlinchModifier);
}

void UCloud9CharacterMovement::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// lerp decay a flinch modifier too slowly, so use the original cstrike15 method
	if (FlinchVelocityModifier < 1.0f)
	{
		FlinchVelocityModifier = Math::Approach(1.0, FlinchVelocityModifier, DeltaTime * 0.35);
	}

	FlinchVelocityModifier = FMath::Clamp(FlinchVelocityModifier, 0.1f, 1.0f);

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

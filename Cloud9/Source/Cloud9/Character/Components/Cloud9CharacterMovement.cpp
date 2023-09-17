#include "Cloud9CharacterMovement.h"
#include "Cloud9/Character/Cloud9Character.h"

UCloud9CharacterMovement::UCloud9CharacterMovement()
{
	MaxWalkSpeed = 500.0f;
	MaxSneakSpeed = 270.0f;
	MaxWalkSpeedCrouched = 170.0f;
	auto& NavAgentProperties = GetNavAgentPropertiesRef();
	NavAgentProperties.bCanCrouch = true;
	NavAgentProperties.bCanSwim = false;
	NavAgentProperties.bCanFly = false;
}

ACloud9Character* UCloud9CharacterMovement::GetMyCharacterOwner() const
{
	if (CharacterOwner == nullptr)
		return nullptr;

	return Cast<ACloud9Character>(CharacterOwner);
}

bool UCloud9CharacterMovement::IsSneaking() const
{
	return CharacterOwner && GetMyCharacterOwner()->bIsSneaking;
}

void UCloud9CharacterMovement::Sneak() const
{
	if (CharacterOwner)
		GetMyCharacterOwner()->bIsSneaking = true;
}

void UCloud9CharacterMovement::UnSneak() const
{
	if (CharacterOwner)
		GetMyCharacterOwner()->bIsSneaking = false;
}

float UCloud9CharacterMovement::GetMaxSpeed() const
{
	const auto MaxSpeed = Super::GetMaxSpeed();
	if (IsSneaking())
		return FMath::Min(MaxSneakSpeed, MaxSpeed);
	return MaxSpeed;
}

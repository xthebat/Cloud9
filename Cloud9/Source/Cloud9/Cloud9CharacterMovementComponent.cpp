#include "Cloud9CharacterMovementComponent.h"

#include "Cloud9Character.h"

UCloud9CharacterMovementComponent::UCloud9CharacterMovementComponent()
{
	MaxWalkSpeed = 500.0f;
	MaxSneakSpeed = 270.0f;
	MaxWalkSpeedCrouched = 170.0f;
	auto& NavAgentProperties = GetNavAgentPropertiesRef();
	NavAgentProperties.bCanCrouch = true;
	NavAgentProperties.bCanSwim = false;
	NavAgentProperties.bCanFly = false;
}

ACloud9Character* UCloud9CharacterMovementComponent::GetMyCharacterOwner() const
{
	if (CharacterOwner == nullptr)
		return nullptr;

	return Cast<ACloud9Character>(CharacterOwner);
}

bool UCloud9CharacterMovementComponent::IsSneaking() const
{
	return CharacterOwner && GetMyCharacterOwner()->bIsSneaking;
}

void UCloud9CharacterMovementComponent::Sneak() const
{
	if (CharacterOwner)
		GetMyCharacterOwner()->bIsSneaking = true;
}

void UCloud9CharacterMovementComponent::UnSneak() const
{
	if (CharacterOwner)
		GetMyCharacterOwner()->bIsSneaking = false;
}

float UCloud9CharacterMovementComponent::GetMaxSpeed() const
{
	const auto MaxSpeed = Super::GetMaxSpeed();
	if (IsSneaking())
		return FMath::Min(MaxSneakSpeed, MaxSpeed);
	return MaxSpeed;
}

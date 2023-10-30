#include "Cloud9CharacterMovement.h"
#include "Cloud9/Character/Cloud9Character.h"

UCloud9CharacterMovement::UCloud9CharacterMovement()
{
	RotationLag = 30.0f;
	TargetRotator = FRotator::ZeroRotator;

	MaxWalkSpeed = 500.0f;
	MaxSneakSpeed = 270.0f;
	MaxWalkSpeedCrouched = 170.0f;

	auto& NavAgentProperties = GetNavAgentPropertiesRef();
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
	const auto Owner = GetCloud9CharacterOwner();
	return IsValid(Owner) && Owner->bIsSneaking;
}

void UCloud9CharacterMovement::Sneak() const
{
	if (const auto Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		Owner->bIsSneaking = true;
	}
}

void UCloud9CharacterMovement::UnSneak() const
{
	if (const auto Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		Owner->bIsSneaking = false;
	}
}

void UCloud9CharacterMovement::Rotate(FRotator Rotator, bool Instant)
{
	TargetRotator = Rotator;

	if (const auto Owner = GetCloud9CharacterOwner(); Instant && IsValid(Owner))
	{
		Owner->SetActorRotation(TargetRotator);
	}
}

float UCloud9CharacterMovement::GetMaxSpeed() const
{
	const auto MaxSpeed = Super::GetMaxSpeed();
	return IsSneaking() ? FMath::Min(MaxSneakSpeed, MaxSpeed) : MaxSpeed;
}

void UCloud9CharacterMovement::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (const auto Owner = GetCloud9CharacterOwner(); IsValid(Owner))
	{
		auto NewRotation = TargetRotator;
		if (RotationLag != 0.0f)
		{
			const auto ActorRotation = Owner->GetActorRotation();
			NewRotation = FMath::Lerp(ActorRotation, TargetRotator, DeltaTime / RotationLag * RotationLagScale);
		}
		Owner->SetActorRotation(NewRotation);
	}
}

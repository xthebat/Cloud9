#include "Cloud9SpringArmComponent.h"


UCloud9SpringArmComponent::UCloud9SpringArmComponent()
{
	RotationVerticalLagSpeed = 0.0f;
}

void UCloud9SpringArmComponent::OnRegister()
{
	Super::OnRegister();
	TargetRelativeRotation = RelativeSocketRotation.Rotator();
}

void UCloud9SpringArmComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto NewRotation = TargetRelativeRotation;
	if (RotationVerticalLagSpeed != 0.0f)
	{
		const auto OldRotation = RelativeSocketRotation.Rotator();
		NewRotation = FMath::Lerp(
			OldRotation,
			TargetRelativeRotation,
			DeltaTime / RotationVerticalLagSpeed * RotationScale);
	}

	RelativeSocketRotation = NewRotation.Quaternion();

	UpdateChildTransforms();
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9AnimationComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"

UCloud9AnimationComponent::UCloud9AnimationComponent() {}

UAnimInstance* UCloud9AnimationComponent::GetAnimInstance() const
{
	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), nullptr, Error, "Character is invalid");

	let Mesh = Character->GetMesh();
	AssertOrReturn(IsValid(Mesh), nullptr, Error, "Mesh is invalid");

	return Mesh->GetAnimInstance();
}

bool UCloud9AnimationComponent::PlayMontage(UAnimMontage* Montage, float StartTime, float Rate) const
{
	AssertOrReturn(IsValid(Montage), false, Error, "Montage is invalid");

	let AnimInstance = GetAnimInstance();
	AssertOrReturn(
		IsValid(AnimInstance), false,
		Error, "AnimInstance is invalid for montage '%s'",
		*Montage->GetName());

	AssertOrReturn(
		AnimInstance->Montage_Play(Montage, Rate, EMontagePlayReturnType::MontageLength, StartTime), false,
		Error, "Can't play montage '%s'", *Montage->GetName()
	);

	return true;
}

bool UCloud9AnimationComponent::IsAnyMontagePlaying() const
{
	let AnimInstance = GetAnimInstance();
	AssertOrReturn(IsValid(AnimInstance), false, Error, "AnimInstance is invalid");
	return AnimInstance->IsAnyMontagePlaying();
}

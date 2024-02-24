// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9AnimationComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"

UCloud9AnimationComponent::UCloud9AnimationComponent() {}

UAnimInstance* UCloud9AnimationComponent::GetAnimInstance() const
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Component='%s'] Character is invalid", *GetName());
		return nullptr;
	}

	let Mesh = Character->GetMesh();

	if (not IsValid(Mesh))
	{
		log(Error, "[Component='%s'] Mesh is invalid", *GetName());
		return nullptr;
	}

	return Mesh->GetAnimInstance();
}

bool UCloud9AnimationComponent::PlayMontage(UAnimMontage* Montage, float StartTime, float Rate) const
{
	if (not IsValid(Montage))
	{
		log(Error, "[Component='%s'] Montage is invalid", *GetName());
		return false;
	}

	let AnimInstance = GetAnimInstance();

	if (not IsValid(AnimInstance))
	{
		log(Error, "[Component='%s'] AnimInstance is invalid for montage '%s'", *GetName(), *Montage->GetName());
		return false;
	}

	if (not AnimInstance->Montage_Play(Montage, Rate, EMontagePlayReturnType::MontageLength, StartTime))
	{
		log(Error, "[Component='%s'] Can't play montage '%s'", *GetName(), *Montage->GetName());
		return false;
	}

	return true;
}

bool UCloud9AnimationComponent::IsAnyMontagePlaying() const
{
	let AnimInstance = GetAnimInstance();

	if (not IsValid(AnimInstance))
	{
		log(Error, "[Component='%s'] AnimInstance is invalid", *GetName());
		return false;
	}

	return AnimInstance->IsAnyMontagePlaying();
}

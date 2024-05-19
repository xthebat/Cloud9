// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9AnimationComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"

UCloud9AnimationComponent::UCloud9AnimationComponent()
{
	BasePoseMontage = nullptr;
	OtherPoseMontage = nullptr;
}

UAnimInstance* UCloud9AnimationComponent::GetAnimInstance() const
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), nullptr, Error, "Character is invalid");

	let Mesh = Character->GetMesh();
	OBJECT_RETURN_IF_FAIL(IsValid(Mesh), nullptr, Error, "Mesh is invalid");

	return Mesh->GetAnimInstance();
}

bool UCloud9AnimationComponent::PlayMontage(
	UAnimMontage* NewBasePoseMontage,
	UAnimMontage* NewOtherPoseMontage,
	float StartTime,
	float Rate)
{
	OBJECT_RETURN_IF_FAIL(IsValid(NewBasePoseMontage), false, Error, "Montage is invalid");

	let AnimInstance = GetAnimInstance();
	OBJECT_RETURN_IF_FAIL(
		IsValid(AnimInstance), false,
		Error, "AnimInstance is invalid for montage '%s'",
		*NewBasePoseMontage->GetName());

	OBJECT_RETURN_IF_FAIL(
		AnimInstance->Montage_Play(NewBasePoseMontage, Rate, EMontagePlayReturnType::MontageLength, StartTime), false,
		Error, "Can't play montage '%s'", *NewBasePoseMontage->GetName()
	);

	BasePoseMontage = NewBasePoseMontage;
	OtherPoseMontage = NewOtherPoseMontage;

	MontageEndedDelegate.BindUObject(this, &UCloud9AnimationComponent::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, BasePoseMontage);

	return true;
}

void UCloud9AnimationComponent::OnMontageEnded(UAnimMontage* Montage, bool IsInterrupted)
{
	BasePoseMontage = nullptr;
	OtherPoseMontage = nullptr;
}

bool UCloud9AnimationComponent::IsAnyMontagePlaying() const
{
	let AnimInstance = GetAnimInstance();
	OBJECT_RETURN_IF_FAIL(IsValid(AnimInstance), false, Error, "AnimInstance is invalid");
	return AnimInstance->IsAnyMontagePlaying();
}

void UCloud9AnimationComponent::StopMontage(const UAnimMontage* Montage) const
{
	let AnimInstance = GetAnimInstance();
	OBJECT_VOID_IF_FAIL(IsValid(AnimInstance), Error, "AnimInstance is invalid");

	if (let MontageInstance = AnimInstance->GetActiveInstanceForMontage(Montage))
	{
		MontageInstance->Stop(Montage->BlendOut);
	}
}

void UCloud9AnimationComponent::PoseChanged()
{
	let AnimInstance = GetAnimInstance();
	OBJECT_VOID_IF_FAIL(IsValid(AnimInstance), Error, "AnimInstance is invalid");

	if (let BaseMontage = AnimInstance->GetCurrentActiveMontage(); BaseMontage != nullptr)
	{
		let MontageInstance = AnimInstance->GetActiveInstanceForMontage(BaseMontage);
		let Position = MontageInstance->GetPosition();
		// Cache pose because OtherPoseMontage will be nulled on StopMontage
		let OtherMontage = OtherPoseMontage;
		StopMontage(BaseMontage);
		if (IsValid(OtherMontage))
		{
			PlayMontage(OtherMontage, BaseMontage, Position);
		}
	}
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9AnimationComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"

UCloud9AnimationComponent::UCloud9AnimationComponent() {}

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

	let MontageInstance = AnimInstance->GetActiveInstanceForMontage(NewBasePoseMontage);
	let InstanceID = MontageInstance->GetInstanceID();
	MontageInstance->OnMontageEnded.BindLambda(
		[this, InstanceID](UAnimMontage* Montage, bool IsInterrupted)
		{
			OBJECT_VERBOSE(
				"Montage stopped=%s InstanceID=%d Interrupted=%d",
				*Montage->GetName(), InstanceID, IsInterrupted);
			Montages.Remove(InstanceID);
		}
	);
	Montages.Add(InstanceID, NewOtherPoseMontage);
	OBJECT_VERBOSE("Montage started=%s InstanceID=%d", *NewBasePoseMontage->GetName(), InstanceID);

	return true;
}

void UCloud9AnimationComponent::StopAllMontages(float BlendOut) const
{
	let AnimInstance = GetAnimInstance();
	OBJECT_VOID_IF_FAIL(IsValid(AnimInstance), Error, "AnimInstance is invalid");
	AnimInstance->StopAllMontages(BlendOut);
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

UAnimMontage* UCloud9AnimationComponent::StopMontage(int32 InstanceID) const
{
	let AnimInstance = GetAnimInstance();
	OBJECT_RETURN_IF_FAIL(IsValid(AnimInstance), nullptr, Error, "AnimInstance is invalid");

	if (let MontageInstance = AnimInstance->GetMontageInstanceForID(InstanceID))
	{
		if (let Montage = MontageInstance->Montage; IsValid(Montage))
		{
			MontageInstance->Stop(Montage->BlendOut);
			return Montage;
		}
	}

	return nullptr;
}

void UCloud9AnimationComponent::PoseChanged()
{
	let AnimInstance = GetAnimInstance();
	OBJECT_VOID_IF_FAIL(IsValid(AnimInstance), Error, "AnimInstance is invalid");

	// Prevent modification during iteration
	for (let Copy = Montages; let [InstanceID, OtherMontage] : Copy)
	{
		let MontageInstance = AnimInstance->GetMontageInstanceForID(InstanceID);
		let Position = MontageInstance->GetPosition();
		let BaseMontage = StopMontage(InstanceID);
		OBJECT_VERBOSE("Pose changed ActiveMontage=%s InstanceID=%d", *BaseMontage->GetName(), InstanceID);
		PlayMontage(OtherMontage, BaseMontage, Position);
	}
}

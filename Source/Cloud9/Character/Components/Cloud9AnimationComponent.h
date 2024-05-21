// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cloud9AnimationComponent.generated.h"

class ACloud9Character;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9AnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9AnimationComponent();

	UAnimInstance* GetAnimInstance() const;

	bool PlayMontage(
		UAnimMontage* NewBasePoseMontage,
		UAnimMontage* NewOtherPoseMontage,
		float StartTime = 0.0f,
		float Rate = 1.0f);

	bool IsAnyMontagePlaying() const;
	void StopMontage(const UAnimMontage* Montage) const;
	UAnimMontage* StopMontage(int32 InstanceID) const;
	void StopAllMontages(float BlendOut) const;

	// TODO: Change with delegate and to untethered PlayerController with AnimationComponent  
	void PoseChanged();

protected:
	// This was made in an account that several montages can be played in same time,
	// but easier way is just stop all other montages for weapon deploy and reload
	// when the character pose changed.
	//
	// Currently left as is in order future modification.

	/** Montage instance ID to another pose montage */
	UPROPERTY()
	TMap<int32, UAnimMontage*> Montages;
};

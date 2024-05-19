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
	void PoseChanged();

protected:
	void OnMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

	FOnMontageEnded MontageEndedDelegate;

	UPROPERTY()
	UAnimMontage* BasePoseMontage;

	UPROPERTY()
	UAnimMontage* OtherPoseMontage;
};

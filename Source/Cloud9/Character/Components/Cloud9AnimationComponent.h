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

	bool PlayMontage(UAnimMontage* Montage, float StartTime = 0.0f, float Rate = 1.0f) const;

	bool IsAnyMontagePlaying() const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Cloud9CharacterMovementComponent.generated.h"


class ACloud9Character;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCloud9CharacterMovementComponent();

	ACloud9Character* GetMyCharacterOwner() const;
	
	bool IsSneaking() const;

	void Sneak() const;

	void UnSneak() const;

	UPROPERTY(Category="Character Movement: Sneaking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxSneakSpeed;

	virtual float GetMaxSpeed() const override;
};

#pragma once

#include "CoreMinimal.h"
#include "Cloud9CharacterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Cloud9CharacterMovement.generated.h"


class ACloud9Character;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9CharacterMovement
	: public UCharacterMovementComponent
	, public ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	UCloud9CharacterMovement();

	ACloud9Character* GetMyCharacterOwner() const;

	bool IsSneaking() const;

	void Sneak() const;

	void UnSneak() const;

	UPROPERTY(Category="Character Movement: Sneaking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxSneakSpeed;

	virtual float GetMaxSpeed() const override;
};

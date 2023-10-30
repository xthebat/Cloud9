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

	ACloud9Character* GetCloud9CharacterOwner() const;

	bool IsSneaking() const;
	void Sneak() const;
	void UnSneak() const;

	void Rotate(FRotator Rotator, bool Instant = false);

	UPROPERTY(Category="Character Movement: Sneaking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxSneakSpeed;

	virtual float GetMaxSpeed() const override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	static constexpr float RotationLagScale = 360.0f;

	/** Character rotation lag*/
	UPROPERTY(Category="Character Movement (Rotation Settings)", EditDefaultsOnly)
	float RotationLag;

	/** Target rotator of character*/
	FRotator TargetRotator;
};

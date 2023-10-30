#pragma once

#include "CoreMinimal.h"
#include "Cloud9CharacterComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Cloud9SpringArmComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9SpringArmComponent
	: public USpringArmComponent
	  , public ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	UCloud9SpringArmComponent();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void OnRegister() override;

private:
	static constexpr float RotationScale = 360.0f;

	/** Vertical rotation speed lag for camera*/
	UPROPERTY(EditDefaultsOnly, Category=Lag,
		meta=(ClampMin="0.0", ClampMax="10000.0", UIMin = "0.0", UIMax = "10000.0"))
	float RotationVerticalLagSpeed;

	FRotator TargetRelativeRotation;
};

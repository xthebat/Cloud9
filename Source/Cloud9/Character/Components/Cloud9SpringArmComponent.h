#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Cloud9SpringArmComponent.generated.h"


UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9SpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	// TODO: CameraLagRotator

	/**
	 * If bEnableCameraLag is true, controls how quickly camera reaches target position by each axis.
	 * Values of this vector multiplies with @CameraLagSpeed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag, meta=(editcondition="bEnableCameraLag"))
	FVector CameraLagVector;

public:
	UCloud9SpringArmComponent();

	virtual void UpdateDesiredArmLocation(
		bool bDoTrace,
		bool bDoLocationLag,
		bool bDoRotationLag,
		float DeltaTime
	) override;
};

#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerBase.h"
#include "Components/ActorComponent.h"
#include "Cloud9MouseController.generated.h"

class ACloud9Character;

enum class EMouseMode
{
	Aiming = 0,
	Rotation = 1,
};

UCLASS()
class CLOUD9_API UCloud9MouseController : public UCloud9ControllerBase
{
	GENERATED_BODY()

public:
	UCloud9MouseController();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Sensitivity, meta = (AllowPrivateAccess = "true"))
	float RotateSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = Sensitivity, meta = (AllowPrivateAccess = "true"))
	float ZoomSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MinZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MaxZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category = Smooth, meta = (AllowPrivateAccess = "true"))
	bool bIsSmoothZoom;

	UPROPERTY(EditDefaultsOnly, Category = Smooth, meta = (AllowPrivateAccess = "true"))
	float SmoothZoomSpeed;

	FVector2D GetMousePosition() const;

	bool IsZoomInProcess() const;
	float GetCameraZoomLevel() const;
	void SetCameraZoomLevel(float Value) const;

private:
	FVector2D MousePositionBase;
	float TargetZoomHeight;
	EMouseMode MouseMode;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	void CameraZoom(float Value);
	void CameraRotationPressed();
	void CameraRotationReleased();
};

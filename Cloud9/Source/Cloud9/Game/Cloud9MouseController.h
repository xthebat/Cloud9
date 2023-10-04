#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerComponent.h"
#include "Cloud9MouseController.generated.h"

class ACloud9Character;

enum class EMouseMode
{
	Aiming = 0,
	Rotation = 1,
};

UCLASS()
class CLOUD9_API UCloud9MouseController
	: public UActorComponent
	, public ICloud9ControllerComponent
{
	GENERATED_BODY()

public:
	UCloud9MouseController();
	
	void OnCameraZoom(float Value);
	void OnCameraRotationPressed();
	void OnCameraRotationReleased();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	FVector2D GetMousePosition() const;

	float GetCameraZoomHeightLevel() const;
	void SetCameraZoomLevel(float Value) const;

	void ProcessZoom(float DeltaTime);
	
private:
	static constexpr float InvalidCameraZoomLevel = -1.0f;
	static constexpr float InvalidCameraZoomAngle = -1.0f;
	static constexpr float MinCameraZoomLevel = 0.0f;
	static constexpr float MaxCameraZoomLevel = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = Sensitivity, meta = (AllowPrivateAccess = "true"))
	float CameraRotateSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = Sensitivity, meta = (AllowPrivateAccess = "true"))
	float CameraZoomSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MinCameraZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MaxCameraZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MinCameraZoomAngle;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float MaxCameraZoomAngle;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	float InitialCameraZoomLevel;

	UPROPERTY(EditDefaultsOnly, Category = Zoom, meta = (AllowPrivateAccess = "true"))
	bool bIsCameraChangeAngleEnabled;
	
	UPROPERTY(EditDefaultsOnly, Category = Smooth, meta = (AllowPrivateAccess = "true"))
	bool bIsCameraZoomSmoothEnabled;
	
	UPROPERTY(EditDefaultsOnly, Category = Smooth, meta = (AllowPrivateAccess = "true"))
	float CameraZoomSmoothSpeed;
	
	FVector2D CameraRotationBase;
	float TargetCameraZoomLevel;
	float TargetCameraZoomSpeed;
	EMouseMode MouseMiddleButtonMode;
};

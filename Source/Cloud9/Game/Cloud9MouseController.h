// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

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

	UFUNCTION()
	void OnCharacterMove();
	
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

	void ProcessCharacterView() const;
	void ProcessCameraRotation();
	void ProcessCameraZoom(float DeltaTime);
	
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
	bool IsMouseRotationMode;
};

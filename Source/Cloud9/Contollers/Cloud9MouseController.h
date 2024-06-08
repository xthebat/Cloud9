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
	static constexpr let InvalidCameraZoomLevel = -1.0f;
	static constexpr let InvalidCameraZoomAngle = -1.0f;
	static constexpr let MinCameraZoomLevel = 0.0f;
	static constexpr let MaxCameraZoomLevel = 1.0f;

	UCloud9MouseController();

	void OnCameraZoom(float Value);
	void OnCameraRotationPressed();
	void OnCameraRotationReleased();

	UFUNCTION()
	void OnCharacterMove();

	UMaterialInterface* GetCrosshairMaterial() const;

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

	void ProcessCharacterView();
	void ProcessCameraRotation();
	void ProcessCameraZoom(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category=Crosshair)
	UMaterialInterface* CrosshairMaterial;

	UPROPERTY(EditDefaultsOnly, Category=Sensitivity)
	float CameraRotateSensitivity;

	UPROPERTY(EditDefaultsOnly, Category=Sensitivity)
	float CameraZoomSensitivity;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float MinCameraZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float MaxCameraZoomHeight;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float MinCameraZoomAngle;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float MaxCameraZoomAngle;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float InitialCameraZoomLevel;

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	bool bIsCameraChangeAngleEnabled;

	UPROPERTY(EditDefaultsOnly, Category=Smooth)
	bool bIsCameraZoomSmoothEnabled;

	UPROPERTY(EditDefaultsOnly, Category=Smooth)
	float CameraZoomSmoothSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	FVector LastCrosshairLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	bool bIsLastCrosshairLocationValid;

	FVector2D CameraRotationBase;
	float TargetCameraZoomLevel;
	float TargetCameraZoomSpeed;
	bool IsMouseRotationMode;
};

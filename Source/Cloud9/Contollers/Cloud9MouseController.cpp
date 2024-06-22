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

#include "Cloud9MouseController.h"

#include "Cloud9/Tools/Cloud9ToolsLibrary.h"
#include "Cloud9/Tools/Math.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "GameFramework/PlayerInput.h"

UCloud9MouseController::UCloud9MouseController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	CameraRotateSensitivity = 1.0f;

	CameraZoomSensitivity = 0.1f;

	MinCameraZoomHeight = 600.0f;
	MaxCameraZoomHeight = 2000.0f;
	MinCameraZoomAngle = 40.0f;
	MaxCameraZoomAngle = 70.0f;
	InitialCameraZoomLevel = 0.5f;

	bIsCameraChangeAngleEnabled = true;

	bIsCameraZoomSmoothEnabled = true;
	CameraZoomSmoothSpeed = 0.5f;

	TargetCameraZoomLevel = InvalidCameraZoomLevel;
	TargetCameraZoomSpeed = 0.0f;

	CameraRotationBase = FVector2D::ZeroVector;
	IsMouseRotationMode = false;

	bIsLastCrosshairLocationValid = false;

	CrosshairMaterial = nullptr;

	MousePosition = FVector2D::ZeroVector;
	ViewportSizeX = 0.0f;
	ViewportSizeY = 0.0f;

	// Set highest tickrate to recalculate mouse position with max precision  
	SetComponentTickInterval(0.001);
}

FVector2D UCloud9MouseController::GetSensitivityMousePosition() const
{
	static let Settings = UCloud9DeveloperSettings::Get();

	if (Settings->IsWindowsInputEnabled)
	{
		return GetWindowsMousePosition();
	}

	return MousePosition;
}

FVector2D UCloud9MouseController::GetWindowsMousePosition() const
{
	FVector2D WindowsMousePosition = FVector2D::ZeroVector;
	GetCloud9Controller()->GetMousePosition(WindowsMousePosition.X, WindowsMousePosition.Y);
	return WindowsMousePosition;
}

float UCloud9MouseController::GetCameraZoomHeightLevel() const
{
	let Pawn = GetCloud9Pawn();
	OBJECT_RETURN_IF_FAIL(IsValid(Pawn), InvalidCameraZoomLevel, Fatal, "Can't get Cloud9Pawn");

	let ZoomHeightLevel = Math::InverseLerp(
		MinCameraZoomHeight,
		MaxCameraZoomHeight,
		Pawn->GetCameraZoomHeight());
	let ZoomAngleLevel = Math::InverseLerp(
		MinCameraZoomAngle,
		MaxCameraZoomAngle,
		Pawn->GetCameraRotationRoll());

	if (not FMath::IsNearlyEqual(ZoomHeightLevel, ZoomAngleLevel, 0.001f))
	{
		OBJECT_ERROR("ZoomHeightLevel = %f != ZoomAngleLevel != %f", ZoomHeightLevel, ZoomAngleLevel);
	}

	return ZoomHeightLevel;
}

void UCloud9MouseController::SetCameraZoomLevel(float Value) const
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Value = FMath::Clamp(Value, MinCameraZoomLevel, MaxCameraZoomLevel);

		let NewZoomHeight = FMath::Lerp(MinCameraZoomHeight, MaxCameraZoomHeight, Value);
		Pawn->SetCameraZoomHeight(NewZoomHeight);

		if (bIsCameraChangeAngleEnabled)
		{
			let NewZoomAngle = FMath::Lerp(MinCameraZoomAngle, MaxCameraZoomAngle, Value);
			Pawn->SetCameraRotationRoll(NewZoomAngle);
		}
	}
}

TOptional<FHitResult> UCloud9MouseController::GetHitUnderCursor(
	const TArray<AActor*>& ActorsToIgnore,
	ECollisionChannel TraceChannel,
	bool bTraceComplex) const
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = bTraceComplex;
	Params.AddIgnoredActors(ActorsToIgnore);
	let Controller = GetCloud9Controller();
	let Position = GetSensitivityMousePosition();
	if (Controller->GetHitResultAtScreenPosition(Position, TraceChannel, Params, HitResult))
	{
		return HitResult;
	}
	return {};
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UCloud9MouseController::OnCharacterMove() { ProcessCharacterView(); }

UMaterialInterface* UCloud9MouseController::GetCrosshairMaterial() const { return CrosshairMaterial; }

void UCloud9MouseController::ProcessMouseChangePosition()
{
	float DeltaX = 0.0f, DeltaY = 0.0f;
	GetCloud9Controller()->GetInputMouseDelta(DeltaX, DeltaY);
	MousePosition.X = FMath::Clamp(MousePosition.X + DeltaX, 0.0f, static_cast<float>(ViewportSizeX));
	MousePosition.Y = FMath::Clamp(MousePosition.Y + DeltaY, 0.0f, static_cast<float>(ViewportSizeY));
}

void UCloud9MouseController::ProcessViewportSizeChange()
{
	int32 NewViewportSizeX = 0;
	int32 NewViewportSizeY = 0;
	let Controller = GetCloud9Controller();
	Controller->GetViewportSize(NewViewportSizeX, NewViewportSizeY);
	if (NewViewportSizeX != ViewportSizeX or NewViewportSizeY != ViewportSizeY)
	{
		MousePosition = GetWindowsMousePosition();
		ViewportSizeX = NewViewportSizeX;
		ViewportSizeY = NewViewportSizeY;
	}
}

void UCloud9MouseController::ProcessCharacterView()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		if (let Controller = GetCloud9Controller(); IsValid(Controller))
		{
			static let Settings = UCloud9DeveloperSettings::Get();

			var ActorsToIgnore = TArray<AActor*>{};

			if (not Settings->IsSelfAimEnabled)
			{
				ActorsToIgnore.Add(Pawn);
			}

			let CursorHit = GetHitUnderCursor(ActorsToIgnore);

			if (CursorHit.IsSet())
			{
				LastCrosshairLocation = CursorHit->Location;
				bIsLastCrosshairLocationValid = true;
				Pawn->SetViewDirection(CursorHit);
			}
			else
			{
				bIsLastCrosshairLocationValid = true;
			}
		}
	}
}

void UCloud9MouseController::ProcessCameraRotation()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn) && IsMouseRotationMode)
	{
		let NewMousePosition = GetWindowsMousePosition();
		let Offset = (NewMousePosition - CameraRotationBase).X;
		CameraRotationBase = NewMousePosition;
		let Angle = Offset * CameraRotateSensitivity;
		Pawn->AddCameraRotationYaw(Angle);
	}
}

void UCloud9MouseController::ProcessCameraZoom(float DeltaTime)
{
	if (TargetCameraZoomLevel != InvalidCameraZoomLevel)
	{
		let CurrentCameraZoomLevel = GetCameraZoomHeightLevel();

		if (TargetCameraZoomLevel == CurrentCameraZoomLevel)
		{
			TargetCameraZoomLevel = InvalidCameraZoomLevel;
			return;
		}

		float NewCameraZoomLevel = TargetCameraZoomLevel;

		if (bIsCameraZoomSmoothEnabled)
		{
			NewCameraZoomLevel = CurrentCameraZoomLevel - DeltaTime * TargetCameraZoomSpeed;
			if (TargetCameraZoomLevel > CurrentCameraZoomLevel)
			{
				NewCameraZoomLevel = FMath::Min(TargetCameraZoomLevel, NewCameraZoomLevel);
			}
			else if (TargetCameraZoomLevel < CurrentCameraZoomLevel)
			{
				NewCameraZoomLevel = FMath::Max(TargetCameraZoomLevel, NewCameraZoomLevel);
			}
		}

		SetCameraZoomLevel(NewCameraZoomLevel);
		if (NewCameraZoomLevel == TargetCameraZoomLevel)
		{
			TargetCameraZoomLevel = InvalidCameraZoomLevel;
		}
	}
}

void UCloud9MouseController::BeginPlay()
{
	Super::BeginPlay();
	SetCameraZoomLevel(InitialCameraZoomLevel);
}

void UCloud9MouseController::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	static var Sensitivity = -1.0f;

	// TODO: Better to do by callback but looks like not dramatically affect performace
	static let Settings = UCloud9DeveloperSettings::Get();
	if (Sensitivity != Settings->Sensitivity)
	{
		// Y-axis inverted by default?
		GetCloud9Controller()->PlayerInput->SetMouseSensitivity(Settings->Sensitivity, -Settings->Sensitivity);
	}

	ProcessViewportSizeChange();

	if (let Controller = GetCloud9Controller(); Controller->GetInputMode() != EInputMode::UIOnly)
	{
		ProcessMouseChangePosition();
		ProcessCharacterView();
		ProcessCameraRotation();
		ProcessCameraZoom(DeltaTime);
	}
}

void UCloud9MouseController::OnCameraZoom(float Value)
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		if (FMath::Abs(Value) > 0.0f && TargetCameraZoomLevel == InvalidCameraZoomLevel)
		{
			let CurrentCameraZoomLevel = GetCameraZoomHeightLevel();

			TargetCameraZoomLevel = FMath::Clamp(
				CurrentCameraZoomLevel - Value * CameraZoomSensitivity,
				MinCameraZoomLevel,
				MaxCameraZoomLevel
			);

			TargetCameraZoomSpeed = Value * CameraZoomSmoothSpeed;
		}
	}
}

void UCloud9MouseController::OnCameraRotationPressed()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		CameraRotationBase = GetWindowsMousePosition();
		IsMouseRotationMode = true;
	}
}

void UCloud9MouseController::OnCameraRotationReleased()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		CameraRotationBase = FVector2D::ZeroVector;
		IsMouseRotationMode = false;
	}
}

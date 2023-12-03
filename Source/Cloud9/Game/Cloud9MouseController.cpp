﻿// Copyright (c) 2023 Alexei Gladkikh
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
#include "Cloud9PlayerController.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

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
}

FVector2D UCloud9MouseController::GetMousePosition() const
{
	if (let Owner = GetOwner<ACloud9PlayerController>(); IsValid(Owner))
	{
		FVector2D MousePosition = FVector2D::ZeroVector;
		GetOwner<ACloud9PlayerController>()->GetMousePosition(MousePosition.X, MousePosition.Y);
		return MousePosition;
	}

	log(Fatal, "Can't get Cloud9PlayerController");
	return FVector2D::ZeroVector;
}

float UCloud9MouseController::GetCameraZoomHeightLevel() const
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		let ZoomHeightLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomHeight,
			MaxCameraZoomHeight,
			Pawn->GetCameraZoomHeight());
		let ZoomAngleLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomAngle,
			MaxCameraZoomAngle,
			Pawn->GetCameraRotationRoll());

		if (not FMath::IsNearlyEqual(ZoomHeightLevel, ZoomAngleLevel, 0.001f))
		{
			log(Error, "ZoomHeightLevel = %f != ZoomAngleLevel != %f", ZoomHeightLevel, ZoomAngleLevel);
		}

		return ZoomHeightLevel;
	}

	log(Fatal, "Can't get Cloud9Pawn");
	return InvalidCameraZoomLevel;
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UCloud9MouseController::OnCharacterMove() { ProcessCharacterView(); }

void UCloud9MouseController::ProcessCharacterView() const
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		if (let Controller = GetCloud9Controller(); IsValid(Controller))
		{
			FHitResult TraceHitResult;
			let bIsHitValid = Controller->GetHitResultUnderCursor(
				ECC_Visibility,
				true,
				TraceHitResult);
			Pawn->SetViewDirection(TraceHitResult, bIsHitValid);
		}
	}
}

void UCloud9MouseController::ProcessCameraRotation()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn) && IsMouseRotationMode)
	{
		let NewMousePosition = GetMousePosition();
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

	ProcessCharacterView();
	ProcessCameraRotation();
	ProcessCameraZoom(DeltaTime);
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
		CameraRotationBase = GetMousePosition();
		IsMouseRotationMode = true;
		Pawn->SetCursorIsHidden(true);
	}
}

void UCloud9MouseController::OnCameraRotationReleased()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		CameraRotationBase = FVector2D::ZeroVector;
		IsMouseRotationMode = false;
		Pawn->SetCursorIsHidden(false);
	}
}

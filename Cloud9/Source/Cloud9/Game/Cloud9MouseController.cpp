#include "Cloud9MouseController.h"
#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

UCloud9MouseController::UCloud9MouseController()
{
	bCanEverTick = true;
	bStartWithTickEnabled = true;
	
	RotateSensitivity = 1.0f;

	ZoomSensitivity = 400.0f;

	MinZoomHeight = 400.0f;
	MaxZoomHeight = 1800.0f;

	bIsSmoothZoom = false;
	SmoothZoomSpeed = 1600.0f;

	MouseMode = EMouseMode::Aiming;
	MousePositionBase = FVector2D::ZeroVector;
	TargetZoomHeight = -1.0f;
}

FVector2D UCloud9MouseController::GetMousePosition() const
{
	FVector2D MousePosition = FVector2D::ZeroVector;
	GetOwner<ACloud9PlayerController>()->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

bool UCloud9MouseController::IsZoomInProcess() const { return TargetZoomHeight > 0.0f; }

float UCloud9MouseController::GetCameraZoomLevel() const
{
	const auto ZoomHeight = GetPawn()->GetCameraZoom();
	return UCloud9ToolsLibrary::InverseLerp(MinZoomHeight, MaxZoomHeight, ZoomHeight);
}

void UCloud9MouseController::SetCameraZoomLevel(float Value) const
{
	if (Value >= 0.0f && Value <= 1.0f)
	{
		const auto NewZoomHeight = FMath::Lerp(MinZoomHeight, MaxZoomHeight, Value);
		GetPawn()->SetCameraZoom(NewZoomHeight);
	}
}

ACloud9Character* UCloud9MouseController::GetPawn() const
{
	return GetOwner<ACloud9PlayerController>()->GetPawn<ACloud9Character>();
}

void UCloud9MouseController::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MouseMode == EMouseMode::Aiming)
	{
		FHitResult TraceHitResult;
		GetOwner<ACloud9PlayerController>()->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
		GetPawn()->SetViewDirection(TraceHitResult);
	}
	else if (MouseMode == EMouseMode::Rotation)
	{
		const auto NewMousePosition = GetMousePosition();
		const auto Offset = (NewMousePosition - MousePositionBase).X;
		MousePositionBase = NewMousePosition;
		const auto Angle = Offset * RotateSensitivity;
		GetPawn()->AddCameraRotation(Angle);
	}

	if (IsZoomInProcess())
	{
		const auto Sign = FMath::Sign(TargetZoomHeight - GetPawn()->GetCameraZoom());
		const auto NewCameraZoom = FMath::Clamp(
			GetPawn()->GetCameraZoom() + Sign * SmoothZoomSpeed * DeltaTime,
			Sign > 0 ? MinZoomHeight : TargetZoomHeight,
			Sign < 0 ? MaxZoomHeight : TargetZoomHeight
		);
		GetPawn()->SetCameraZoom(NewCameraZoom);
		if (TargetZoomHeight == NewCameraZoom)
			TargetZoomHeight = -1.0f;
	}
}

void UCloud9MouseController::CameraZoom(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
	{
		const auto NewCameraZoom = FMath::Clamp(
			GetPawn()->GetCameraZoom() - Value * ZoomSensitivity,
			MinZoomHeight,
			MaxZoomHeight
		);

		if (!bIsSmoothZoom)
		{
			GetPawn()->SetCameraZoom(NewCameraZoom);
		}
		else if (!IsZoomInProcess() && TargetZoomHeight != NewCameraZoom)
		{
			TargetZoomHeight = NewCameraZoom;
		}
	}
}

void UCloud9MouseController::CameraRotationPressed()
{
	MousePositionBase = GetMousePosition();
	MouseMode = EMouseMode::Rotation;
	GetPawn()->SetCursorIsHidden(true);
}

void UCloud9MouseController::CameraRotationReleased()
{
	MousePositionBase = FVector2D::ZeroVector;
	MouseMode = EMouseMode::Aiming;
	GetPawn()->SetCursorIsHidden(false);
}

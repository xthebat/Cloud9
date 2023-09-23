#include "Cloud9MouseController.h"
#include "Cloud9PlayerController.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

UCloud9MouseController::UCloud9MouseController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	CameraRotateSensitivity = 1.0f;

	CameraZoomSensitivity = 0.2f;

	MinCameraZoomHeight = 600.0f;
	MaxCameraZoomHeight = 2000.0f;
	InitialCameraZoomLevel = 0.5f;
	InitialCameraZoomAngle = 60.0f;

	bIsCameraZoomSmoothEnabled = true;
	CameraZoomSmoothSpeed = 0.5f;

	TargetCameraZoomLevel = InvalidCameraZoomLevel;
	TargetCameraZoomSpeed = 0.0f;

	CameraRotationBase = FVector2D::ZeroVector;
	MouseMiddleButtonMode = EMouseMode::Aiming;
}

FVector2D UCloud9MouseController::GetMousePosition() const
{
	FVector2D MousePosition = FVector2D::ZeroVector;
	GetOwner<ACloud9PlayerController>()->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

float UCloud9MouseController::GetCameraZoomLevel() const
{
	if (IsValid(GetPawn()))
	{
		const auto ZoomHeight = GetPawn()->GetCameraZoom();
		return UCloud9ToolsLibrary::InverseLerp(MinCameraZoomHeight, MaxCameraZoomHeight, ZoomHeight);
	}
	
	return InvalidCameraZoomLevel;
}

void UCloud9MouseController::SetCameraZoomLevel(float Value) const
{
	if (IsValid(GetPawn()))
	{
		Value = FMath::Clamp(Value, MinCameraZoomLevel, MaxCameraZoomLevel);
		const auto NewZoomHeight = FMath::Lerp(MinCameraZoomHeight, MaxCameraZoomHeight, Value);
		// GetPawn()->SetCameraRotationRoll();
		GetPawn()->SetCameraZoom(NewZoomHeight);
	}
}

void UCloud9MouseController::ProcessZoom(float DeltaTime)
{
	if (TargetCameraZoomLevel == InvalidCameraZoomLevel)
		return;

	const auto CurrentCameraZoomLevel = GetCameraZoomLevel();

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
			NewCameraZoomLevel = FMath::Min(TargetCameraZoomLevel, NewCameraZoomLevel);
		else if (TargetCameraZoomLevel < CurrentCameraZoomLevel)
			NewCameraZoomLevel = FMath::Max(TargetCameraZoomLevel, NewCameraZoomLevel);
	}

	SetCameraZoomLevel(NewCameraZoomLevel);
	if (NewCameraZoomLevel == TargetCameraZoomLevel)
		TargetCameraZoomLevel = InvalidCameraZoomLevel;
}

void UCloud9MouseController::BeginPlay()
{
	Super::BeginPlay();
	SetCameraZoomLevel(InitialCameraZoomLevel);
	GetPawn()->SetCameraRotationRoll(InitialCameraZoomAngle);
}

void UCloud9MouseController::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MouseMiddleButtonMode == EMouseMode::Aiming)
	{
		FHitResult TraceHitResult;
		GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
		GetPawn()->SetViewDirection(TraceHitResult);
	}
	else if (MouseMiddleButtonMode == EMouseMode::Rotation)
	{
		const auto NewMousePosition = GetMousePosition();
		const auto Offset = (NewMousePosition - CameraRotationBase).X;
		CameraRotationBase = NewMousePosition;
		const auto Angle = Offset * CameraRotateSensitivity;
		GetPawn()->AddCameraRotationYaw(Angle);
	}

	ProcessZoom(DeltaTime);
}

void UCloud9MouseController::OnCameraZoom(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f && TargetCameraZoomLevel == InvalidCameraZoomLevel)
	{
		const auto CurrentCameraZoomLevel = GetCameraZoomLevel();

		TargetCameraZoomLevel = FMath::Clamp(
			CurrentCameraZoomLevel - Value * CameraZoomSensitivity,
			MinCameraZoomLevel,
			MaxCameraZoomLevel
		);

		TargetCameraZoomSpeed = Value * CameraZoomSmoothSpeed;

		UE_LOG(LogCloud9, Display, TEXT("Set TargetCameraZoomLevel = %f"), TargetCameraZoomLevel);
	}
}

void UCloud9MouseController::OnCameraRotationPressed()
{
	if (IsValid(GetPawn()))
	{
		CameraRotationBase = GetMousePosition();
		MouseMiddleButtonMode = EMouseMode::Rotation;
		GetPawn()->SetCursorIsHidden(true);
	}
}

void UCloud9MouseController::OnCameraRotationReleased()
{
	if (IsValid(GetPawn()))
	{
		CameraRotationBase = FVector2D::ZeroVector;
		MouseMiddleButtonMode = EMouseMode::Aiming;
		GetPawn()->SetCursorIsHidden(false);
	}
}



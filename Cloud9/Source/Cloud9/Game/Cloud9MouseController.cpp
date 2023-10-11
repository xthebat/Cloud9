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
	FVector2D MousePosition = FVector2D::ZeroVector;
	GetOwner<ACloud9PlayerController>()->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

float UCloud9MouseController::GetCameraZoomHeightLevel() const
{
	if (IsValid(GetPawn()))
	{
		const auto ZoomHeightLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomHeight,
			MaxCameraZoomHeight,
			GetPawn()->GetCameraZoomHeight());
		const auto ZoomAngleLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomAngle,
			MaxCameraZoomAngle,
			GetPawn()->GetCameraRotationRoll());

		if (!FMath::IsNearlyEqual(ZoomHeightLevel, ZoomAngleLevel, 0.001f))
		{
			UE_LOG(LogCloud9, Error, TEXT("ZoomHeightLevel = %f != ZoomAngleLevel != %f"),
			       ZoomHeightLevel,
			       ZoomAngleLevel);
		}
		
		return ZoomHeightLevel;
	}

	return InvalidCameraZoomLevel;
}

void UCloud9MouseController::SetCameraZoomLevel(float Value) const
{
	if (IsValid(GetPawn()))
	{
		Value = FMath::Clamp(Value, MinCameraZoomLevel, MaxCameraZoomLevel);

		const auto NewZoomHeight = FMath::Lerp(MinCameraZoomHeight, MaxCameraZoomHeight, Value);
		GetPawn()->SetCameraZoomHeight(NewZoomHeight);

		if (bIsCameraChangeAngleEnabled)
		{
			const auto NewZoomAngle = FMath::Lerp(MinCameraZoomAngle, MaxCameraZoomAngle, Value);
			GetPawn()->SetCameraRotationRoll(NewZoomAngle);			
		}
	}
}

void UCloud9MouseController::ProcessZoom(float DeltaTime)
{
	if (TargetCameraZoomLevel == InvalidCameraZoomLevel)
		return;

	const auto CurrentCameraZoomLevel = GetCameraZoomHeightLevel();

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
}

void UCloud9MouseController::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult TraceHitResult;
	GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
	GetPawn()->SetViewDirection(TraceHitResult);

	if (IsMouseRotationMode)
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
		const auto CurrentCameraZoomLevel = GetCameraZoomHeightLevel();

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
		IsMouseRotationMode = true;
		GetPawn()->SetCursorIsHidden(true);
	}
}

void UCloud9MouseController::OnCameraRotationReleased()
{
	if (IsValid(GetPawn()))
	{
		CameraRotationBase = FVector2D::ZeroVector;
		IsMouseRotationMode = false;
		GetPawn()->SetCursorIsHidden(false);
	}
}

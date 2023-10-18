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
	if (IsValid(GetCloud9Pawn()))
	{
		const auto ZoomHeightLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomHeight,
			MaxCameraZoomHeight,
			GetCloud9Pawn()->GetCameraZoomHeight());
		const auto ZoomAngleLevel = UCloud9ToolsLibrary::InverseLerp(
			MinCameraZoomAngle,
			MaxCameraZoomAngle,
			GetCloud9Pawn()->GetCameraRotationRoll());

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
	if (IsValid(GetCloud9Pawn()))
	{
		Value = FMath::Clamp(Value, MinCameraZoomLevel, MaxCameraZoomLevel);

		const auto NewZoomHeight = FMath::Lerp(MinCameraZoomHeight, MaxCameraZoomHeight, Value);
		GetCloud9Pawn()->SetCameraZoomHeight(NewZoomHeight);

		if (bIsCameraChangeAngleEnabled)
		{
			const auto NewZoomAngle = FMath::Lerp(MinCameraZoomAngle, MaxCameraZoomAngle, Value);
			GetCloud9Pawn()->SetCameraRotationRoll(NewZoomAngle);
		}
	}
}

void UCloud9MouseController::OnCharacterMove() { ProcessCharacterView(); }

void UCloud9MouseController::ProcessCharacterView() const
{
	FHitResult TraceHitResult;
	GetCloud9Controller()->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	GetCloud9Pawn()->SetViewDirection(TraceHitResult);
}

void UCloud9MouseController::ProcessCameraRotation()
{
	if (IsMouseRotationMode)
	{
		const auto NewMousePosition = GetMousePosition();
		const auto Offset = (NewMousePosition - CameraRotationBase).X;
		CameraRotationBase = NewMousePosition;
		const auto Angle = Offset * CameraRotateSensitivity;
		GetCloud9Pawn()->AddCameraRotationYaw(Angle);
	}
}

void UCloud9MouseController::ProcessCameraZoom(float DeltaTime)
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

	ProcessCharacterView();
	ProcessCameraRotation();
	ProcessCameraZoom(DeltaTime);
}

void UCloud9MouseController::OnCameraZoom(float Value)
{
	if (IsValid(GetCloud9Pawn()) && FMath::Abs(Value) > 0.0f && TargetCameraZoomLevel == InvalidCameraZoomLevel)
	{
		const auto CurrentCameraZoomLevel = GetCameraZoomHeightLevel();

		TargetCameraZoomLevel = FMath::Clamp(
			CurrentCameraZoomLevel - Value * CameraZoomSensitivity,
			MinCameraZoomLevel,
			MaxCameraZoomLevel
		);

		TargetCameraZoomSpeed = Value * CameraZoomSmoothSpeed;
	}
}

void UCloud9MouseController::OnCameraRotationPressed()
{
	if (IsValid(GetCloud9Pawn()))
	{
		CameraRotationBase = GetMousePosition();
		IsMouseRotationMode = true;
		GetCloud9Pawn()->SetCursorIsHidden(true);
	}
}

void UCloud9MouseController::OnCameraRotationReleased()
{
	if (IsValid(GetCloud9Pawn()))
	{
		CameraRotationBase = FVector2D::ZeroVector;
		IsMouseRotationMode = false;
		GetCloud9Pawn()->SetCursorIsHidden(false);
	}
}

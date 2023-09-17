// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9PlayerController.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = false;
	MouseMode = EMouseMode::Aiming;
	MousePositionBase = FVector2D::ZeroVector;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	RotateSensitivity = 1.0f;
	ZoomSensitivity = 400.0f;
	MinZoomHeight = 400.0f;
	MaxZoomHeight = 1800.0f;
	bIsSmoothZoom = false;
	TargetZoomHeight = -1.0f;
	SmoothZoomSpeed = 1600.0f;
	Console = CreateDefaultSubobject<UCloud9ConsoleComponent>(TEXT("Console"));
}

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (MouseMode == EMouseMode::Aiming)
	{
		FHitResult TraceHitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
		GetMyPawn()->SetViewDirection(TraceHitResult);
	}
	else if (MouseMode == EMouseMode::Rotation)
	{
		const auto NewMousePosition = GetMousePosition();
		const auto Offset = (NewMousePosition - MousePositionBase).X;
		MousePositionBase = NewMousePosition;
		const auto Angle = Offset * RotateSensitivity;
		GetMyPawn()->AddCameraRotation(Angle);
	}

	if (IsZoomInProcess())
	{
		const auto Sign = FMath::Sign(TargetZoomHeight - GetMyPawn()->GetCameraZoom());
		const auto NewCameraZoom = FMath::Clamp(
			GetMyPawn()->GetCameraZoom() + Sign * SmoothZoomSpeed * DeltaTime,
			Sign > 0 ? MinZoomHeight : TargetZoomHeight,
			Sign < 0 ? MaxZoomHeight : TargetZoomHeight
		);
		GetMyPawn()->SetCameraZoom(NewCameraZoom);
		if (TargetZoomHeight == NewCameraZoom)
			TargetZoomHeight = -1.0f;
	}
}

void ACloud9PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ACloud9PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACloud9PlayerController::MoveRight);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ACloud9PlayerController::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, this, &ACloud9PlayerController::CrouchReleased);

	InputComponent->BindAction("Walk", IE_Pressed, this, &ACloud9PlayerController::WalkPressed);
	InputComponent->BindAction("Walk", IE_Released, this, &ACloud9PlayerController::WalkReleased);

	InputComponent->BindAxis("CameraZoom", this, &ACloud9PlayerController::CameraZoom);
	InputComponent->BindAction("CameraRotation", IE_Pressed, this, &ACloud9PlayerController::CameraRotationPressed);
	InputComponent->BindAction("CameraRotation", IE_Released, this, &ACloud9PlayerController::CameraRotationReleased);
}

void ACloud9PlayerController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

bool ACloud9PlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && Console != nullptr)
		bHandled |= Console->ProcessConsoleExec(Cmd, Ar, Executor);

	return bHandled;
}

ACloud9Character* ACloud9PlayerController::GetMyPawn() const { return GetPawn<ACloud9Character>(); }

FVector2D ACloud9PlayerController::GetMousePosition() const
{
	FVector2D MousePosition = FVector2D::ZeroVector;
	Super::GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

bool ACloud9PlayerController::IsZoomInProcess() const { return TargetZoomHeight > 0.0f; }

void ACloud9PlayerController::MoveForward(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetMyPawn()->AddMovementInput(FVector::XAxisVector, Value);
}

void ACloud9PlayerController::MoveRight(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetMyPawn()->AddMovementInput(FVector::YAxisVector, Value);
}

void ACloud9PlayerController::WalkPressed() { GetMyPawn()->Sneak(); }

void ACloud9PlayerController::WalkReleased() { GetMyPawn()->UnSneak(); }

void ACloud9PlayerController::CrouchPressed() { GetMyPawn()->Crouch(false); }

void ACloud9PlayerController::CrouchReleased() { GetMyPawn()->UnCrouch(false); }

void ACloud9PlayerController::CameraZoom(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
	{
		const auto NewCameraZoom = FMath::Clamp(
			GetMyPawn()->GetCameraZoom() - Value * ZoomSensitivity,
			MinZoomHeight,
			MaxZoomHeight
		);
	
		if (!bIsSmoothZoom)
			GetMyPawn()->SetCameraZoom(NewCameraZoom);
		else if (!IsZoomInProcess() && TargetZoomHeight != NewCameraZoom)
		{
			TargetZoomHeight = NewCameraZoom;
		}
	}
}

void ACloud9PlayerController::CameraRotationPressed()
{
	MousePositionBase = GetMousePosition();
	MouseMode = EMouseMode::Rotation;
	GetMyPawn()->SetCursorIsHidden(true);
}

void ACloud9PlayerController::CameraRotationReleased()
{
	MousePositionBase = FVector2D::ZeroVector;
	MouseMode = EMouseMode::Aiming;
	GetMyPawn()->SetCursorIsHidden(false);
}

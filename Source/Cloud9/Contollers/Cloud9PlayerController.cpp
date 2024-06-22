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

#include "Cloud9PlayerController.h"

#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Contollers/Cloud9KeyboardController.h"
#include "Cloud9/Contollers/Cloud9MouseController.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	ConsoleComponent = CreateDefaultSubobject<UCloud9ConsoleComponent>(ConsoleName);
	KeyboardController = CreateDefaultSubobject<UCloud9KeyboardController>(KeyboardControllerName);
	MouseController = CreateDefaultSubobject<UCloud9MouseController>(MouseControllerName);

	IsPawnControlEnabled = true;
}

UCloud9MouseController* ACloud9PlayerController::GetMouseControllerComponent() const { return MouseController; }

UCloud9KeyboardController* ACloud9PlayerController::GetKeyboardControllerComponent() const
{
	return KeyboardController;
}

void ACloud9PlayerController::ShowMouseCursor(bool NewValue)
{
	bShowMouseCursor = NewValue;
}

EInputMode ACloud9PlayerController::GetInputMode() const
{
	let MyWorld = GetWorld();
	OBJECT_RETURN_IF_FAIL(IsValid(MyWorld), EInputMode::Unknown, Verbose, "World is invalid");

	let GameViewport = MyWorld->GetGameViewport();
	OBJECT_RETURN_IF_FAIL(IsValid(GameViewport), EInputMode::Unknown, Verbose, "GameViewport is invalid");

	let IgnoreInput = GameViewport->IgnoreInput();
	let MouseCaptureMode = GameViewport->GetMouseCaptureMode();

	if (not IgnoreInput and MouseCaptureMode == EMouseCaptureMode::CaptureDuringMouseDown)
	{
		return EInputMode::UIAndGame;
	}

	if (IgnoreInput && MouseCaptureMode == EMouseCaptureMode::NoCapture)
	{
		return EInputMode::UIOnly;
	}

	return EInputMode::GameOnly;
}

void ACloud9PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(
		*UCloud9DeveloperSettings::BindMoveForwardName,
		KeyboardController, &UCloud9KeyboardController::OnMoveForward);

	InputComponent->BindAxis(
		*UCloud9DeveloperSettings::BindMoveBackwardName,
		KeyboardController, &UCloud9KeyboardController::OnMoveBackward);

	InputComponent->BindAxis(
		*UCloud9DeveloperSettings::BindMoveLeftName,
		KeyboardController, &UCloud9KeyboardController::OnMoveLeft);

	InputComponent->BindAxis(
		*UCloud9DeveloperSettings::BindMoveRightName,
		KeyboardController, &UCloud9KeyboardController::OnMoveRight);

	InputComponent->BindAxis("CameraZoom", MouseController, &UCloud9MouseController::OnCameraZoom);
	InputComponent->BindAction(
		"CameraRotation",
		IE_Pressed,
		MouseController,
		&UCloud9MouseController::OnCameraRotationPressed);

	InputComponent->BindAction(
		"CameraRotation",
		IE_Released,
		MouseController,
		&UCloud9MouseController::OnCameraRotationReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindCrouchName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnCrouchPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindCrouchName,
		IE_Released, KeyboardController,
		&UCloud9KeyboardController::OnCrouchReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindWalkName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnWalkPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindWalkName,
		IE_Released, KeyboardController,
		&UCloud9KeyboardController::OnWalkReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindJumpName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnJumpPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindJumpName,
		IE_Released, KeyboardController,
		&UCloud9KeyboardController::OnJumpReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSlot1Name,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnSlot1);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSlot2Name,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnSlot2);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSlot3Name,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnSlot3);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSlot4Name,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnSlot4);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSlot5Name,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnSlot5);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindReloadName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnReloadPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindReloadName,
		IE_Released, KeyboardController,
		&UCloud9KeyboardController::OnReloadReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindPrimaryName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnPrimaryActionPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindPrimaryName,
		IE_Released, KeyboardController,
		&UCloud9KeyboardController::OnPrimaryActionReleased);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSecondaryName,
		IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSecondaryActionPressed);
	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindSecondaryName,
		IE_Released, KeyboardController, &UCloud9KeyboardController::OnSecondaryActionReleased);

	InputComponent->BindAction(
		*UCloud9DeveloperSettings::BindUseName,
		IE_Pressed, KeyboardController,
		&UCloud9KeyboardController::OnUseAction);

	InputComponent->BindAction(
		"CursorSelfAim", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnCursorSelfAim);

	KeyboardController->OnMoveDelegate.AddDynamic(MouseController, &UCloud9MouseController::OnCharacterMove);
}

bool ACloud9PlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && ConsoleComponent != nullptr)
	{
		bHandled |= ConsoleComponent->ProcessConsoleExec(Cmd, Ar, Executor);
	}

	return bHandled;
}

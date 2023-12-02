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

#include "Cloud9DeveloperSettings.h"
#include "Cloud9KeyboardController.h"
#include "Cloud9MouseController.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

const FName ACloud9PlayerController::ConsoleName = TEXT("Console");
const FName ACloud9PlayerController::KeyboardControllerName = TEXT("KeyboardController");
const FName ACloud9PlayerController::MouseControllerName = TEXT("MouseController");

ACloud9PlayerController::ACloud9PlayerController()
{
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	ConsoleComponent = CreateDefaultSubobject<UCloud9ConsoleComponent>(ConsoleName);
	KeyboardController = CreateDefaultSubobject<UCloud9KeyboardController>(KeyboardControllerName);
	MouseController = CreateDefaultSubobject<UCloud9MouseController>(MouseControllerName);
}

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	let Settings = UCloud9DeveloperSettings::Get();
	bShowMouseCursor = Settings->bIsShowMouseCursor;
}

void ACloud9PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", KeyboardController, &UCloud9KeyboardController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", KeyboardController, &UCloud9KeyboardController::OnMoveRight);

	InputComponent->BindAction("Crouch", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnCrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, KeyboardController, &UCloud9KeyboardController::OnCrouchReleased);

	InputComponent->BindAction("Walk", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnWalkPressed);
	InputComponent->BindAction("Walk", IE_Released, KeyboardController, &UCloud9KeyboardController::OnWalkReleased);

	InputComponent->BindAction("Jump", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnJumpPressed);
	InputComponent->BindAction("Jump", IE_Released, KeyboardController, &UCloud9KeyboardController::OnJumpReleased);

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

	InputComponent->BindAction("Slot1", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot1);
	InputComponent->BindAction("Slot2", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot2);
	InputComponent->BindAction("Slot3", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot3);
	InputComponent->BindAction("Slot4", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot4);
	InputComponent->BindAction("Slot5", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot5);

	InputComponent->BindAction("Reload", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnReload);

	InputComponent->BindAction(
		"Primary", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnPrimaryActionPressed);
	InputComponent->BindAction(
		"Primary", IE_Released, KeyboardController, &UCloud9KeyboardController::OnPrimaryActionReleased);
	InputComponent->BindAction(
		"Secondary", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSecondaryActionPressed);
	InputComponent->BindAction(
		"Secondary", IE_Released, KeyboardController, &UCloud9KeyboardController::OnSecondaryActionReleased);

	InputComponent->BindAction(
		"Use", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnUseAction);

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

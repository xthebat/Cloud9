#include "Cloud9PlayerController.h"

#include "Cloud9KeyboardController.h"
#include "Cloud9MouseController.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

const FName ACloud9PlayerController::ConsoleName = TEXT("Console");
const FName ACloud9PlayerController::KeyboardControllerName = TEXT("KeyboardController");
const FName ACloud9PlayerController::MouseControllerName = TEXT("MouseController");

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	ConsoleComponent = CreateDefaultSubobject<UCloud9ConsoleComponent>(ConsoleName);
	KeyboardController = CreateDefaultSubobject<UCloud9KeyboardController>(KeyboardControllerName);
	MouseController = CreateDefaultSubobject<UCloud9MouseController>(MouseControllerName);
}

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
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
	InputComponent->BindAction("CameraRotation", IE_Pressed, MouseController, &UCloud9MouseController::OnCameraRotationPressed);
	InputComponent->BindAction("CameraRotation", IE_Released, MouseController, &UCloud9MouseController::OnCameraRotationReleased);

	InputComponent->BindAction("Slot1", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot1);
	InputComponent->BindAction("Slot2", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot2);
	InputComponent->BindAction("Slot3", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot3);
	InputComponent->BindAction("Slot4", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot4);
	InputComponent->BindAction("Slot5", IE_Pressed, KeyboardController, &UCloud9KeyboardController::OnSlot5);
	
	InputComponent->BindAction("Reload", IE_Pressed, KeyboardController, &UCloud9KeyboardController::Reload);

	KeyboardController->OnMoveDelegate.AddDynamic(MouseController, &UCloud9MouseController::OnCharacterMove);
}

bool ACloud9PlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && ConsoleComponent != nullptr)
		bHandled |= ConsoleComponent->ProcessConsoleExec(Cmd, Ar, Executor);

	return bHandled;
}

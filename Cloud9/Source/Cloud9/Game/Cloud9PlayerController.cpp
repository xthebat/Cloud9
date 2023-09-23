#include "Cloud9PlayerController.h"

#include "Cloud9KeyboardController.h"
#include "Cloud9MouseController.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	Console = CreateDefaultSubobject<UCloud9ConsoleComponent>(TEXT("Console"));
	KeyboardController = CreateDefaultSubobject<UCloud9KeyboardController>(TEXT("KeyboardController"));
	MouseController = CreateDefaultSubobject<UCloud9MouseController>(TEXT("MouseController"));
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
}

bool ACloud9PlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && Console != nullptr)
		bHandled |= Console->ProcessConsoleExec(Cmd, Ar, Executor);

	return bHandled;
}

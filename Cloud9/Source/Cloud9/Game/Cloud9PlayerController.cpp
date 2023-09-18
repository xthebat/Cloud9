#include "Cloud9PlayerController.h"

#include "Cloud9KeyboardController.h"
#include "Cloud9MouseController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	Console = CreateDefaultSubobject<UCloud9ConsoleComponent>(TEXT("Console"));
	MouseController = CreateDefaultSubobject<UCloud9MouseController>(TEXT("MouseController"));
}

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ACloud9PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", KeyboardController, &UCloud9KeyboardController::MoveForward);
	InputComponent->BindAxis("MoveRight", KeyboardController, &UCloud9KeyboardController::MoveRight);

	InputComponent->BindAction("Crouch", IE_Pressed, KeyboardController, &UCloud9KeyboardController::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, KeyboardController, &UCloud9KeyboardController::CrouchReleased);

	InputComponent->BindAction("Walk", IE_Pressed, KeyboardController, &UCloud9KeyboardController::WalkPressed);
	InputComponent->BindAction("Walk", IE_Released, KeyboardController, &UCloud9KeyboardController::WalkReleased);

	InputComponent->BindAxis("CameraZoom", MouseController, &UCloud9MouseController::CameraZoom);
	InputComponent->BindAction("CameraRotation", IE_Pressed, MouseController, &UCloud9MouseController::CameraRotationPressed);
	InputComponent->BindAction("CameraRotation", IE_Released, MouseController, &UCloud9MouseController::CameraRotationReleased);
}

bool ACloud9PlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && Console != nullptr)
		bHandled |= Console->ProcessConsoleExec(Cmd, Ar, Executor);

	return bHandled;
}

ACloud9Character* ACloud9PlayerController::GetMyPawn() const { return GetPawn<ACloud9Character>(); }



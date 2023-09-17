// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Console/Cloud9ConsoleComponent.h"
#include "Engine/World.h"

ACloud9PlayerController::ACloud9PlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	Console = CreateDefaultSubobject<UCloud9ConsoleComponent>(TEXT("Console"));
	// ConsoleClass = UCloud9ConsoleComponent::StaticClass();
	// Console = NewObject<UCloud9ConsoleComponent>(this, *ConsoleClass, TEXT("Console"));
	// AddOwnedComponent(Console);
}

// UCloud9ConsoleComponent* ACloud9PlayerController::GetConsole() const { return Console; }

void ACloud9PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	FHitResult TraceHitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
	GetPawn<ACloud9Character>()->SetViewDirection(TraceHitResult);
}

void ACloud9PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ACloud9PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACloud9PlayerController::MoveRight);

	InputComponent->BindAction("Walk", IE_Pressed, this, &ACloud9PlayerController::WalkPressed);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ACloud9PlayerController::CrouchPressed);

	InputComponent->BindAction("Walk", IE_Released, this, &ACloud9PlayerController::WalkReleased);
	InputComponent->BindAction("Crouch", IE_Released, this, &ACloud9PlayerController::CrouchReleased);
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

void ACloud9PlayerController::MoveForward(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
	{
		const auto MyPawn = GetPawn<ACloud9Character>();
		const auto Velocity = MyPawn->GetVelocity().Size();
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Velocity = %f"), Velocity));
		MyPawn->AddMovementInput(FVector::XAxisVector, Value);
	}
}

void ACloud9PlayerController::MoveRight(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
	{
		const auto MyPawn = GetPawn<ACloud9Character>();
		const auto Velocity = MyPawn->GetVelocity().Size();
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Velocity = %f"), Velocity));
		MyPawn->AddMovementInput(FVector::YAxisVector, Value);
	}
}

void ACloud9PlayerController::WalkPressed() { GetPawn<ACloud9Character>()->Sneak(); }

void ACloud9PlayerController::CrouchPressed() { GetPawn<ACloud9Character>()->Crouch(false); }

void ACloud9PlayerController::WalkReleased() { GetPawn<ACloud9Character>()->UnSneak(); }

void ACloud9PlayerController::CrouchReleased() { GetPawn<ACloud9Character>()->UnCrouch(false); }

// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9KeyboardController.h"

UCloud9KeyboardController::UCloud9KeyboardController()
{
	
}

void UCloud9KeyboardController::MoveForward(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn()->AddMovementInput(FVector::XAxisVector, Value);
}

void UCloud9KeyboardController::MoveRight(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn()->AddMovementInput(FVector::YAxisVector, Value);
}

void UCloud9KeyboardController::WalkPressed() { GetPawn()->Sneak(); }

void UCloud9KeyboardController::WalkReleased() { GetPawn()->UnSneak(); }

void UCloud9KeyboardController::CrouchPressed() { GetPawn()->Crouch(false); }

void UCloud9KeyboardController::CrouchReleased() { GetPawn()->UnCrouch(false); }
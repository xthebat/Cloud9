// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9KeyboardController.h"

void UCloud9KeyboardController::OnMoveForward(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn()->AddMovementInput(FVector::XAxisVector, Value);
}

void UCloud9KeyboardController::OnMoveRight(float Value)
{
	if (IsValid(GetPawn()) && FMath::Abs(Value) > 0.0f)
		GetPawn()->AddMovementInput(FVector::YAxisVector, Value);
}

void UCloud9KeyboardController::OnWalkPressed()
{
	if (IsValid(GetPawn()))
		GetPawn()->Sneak();
}

void UCloud9KeyboardController::OnWalkReleased()
{
	if (IsValid(GetPawn()))
		GetPawn()->UnSneak();
}

void UCloud9KeyboardController::OnCrouchPressed()
{
	if (IsValid(GetPawn()))
		GetPawn()->Crouch(false);
}

void UCloud9KeyboardController::OnCrouchReleased()
{
	if (IsValid(GetPawn()))
		GetPawn()->UnCrouch(false);
}

void UCloud9KeyboardController::OnJumpPressed()
{
	if (IsValid(GetPawn()))
		GetPawn()->Jump();
}

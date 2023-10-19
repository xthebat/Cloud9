// ReSharper disable CppMemberFunctionMayBeConst
#include "Cloud9KeyboardController.h"
#include "Cloud9/Character/Enums/Cloud9WeaponSlot.h"
#include "GameFramework/SpringArmComponent.h"

UCloud9KeyboardController::UCloud9KeyboardController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	ForwardScale = 0.0f;
	RightScale = 0.0f;
}

void UCloud9KeyboardController::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto Pawn = GetCloud9Pawn();

	if (IsValid(Pawn) && (FMath::Abs(ForwardScale) > 0.0f ||FMath::Abs(RightScale) > 0.0f))
	{
		const auto FV = ForwardScale * Pawn->GetCameraBoom()->GetForwardVector();
		const auto RV = RightScale * Pawn->GetCameraBoom()->GetRightVector();
		
		auto DirectionXY = FVector{FV.X + RV.X, FV.Y + RV.Y, 0.0f};
		DirectionXY.Normalize();
		Pawn->AddMovementInput(DirectionXY);
		OnMoveDelegate.Broadcast();
	}

	// UpdateMove([](auto Pawn) { return Pawn->GetCameraBoom()->GetForwardVector(); }, ForwardScale);
	// UpdateMove([](auto Pawn) { return Pawn->GetCameraBoom()->GetRightVector(); }, RightScale);
}

template <typename TGetDirection>
void UCloud9KeyboardController::UpdateMove(TGetDirection GetDirection, float Scale)
{
	const auto Pawn = GetCloud9Pawn();

	if (IsValid(Pawn) && FMath::Abs(Scale) > 0.0f)
	{
		auto Direction3D = GetDirection(Pawn);
		auto DirectionXY = FVector{Direction3D.X, Direction3D.Y, 0.0f};
		DirectionXY.Normalize();
		Pawn->AddMovementInput(DirectionXY, Scale);
		OnMoveDelegate.Broadcast();
	}
}

void UCloud9KeyboardController::OnMoveForward(float Value) { ForwardScale = Value; }

void UCloud9KeyboardController::OnMoveRight(float Value) { RightScale = Value; }

void UCloud9KeyboardController::OnWalkPressed()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->Sneak();
}

void UCloud9KeyboardController::OnWalkReleased()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->UnSneak();
}

void UCloud9KeyboardController::OnCrouchPressed()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->Crouch(false);
}

void UCloud9KeyboardController::OnCrouchReleased()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->UnCrouch(false);
}

void UCloud9KeyboardController::OnJumpPressed()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->Jump();
}

void UCloud9KeyboardController::OnJumpReleased()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->StopJumping();
}

void UCloud9KeyboardController::OnSlot1()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->GetInventory()->SelectWeapon(EWeaponSlot::Main);
}

void UCloud9KeyboardController::OnSlot2()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->GetInventory()->SelectWeapon(EWeaponSlot::Pistol);
}

void UCloud9KeyboardController::OnSlot3()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->GetInventory()->SelectWeapon(EWeaponSlot::Knife);
}

void UCloud9KeyboardController::OnSlot4()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->GetInventory()->SelectWeapon(EWeaponSlot::Grenade);
}

void UCloud9KeyboardController::OnSlot5()
{
	if (IsValid(GetCloud9Pawn()))
		GetCloud9Pawn()->GetInventory()->SelectWeapon(EWeaponSlot::Stuff);
}

void UCloud9KeyboardController::Reload()
{
	if (IsValid(GetCloud9Pawn()))
	{
		// TODO: Reload	
	}
}

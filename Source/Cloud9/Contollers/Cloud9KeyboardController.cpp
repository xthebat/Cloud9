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

// ReSharper disable CppMemberFunctionMayBeConst

#include "Cloud9KeyboardController.h"

#include "GameFramework/SpringArmComponent.h"

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

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

	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		if (FMath::Abs(ForwardScale) > 0.0f or FMath::Abs(RightScale) > 0.0f)
		{
			let FV = ForwardScale * Pawn->GetCameraBoom()->GetForwardVector();
			let RV = RightScale * Pawn->GetCameraBoom()->GetRightVector();

			var DirectionXY = FVector{FV.X + RV.X, FV.Y + RV.Y, 0.0f};
			DirectionXY.Normalize();
			Pawn->AddMovementInput(DirectionXY);
			OnMoveDelegate.Broadcast();
		}
	}
}

template <typename FunctionType>
void UCloud9KeyboardController::WeaponAction(FunctionType Function)
{
	let Pawn = GetCloud9Pawn();

	if (not IsValid(Pawn))
	{
		log(Error, "Pawn is invalid")
		return;
	}

	let Inventory = Pawn->GetInventory();

	if (not IsValid(Inventory))
	{
		log(Error, "Pawn inventory is invalid")
		return;
	}

	if (not Inventory->IsWeaponChanging())
	{
		let SelectedWeapon = Inventory->GetSelectedWeapon();

		if (not IsValid(SelectedWeapon))
		{
			log(Error, "Selected weapon is invalid");
			return;
		}

		// Checking of other actions in progress should be done inside weapon
		// because this can depend on implementation of weapon work logic
		Function(SelectedWeapon);
	}
}

void UCloud9KeyboardController::OnPrimaryActionPressed() { WeaponAction([](let It) { It->PrimaryAction(false); }); }

void UCloud9KeyboardController::OnPrimaryActionReleased() { WeaponAction([](let It) { It->PrimaryAction(true); }); }

void UCloud9KeyboardController::OnSecondaryActionPressed() { WeaponAction([](let It) { It->SecondaryAction(false); }); }

void UCloud9KeyboardController::OnSecondaryActionReleased() { WeaponAction([](let It) { It->SecondaryAction(true); }); }

void UCloud9KeyboardController::OnReloadPressed() { WeaponAction([](let It) { It->Reload(false); }); }

void UCloud9KeyboardController::OnReloadReleased() { WeaponAction([](let It) { It->Reload(true); }); }

void UCloud9KeyboardController::OnUseAction()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		// TODO: Get overhauled object by mouse
		Pawn->UseObject();
	}
}

template <typename GetDirectionType>
void UCloud9KeyboardController::UpdateMove(GetDirectionType GetDirection, float Scale)
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn) && FMath::Abs(Scale) > 0.0f)
	{
		var Direction3D = GetDirection(Pawn);
		var DirectionXY = FVector{Direction3D.X, Direction3D.Y, 0.0f};
		DirectionXY.Normalize();
		Pawn->AddMovementInput(DirectionXY, Scale);
		OnMoveDelegate.Broadcast();
	}
}

void UCloud9KeyboardController::OnMoveForward(float Value) { ForwardScale = Value; }

void UCloud9KeyboardController::OnMoveRight(float Value) { RightScale = Value; }

void UCloud9KeyboardController::OnWalkPressed()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->Sneak();
	}
}

void UCloud9KeyboardController::OnWalkReleased()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->UnSneak();
	}
}

void UCloud9KeyboardController::OnCrouchPressed()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->Crouch(false);
	}
}

void UCloud9KeyboardController::OnCrouchReleased()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->UnCrouch(false);
	}
}

void UCloud9KeyboardController::OnJumpPressed()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->Jump();
	}
}

void UCloud9KeyboardController::OnJumpReleased()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->StopJumping();
	}
}

void UCloud9KeyboardController::OnSlot1()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->GetInventory()->SelectWeapon(EWeaponSlot::Main);
	}
}

void UCloud9KeyboardController::OnSlot2()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->GetInventory()->SelectWeapon(EWeaponSlot::Pistol);
	}
}

void UCloud9KeyboardController::OnSlot3()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->GetInventory()->SelectWeapon(EWeaponSlot::Knife);
	}
}

void UCloud9KeyboardController::OnSlot4()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->GetInventory()->SelectWeapon(EWeaponSlot::Grenade);
	}
}

void UCloud9KeyboardController::OnSlot5()
{
	if (let Pawn = GetCloud9Pawn(); IsValid(Pawn))
	{
		Pawn->GetInventory()->SelectWeapon(EWeaponSlot::Stuff);
	}
}

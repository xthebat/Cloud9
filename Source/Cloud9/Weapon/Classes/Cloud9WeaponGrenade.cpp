﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9WeaponGrenade.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Weapon/Tables/WeaponTableGrenade.h"
#include "Engine/StaticMeshActor.h"

FWeaponId ACloud9WeaponGrenade::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

const FGrenadeWeaponInfo* ACloud9WeaponGrenade::GetWeaponInfo() const
{
	return WeaponDefinition.GetWeaponInfo<FGrenadeWeaponInfo>();
}

bool ACloud9WeaponGrenade::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	if (Super::OnInitialize(NewWeaponId, NewWeaponSkin))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FGrenadeWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(NewWeaponSkin);

		if (MySkinInfo.Material == nullptr)
		{
			log(Error, "[Weapon='%s'] Skin material is invalid", *GetName());
			return false;
		}

		return InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo.Material);
	}

	return false;
}

void ACloud9WeaponGrenade::OnWeaponAddedToInventory()
{
	Super::OnWeaponAddedToInventory();
	ChangeMeshCollisionState(WeaponMesh, false);
}

void ACloud9WeaponGrenade::OnWeaponRemovedFromInventory()
{
	Super::OnWeaponRemovedFromInventory();
	ChangeMeshCollisionState(WeaponMesh, true);
}

void ACloud9WeaponGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();
	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	let Character = GetOwner<ACloud9Character>();
	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FGrenadeWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&] { return PlayAnimMontage(PoseMontages->DeployMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::Deploy); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Primary))
	{
		// log(Error, "PoseMontages->PinpullPrimaryActionMontage");

		ExecuteAction(
			EWeaponAction::Primary,
			WeaponInfo->PinpullTime,
			[&] { return PlayAnimMontage(PoseMontages->PinpullPrimaryActionMontage); },
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::Primary);
				WeaponState.ActivateAction(EWeaponAction::PrimaryLoop);
			}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryLoop))
	{
		// log(Error, "PoseMontages->PinpullPrimaryActionMontage");

		// Hold last frame of montage
		PlayAnimMontage(
			PoseMontages->PinpullPrimaryActionMontage,
			1.0f,
			PoseMontages->PinpullPrimaryActionHoldTiming);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryEnd))
	{
		// log(Error, "PoseMontages->PrimaryActionMontage");

		ExecuteAction(
			EWeaponAction::PrimaryEnd,
			WeaponInfo->ThrowTime,
			[&] { return PlayAnimMontage(PoseMontages->PrimaryActionMontage); },
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::PrimaryEnd);
				Throw();
			}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		// TODO: Implement near throw (or cancel throw) for grenade
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Third))
	{
		// TODO: Implement middle throw (or cancel throw) for grenade
	}
}

void ACloud9WeaponGrenade::PrimaryAction(bool bIsReleased)
{
	if (not bIsReleased)
	{
		WeaponState.ActivateAction(EWeaponAction::Primary);
	}
	else
	{
		WeaponState.ActivateAction(EWeaponAction::PrimaryEnd);
	}

	WeaponState.ActivateAction(EWeaponAction::PrimaryLoop, bIsReleased);
}

bool ACloud9WeaponGrenade::Throw()
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Character is invalid", *GetName());
		return false;
	}

	let Controller = Character->GetCloud9Controller();

	if (not IsValid(Controller))
	{
		log(Error, "[Weapon='%s'] Controller is invalid", *GetName());
		return false;
	}

	let Inventory = Character->GetInventory();

	if (not IsValid(Inventory))
	{
		log(Error, "[Weapon='%s'] Inventory is invalid", *GetName());
		return false;
	}

	FHitResult CursorHit;
	if (not Controller->GetHitResultUnderCursor(ECC_Visibility, true, CursorHit))
	{
		log(Error, "Cursor not hit anything")
		return true;
	}

	let StartLocation = WeaponMesh->GetComponentLocation();
	let EndLocation = CursorHit.Location;
	let ViewDirection = (EndLocation - StartLocation) | EFVector::Normalize{};
	let RotationAxis = Character->GetActorRightVector();

	let Direction = ViewDirection.RotateAngleAxis(-45.0f, RotationAxis);

	Inventory->DropWeapon(GetWeaponSlot());

	SetActorLocation(StartLocation);

	// TODO: Implement grenade throw impulse
	let MyMesh = GetWeaponMesh();
	MyMesh->AddImpulse(800.0f * Direction, NAME_None, true);

	return true;
}

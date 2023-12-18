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

#include "Cloud9Inventory.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponMelee.h"


UCloud9Inventory::UCloud9Inventory()
{
	DefaultKnifeName = EMelee::Knife;
	DefaultPistolName = EFirearm::Deagle;

	SelectedWeaponSlot = EWeaponSlot::NotSelected;
	PendingWeaponSlot = EWeaponSlot::NotSelected;

	let SlotsNumber = StaticEnum<EWeaponSlot>()->NumEnums();
	WeaponSlots.SetNum(SlotsNumber);
}

void UCloud9Inventory::BeginPlay()
{
	Super::BeginPlay();

	if (let MyOwner = GetOwner<ACloud9Character>(); IsValid(MyOwner))
	{
		let DefaultKnife = GetWorld() | EUWorld::SpawnActor<ACloud9WeaponMelee>(
			[this](let It) { return It->OnSpawn(DefaultKnifeName, FWeaponSkin::Lore); });

		let DefaultPistol = GetWorld() | EUWorld::SpawnActor<ACloud9WeaponFirearm>(
			[this](let It) { return It->OnSpawn(DefaultPistolName, FWeaponSkin::OceanDrive); });

		let DefaultMain = GetWorld() | EUWorld::SpawnActor<ACloud9WeaponFirearm>(
			[this](let It) { return It->OnSpawn(EFirearm::Ak47); });

		if (not ShoveWeapon(EWeaponSlot::Knife, DefaultKnife))
		{
			log(Error, "Can't shove default knife into inventory");
			return;
		}

		if (not ShoveWeapon(EWeaponSlot::Pistol, DefaultPistol))
		{
			log(Error, "Can't shove default pistol into inventory");
			return;
		}

		if (not ShoveWeapon(EWeaponSlot::Main, DefaultMain))
		{
			log(Error, "Can't shove default pistol into inventory");
			return;
		}

		if (not SelectWeapon(EWeaponSlot::Knife))
		{
			log(Error, "Can't select default weapon");
		}
	}
}

bool UCloud9Inventory::SelectWeapon(EWeaponSlot Slot)
{
	if (Slot == EWeaponSlot::NotSelected)
	{
		log(Error, "Should not be called with EWeaponSlot::NotSelected");
		return false;
	}

	if (IsWeaponChanging())
	{
		log(Verbose, "[Weapon='%s'] Switching already in progress", *GetName());
		return false;
	}

	if (Slot == SelectedWeaponSlot)
	{
		log(Verbose, "[Weapon='%s'] No switching will be performed", *GetName());
		return false;
	}

	let PendingWeapon = GetWeaponAt(Slot);

	if (not IsValid(PendingWeapon))
	{
		log(Verbose, "[Weapon='%s'] Weapon at slot='%d' not set", *GetName(), Slot);
		return false;
	}

	if (SelectedWeaponSlot == EWeaponSlot::NotSelected)
	{
		if (PendingWeapon->ChangeState(EWeaponState::Armed))
		{
			PendingWeaponSlot = Slot;
			return true;
		}

		log(Error, "[Weapon='%s'] Can't select starting weapon", *GetName())
		return false;
	}

	if (let SelectedWeapon = GetWeaponAt(SelectedWeaponSlot); IsValid(SelectedWeapon)
		and SelectedWeapon->ChangeState(EWeaponState::Holstered)
		and PendingWeapon->ChangeState(EWeaponState::Armed))
	{
		PendingWeaponSlot = Slot;
		return true;
	}

	return false;
}

void UCloud9Inventory::OnWeaponChangeFinished()
{
	SelectedWeaponSlot = PendingWeaponSlot;
}

ACloud9WeaponBase* UCloud9Inventory::GetWeaponAt(EWeaponSlot Slot) const { return WeaponAt(Slot); }

bool UCloud9Inventory::ShoveWeapon(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "Inventory owner wasn't set");
		return false;
	}

	if (not Weapon->AddToInventory(Character, Slot))
	{
		log(Error, "Failed add to inventory weapon '%s'", *Weapon->GetName());
		return false;
	}

	WeaponAt(Slot) = Weapon;
	return true;
}

bool UCloud9Inventory::DropWeapon(EWeaponSlot Slot)
{
	if (let Character = GetOwner<ACloud9Character>(); not IsValid(Character))
	{
		log(Error, "Inventory owner wasn't set");
		return false;
	}

	let Weapon = WeaponAt(Slot);

	if (not Weapon)
	{
		log(Error, "Inventory cell for slot '%d' is empty", Slot);
		return false;
	}

	if (not Weapon->RemoveFromInventory())
	{
		log(Error, "Failed to remove from inventory weapon '%s'", *Weapon->GetName());
		return false;
	}

	WeaponAt(Slot) = nullptr; // make TArray cell empty
	return true;
}

bool UCloud9Inventory::ReplaceWeaponAt(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	if (not DropWeapon(Slot))
	{
		log(Error, "Can't drop weapon at slot='%d'", Slot);
		return false;
	}

	return ShoveWeapon(Slot, Weapon);
}

ACloud9WeaponBase* UCloud9Inventory::GetSelectedWeapon() const { return GetWeaponAt(SelectedWeaponSlot); }

ACloud9WeaponBase* UCloud9Inventory::GetPendingWeapon() const { return GetWeaponAt(PendingWeaponSlot); }

bool UCloud9Inventory::IsWeaponChanging() const { return SelectedWeaponSlot != PendingWeaponSlot; }

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
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Enums/WeaponState.h"

UCloud9Inventory::UCloud9Inventory()
{
	SelectedWeaponSlot = EWeaponSlot::NotSelected;

	let SlotsNumber = StaticEnum<EWeaponSlot>()->NumEnums();
	WeaponSlots.SetNum(SlotsNumber);
}

void UCloud9Inventory::BeginPlay()
{
	Super::BeginPlay();

	let MyOwner = GetOwner<ACloud9Character>();

	if (not IsValid(MyOwner))
	{
		log(Error, "[Actor='%s'] ACloud9Character isn't valid", *GetName());
		return;
	}

	let GameInstance = MyOwner->GetGameInstance<UCloud9GameInstance>();

	if (not IsValid(GameInstance))
	{
		log(Error, "[Actor='%s'] UCloud9GameInstance isn't valid", *GetName());
	}

	GameInstance->GetDefaultWeaponsConfig()
		| ETContainer::Filter{[this](let& Config) { return IsValid(Config); }}
		| ETContainer::ForEach{[this](let& Config) { AddWeapon(Config); }};

	let InitialWeaponSlot = GameInstance->GetInitialWeaponSlot();

	if (not SelectWeapon(InitialWeaponSlot))
	{
		log(Error,
		    "[Actor='%s'] Can't select default weapon slot='%s'",
		    *GetName(), InitialWeaponSlot | EUEnum::GetValueName() | EFName::ToCStr());
	}
}

bool UCloud9Inventory::SelectWeapon(EWeaponSlot Slot, bool Instant)
{
	if (Slot == EWeaponSlot::NotSelected)
	{
		log(Warning, "Should not be called with EWeaponSlot::NotSelected");
		return false;
	}

	if (IsWeaponChanging())
	{
		log(Warning, "[Weapon='%s'] Weapon deploying in progress", *GetName());
		return false;
	}

	if (Slot == SelectedWeaponSlot)
	{
		log(Warning, "[Weapon='%s'] No switching will be performed", *GetName());
		return false;
	}

	let PendingWeapon = GetWeaponAt(Slot);

	if (not IsValid(PendingWeapon))
	{
		log(Warning, "[Weapon='%s'] Weapon at requested slot='%d' not set or invalid", *GetName(), Slot);
		return false;
	}

	if (SelectedWeaponSlot == EWeaponSlot::NotSelected)
	{
		let SelectedWeapon = GetWeaponAt(SelectedWeaponSlot);

		assertf(
			not IsValid(SelectedWeapon),
			"[Weapon='%s'] SelectedWeapon should not be valid if slot == EWeaponSlot::NotSelected",
			*GetName());

		if (PendingWeapon->ChangeState(EWeaponState::Armed, Instant))
		{
			SelectedWeaponSlot = Slot;
			return true;
		}

		log(Error, "[Weapon='%s'] Can't select starting weapon", *GetName());
		return false;
	}

	let SelectedWeapon = GetSelectedWeapon();

	if (not IsValid(SelectedWeapon))
	{
		log(Error, "[Weapon='%s'] Weapon at selected slot='%d' not set or invalid", *GetName(), SelectedWeaponSlot);
		return false;
	}

	if (SelectedWeapon->ChangeState(EWeaponState::Holstered, true)
		and PendingWeapon->ChangeState(EWeaponState::Armed, Instant))
	{
		SelectedWeaponSlot = Slot;
		return true;
	}

	return false;
}

ACloud9WeaponBase* UCloud9Inventory::GetWeaponAt(EWeaponSlot Slot) const { return WeaponAt(Slot); }

bool UCloud9Inventory::ShoveWeapon(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	if (not IsValid(Weapon))
	{
		log(Error, "Weapon is invalid to shove into inventory");
		return false;
	}

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

bool UCloud9Inventory::AddWeapon(const FWeaponConfig& Config, bool Select, bool Force)
{
	if (let Character = GetOwner<ACloud9Character>(); not IsValid(Character))
	{
		log(Error, "[Inventory='%s'] Owner wasn't set", *GetName());
		return false;
	}

	if (IsWeaponChanging())
	{
		log(
			Error,
			"[Inventory='%s'] Weapon deploying in progress when added by config='%s'",
			*GetName(), *Config.ToString());
		return false;
	}

	let Slot = Config.GetWeaponSlot();

	if (WeaponAt(Slot))
	{
		if (not Force)
		{
			log(Error, "[Inventory='%s'] Weapon slot already occupied", *GetName());
			return false;
		}

		if (not RemoveWeapon(Slot))
		{
			log(Error, "[Inventory='%s'] Can't remove weapon", *GetName());
			return false;
		}
	}

	let Weapon = Config.SpawnWeapon(GetWorld());

	if (not IsValid(Weapon))
	{
		log(Error, "[Inventory='%s'] Can't spawn weapon by config: %s", *GetName(), *Config.ToString());
		return false;
	}

	if (not ShoveWeapon(Slot, Weapon))
	{
		log(Error, "[Inventory='%s'] Can't shove weapon by config: %s", *GetName(), *Config.ToString());
		return false;
	}

	if (Select)
	{
		if (not SelectWeapon(Slot))
		{
			log(Error, "[Inventory='%s'] New weapon selection failure: %s", *GetName(), *Config.ToString());
			return false;
		}
	}

	log(Display, "[Inventory='%s'] Added configured weapon = '%s'", *GetName(), *Config.ToString());

	return true;
}

bool UCloud9Inventory::RemoveWeapon(EWeaponSlot Slot)
{
	if (let Character = GetOwner<ACloud9Character>(); not IsValid(Character))
	{
		log(Error, "[Inventory='%s'] Owner is invalid", *GetName());
		return false;
	}

	if (not WeaponAt(Slot))
	{
		log(Error, "[Inventory='%s'] Cell for slot '%d' is empty", *GetName(), Slot);
		return false;
	}

	if (IsWeaponChanging())
	{
		log(Error, "[Inventory='%s'] Weapon deploying in progress when remove weapon from slot='%d'", *GetName(), Slot);
		return false;
	}

	if (Slot == SelectedWeaponSlot)
	{
		EWeaponSlot NewSlot = EWeaponSlot::NotSelected;
		let Found = WeaponSlots
			| ETContainer::Find{[&](let It) { return IsValid(It) and It->GetWeaponSlot() != Slot; }}
			| ETOptional::OnSet{[&](let It) { NewSlot = It->GetWeaponSlot(); }};

		log(
			Display,
			"[Inventory='%s'] Change selected/pending slot to '%s'",
			*GetName(), *UWeaponSlot::ToString(NewSlot));

		SelectWeapon(NewSlot, true);
	}

	WeaponAt(Slot)->Destroy();
	WeaponAt(Slot) = nullptr;

	return true;
}

ACloud9WeaponBase* UCloud9Inventory::GetSelectedWeapon() const { return GetWeaponAt(SelectedWeaponSlot); }

bool UCloud9Inventory::IsWeaponChanging() const
{
	let Weapon = GetSelectedWeapon();
	return Weapon and Weapon->IsDeploying();
}

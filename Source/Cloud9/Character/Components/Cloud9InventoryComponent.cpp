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

#include "Cloud9InventoryComponent.h"

#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Enums/WeaponBond.h"

UCloud9InventoryComponent::UCloud9InventoryComponent()
{
	SelectedWeaponSlot = EWeaponSlot::NotSelected;
	WeaponsCount = 0;

	let SlotsNumber = StaticEnum<EWeaponSlot>()->NumEnums();
	WeaponSlots.SetNum(SlotsNumber);
}

void UCloud9InventoryComponent::OnUnregister()
{
	Super::OnUnregister();
	// TODO: Drop the most expensive weapon when owner died but for now just destroy all items
	WeaponSlots
		| ETContainer::Filter{[](let It) { return It != nullptr; }}
		| ETContainer::ForEach{[](let It) { It->Destroy(); }};
}

bool UCloud9InventoryComponent::Initialize(const TArray<FWeaponConfig>& WeaponConfigs, EWeaponSlot WeaponSlot)
{
	WeaponConfigs
		| ETContainer::Filter{[](let& Config) { return IsValid(Config); }}
		| ETContainer::ForEach{[this](let& Config) { AddWeapon(Config); }};

	AssertOrReturn(
		SelectWeapon(WeaponSlot), false,
		Error, "Can't select default weapon slot='%s'", WeaponSlot | EUEnum::GetValueName() | EFName::ToCStr());

	return true;
}

bool UCloud9InventoryComponent::SelectWeaponImpl(EWeaponSlot Slot, bool Instant, bool Force)
{
	AssertOrReturn(
		Slot != EWeaponSlot::NotSelected, false,
		Warning, "Should not be called with EWeaponSlot::NotSelected");

	AssertOrReturn(not IsWeaponChanging(), false, Warning, "Weapon deploying in progress");
	AssertOrReturn(Force or Slot != SelectedWeaponSlot, false, Warning, "No switching will be performed");

	let PendingWeapon = GetWeaponAt(Slot);
	AssertOrReturn(IsValid(PendingWeapon), false, Warning, "Weapon at requested slot='%d' not set or invalid", Slot);

	if (SelectedWeaponSlot == EWeaponSlot::NotSelected)
	{
		let SelectedWeapon = GetWeaponAt(SelectedWeaponSlot);

		AssertOrCrash(
			not IsValid(SelectedWeapon),
			"[%s] SelectedWeapon should not be valid if slot == EWeaponSlot::NotSelected",
			*GetName());

		AssertOrReturn(
			PendingWeapon->ChangeState(EWeaponBond::Armed, Instant, Force), false,
			Error, "Can't select starting weapon");

		SelectedWeaponSlot = Slot;
		return true;
	}

	if (let SelectedWeapon = GetSelectedWeapon(); IsValid(SelectedWeapon))
	{
		AssertOrReturn(
			SelectedWeapon->ChangeState(EWeaponBond::Holstered, true, Force), false,
			Verbose, "Can't change state of selected weapon from slot='%d'", SelectedWeaponSlot);
	}

	AssertOrReturn(
		PendingWeapon->ChangeState(EWeaponBond::Armed, Instant, Force), false,
		Verbose, "Can't change state of pending weapon to slot='%d'", SelectedWeaponSlot);

	SelectedWeaponSlot = Slot;

	return true;
}

bool UCloud9InventoryComponent::SelectWeapon(EWeaponSlot Slot, bool Instant, bool Force)
{
	let BeforeSwitch = GetSelectedWeapon();
	if (SelectWeaponImpl(Slot, Instant, Force))
	{
		let AfterSwitch = GetSelectedWeapon();
		OnWeaponSwitchDelegate.Broadcast(BeforeSwitch, AfterSwitch);
		return true;
	}

	return false;
}

ACloud9WeaponBase* UCloud9InventoryComponent::GetWeaponAt(EWeaponSlot Slot) const { return WeaponAt(Slot); }

bool UCloud9InventoryComponent::ShoveWeapon(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	AssertOrReturn(IsValid(Weapon), false, Error, "Weapon is invalid to shove into inventory");

	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), false, Error, "Inventory owner wasn't set");

	AssertOrReturn(
		Weapon->AddToInventory(Character, Slot), false,
		Error, "Failed add to inventory weapon '%s'", *Weapon->GetName());

	WeaponAt(Slot) = Weapon;
	return true;
}

bool UCloud9InventoryComponent::DropWeapon(EWeaponSlot Slot, FVector ViewLocation, float Angle, float Impulse)
{
	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), false, Error, "Inventory owner wasn't set");

	let Weapon = WeaponAt(Slot);
	AssertOrReturn(Weapon, false, Error, "Inventory cell for slot '%d' is empty", Slot);

	let StartLocation = Weapon->GetActorLocation();
	let ViewDirection = ViewLocation - StartLocation | EFVector::Normalize{};
	let RotationAxis = Character->GetActorRightVector();

	let Direction = ViewDirection.RotateAngleAxis(-Angle, RotationAxis);

	AssertOrReturn(
		Weapon->RemoveFromInventory(), false,
		Error, "Failed to remove from inventory weapon '%s'", *Weapon->GetName());

	Weapon->SetActorLocation(StartLocation);

	let WeaponMesh = Weapon->GetWeaponMesh();
	WeaponMesh->AddImpulse(Impulse * Direction, NAME_None, true);

	WeaponAt(Slot) = nullptr; // make TArray cell empty
	return true;
}

bool UCloud9InventoryComponent::AddWeapon(const FWeaponConfig& Config, bool Select, bool Force)
{
	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), false, Error, "Inventory owner wasn't set");
	AssertOrReturn(
		not IsWeaponChanging(), false,
		Verbose, "Weapon deploying in progress when added by config='%s'", *Config.ToString());

	let Slot = Config.GetWeaponSlot();

	if (WeaponAt(Slot))
	{
		AssertOrReturn(Force, false, Error, "Weapon slot already occupied");
		AssertOrReturn(RemoveWeapon(Slot), false, Error, "Can't remove weapon");
	}

	let Weapon = Config.SpawnWeapon(GetWorld());

	AssertOrReturn(IsValid(Weapon), false, Error, "Can't spawn weapon by config: %s", *Config.ToString());
	AssertOrReturn(ShoveWeapon(Slot, Weapon), false, Error, "Can't shove weapon by config: %s", *Config.ToString());

	if (Select)
	{
		AssertOrReturn(SelectWeapon(Slot), false, Error, "New weapon selection failure: %s", *Config.ToString());
	}

	ObjectVerbose("Added configured weapon='%s'", *Config.ToString());

	WeaponsCount++;
	OnWeaponAddDelegate.Broadcast(Weapon);
	return true;
}

bool UCloud9InventoryComponent::RemoveWeapon(EWeaponSlot Slot)
{
	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), false, Error, "Inventory owner wasn't set");
	AssertOrReturn(WeaponAt(Slot), false, Error, "Cell for slot '%d' is empty", Slot);
	AssertOrReturn(
		not IsWeaponChanging(), false,
		Error, "Weapon deploying in progress when remove weapon from slot='%d'", Slot);

	if (Slot == SelectedWeaponSlot and not SelectOtherAvailableWeapon(true))
	{
		return false;
	}

	WeaponAt(Slot)->Destroy();
	WeaponAt(Slot) = nullptr;

	WeaponsCount--;
	OnWeaponRemoveDelegate.Broadcast();
	return true;
}

bool UCloud9InventoryComponent::IsEmpty() const { return WeaponsCount == 0; }

bool UCloud9InventoryComponent::SelectOtherAvailableWeapon(bool Instant, bool Force)
{
	EWeaponSlot NewSlot = EWeaponSlot::NotSelected;
	let Found = WeaponSlots
		| ETContainer::Filter{[&](let It) { return IsValid(It); }}
		| ETContainer::Find{[&](let It) { return It->GetWeaponSlot() != SelectedWeaponSlot; }}
		| ETOptional::OnSet{[&](let It) { NewSlot = It->GetWeaponSlot(); }};

	ObjectVerbose("Change selected slot='%d'", NewSlot);

	return SelectWeapon(NewSlot, Instant, Force);
}

ACloud9WeaponBase* UCloud9InventoryComponent::GetSelectedWeapon() const { return GetWeaponAt(SelectedWeaponSlot); }

bool UCloud9InventoryComponent::IsWeaponChanging() const
{
	let Weapon = GetSelectedWeapon();
	return Weapon and Weapon->IsDeploying();
}

TArray<ACloud9WeaponBase*>::TConstIterator UCloud9InventoryComponent::GetWeaponsIterator() const
{
	return WeaponSlots.CreateConstIterator();
}

const TArray<ACloud9WeaponBase*>& UCloud9InventoryComponent::GetWeapons() const
{
	return WeaponSlots;
}

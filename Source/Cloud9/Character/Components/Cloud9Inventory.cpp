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
#include "Cloud9/Weapon/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Cloud9WeaponKnife.h"
#include "Cloud9/Weapon/Cloud9WeaponPistol.h"


UCloud9Inventory::UCloud9Inventory()
{
	SelectedWeaponSlot = EWeaponSlot::NotSelected;
	PendingWeaponSlot = EWeaponSlot::NotSelected;

	let SlotsNumber = StaticEnum<EWeaponSlot>()->NumEnums();
	WeaponSlots.SetNum(SlotsNumber);

	DefaultKnifeClass = ACloud9WeaponKnife::StaticClass();
	DefaultPistolClass = ACloud9WeaponPistol::StaticClass();
}

void UCloud9Inventory::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	let DefaultKnife = GetWorld()->SpawnActor<ACloud9WeaponBase>(DefaultKnifeClass, SpawnParams);
	let DefaultPistol = GetWorld()->SpawnActor<ACloud9WeaponBase>(DefaultPistolClass, SpawnParams);
	// let DefaultMain = GetWorld()->SpawnActor<ACloud9WeaponBase>(ACloud9WeaponSniper::StaticClass(), SpawnParams);

	SetWeaponAt(EWeaponSlot::Knife, DefaultKnife);
	SetWeaponAt(EWeaponSlot::Pistol, DefaultPistol);
	// SetWeaponAt(EWeaponSlot::Main, DefaultMain);

	SelectWeapon(EWeaponSlot::Knife);
}

bool UCloud9Inventory::SelectWeapon(EWeaponSlot Slot)
{
	if (Slot == EWeaponSlot::NotSelected)
	{
		UE_LOG(LogCloud9, Error, TEXT("Should not be called with EWeaponSlot::NotSelected"))
		return false;
	}

	if (Slot != SelectedWeaponSlot)
	{
		if (let PendingWeapon = GetWeaponAt(Slot))
		{
			if (let SelectedWeapon = GetWeaponAt(SelectedWeaponSlot))
			{
				SelectedWeapon->SetActorHiddenInGame(true);
			}
			PendingWeapon->SetActorHiddenInGame(false);
			PendingWeaponSlot = Slot;
			return true;
		}

		return false;
	}

	return true;
}

void UCloud9Inventory::OnWeaponChangeFinished() { SelectedWeaponSlot = PendingWeaponSlot; }

bool UCloud9Inventory::SetWeaponAt(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	if (GetWeaponAt(Slot))
	{
		return false;
	}

	let Index = static_cast<int>(Slot);
	WeaponSlots[Index] = Weapon;
	return true;
}

ACloud9WeaponBase* UCloud9Inventory::GetWeaponAt(EWeaponSlot Slot) const
{
	let Index = static_cast<int>(Slot);
	return WeaponSlots[Index];
}

ACloud9WeaponBase* UCloud9Inventory::GetSelectedWeapon() const { return GetWeaponAt(SelectedWeaponSlot); }

bool UCloud9Inventory::IsWeaponChanging() const { return SelectedWeaponSlot != PendingWeaponSlot; }

EWeaponType UCloud9Inventory::GetSelectedWeaponType() const
{
	if (let Weapon = GetWeaponAt(SelectedWeaponSlot))
	{
		return Weapon->GetWeaponType();
	}

	return EWeaponType::NoWeapon;
}

EWeaponType UCloud9Inventory::GetPendingWeaponType() const
{
	if (let Weapon = GetWeaponAt(PendingWeaponSlot))
	{
		return Weapon->GetWeaponType();
	}

	return EWeaponType::NoWeapon;
}

EWeaponSlot UCloud9Inventory::GetSelectedWeaponSlot() const { return SelectedWeaponSlot; }

#include "Cloud9Inventory.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Weapon/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Cloud9WeaponKnife.h"
#include "Cloud9/Weapon/Cloud9WeaponPistol.h"
#include "Cloud9/Weapon/Cloud9WeaponSniper.h"


UCloud9Inventory::UCloud9Inventory()
{
	SelectedWeaponSlot = EWeaponSlot::NotSelected;
	PendingWeaponSlot = EWeaponSlot::NotSelected;

	const auto SlotsNumber = StaticEnum<EWeaponSlot>()->NumEnums();
	WeaponSlots.SetNum(SlotsNumber);
	
	DefaultKnifeClass = ACloud9WeaponKnife::StaticClass();
	DefaultPistolClass = ACloud9WeaponPistol::StaticClass();
}

void UCloud9Inventory::BeginPlay()
{
	Super::BeginPlay();
	const auto Owner = GetOwner();
	const auto DefaultKnife = NewObject<ACloud9WeaponBase>(Owner, DefaultKnifeClass);
	const auto DefaultPistol = NewObject<ACloud9WeaponBase>(Owner, DefaultPistolClass);
	const auto DefaultMain = NewObject<ACloud9WeaponBase>(Owner, ACloud9WeaponSniper::StaticClass());
	SetWeaponAt(EWeaponSlot::Knife, DefaultKnife);
	SetWeaponAt(EWeaponSlot::Pistol, DefaultPistol);
	SetWeaponAt(EWeaponSlot::Main, DefaultMain);
}

bool UCloud9Inventory::SelectWeapon(EWeaponSlot Slot)
{
	if (GetWeaponAt(Slot))
	{
		PendingWeaponSlot = Slot;
		return true;
	}
	
	return false;
}

bool UCloud9Inventory::SetWeaponAt(EWeaponSlot Slot, ACloud9WeaponBase* Weapon)
{
	if (GetWeaponAt(Slot))
		return false;

	const auto Index = static_cast<int>(Slot);
	WeaponSlots[Index] = Weapon;
	return true;
}

ACloud9WeaponBase* UCloud9Inventory::GetWeaponAt(EWeaponSlot Slot) const
{
	const auto Index = static_cast<int>(Slot);
	return WeaponSlots[Index];
}

EWeaponType UCloud9Inventory::GetSelectedWeaponType()
{
	if (const auto Weapon = GetWeaponAt(SelectedWeaponSlot))
		return Weapon->GetWeaponType();

	return EWeaponType::NoWeapon;
}

EWeaponType UCloud9Inventory::GetPendingWeaponType()
{
	if (const auto Weapon = GetWeaponAt(PendingWeaponSlot))
		return Weapon->GetWeaponType();

	return EWeaponType::NoWeapon;
}

void UCloud9Inventory::OnWeaponChangeFinished()
{
	SelectedWeaponSlot = PendingWeaponSlot;
}

void UCloud9Inventory::OnPoseUpdated()
{
	// UE_LOG(LogCloud9, Display, TEXT("GetMesh()->Bounds = %s"), *GetMesh()->Bounds.ToString());
}

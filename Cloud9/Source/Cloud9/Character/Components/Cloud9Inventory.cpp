#include "Cloud9Inventory.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Weapon/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Cloud9WeaponKnife.h"
#include "Cloud9/Weapon/Cloud9WeaponPistol.h"


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

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	const auto DefaultKnife = GetWorld()->SpawnActor<ACloud9WeaponBase>(DefaultKnifeClass, SpawnParams);
	const auto DefaultPistol = GetWorld()->SpawnActor<ACloud9WeaponBase>(DefaultPistolClass, SpawnParams);
	// const auto DefaultMain = GetWorld()->SpawnActor<ACloud9WeaponBase>(ACloud9WeaponSniper::StaticClass(), SpawnParams);
	
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
	
	if (Slot == SelectedWeaponSlot)
		return true;
	
	if (const auto PendingWeapon = GetWeaponAt(Slot))
	{
		if (const auto SelectedWeapon = GetWeaponAt(SelectedWeaponSlot))
			SelectedWeapon->SetActorHiddenInGame(true);
		PendingWeapon->SetActorHiddenInGame(false);
		PendingWeaponSlot = Slot;
		return true;
	}
	
	return false;
}

void UCloud9Inventory::OnWeaponChangeFinished()
{
	SelectedWeaponSlot = PendingWeaponSlot;
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

void UCloud9Inventory::OnPoseUpdated()
{
	// UE_LOG(LogCloud9, Display, TEXT("GetMesh()->Bounds = %s"), *GetMesh()->Bounds.ToString());
}

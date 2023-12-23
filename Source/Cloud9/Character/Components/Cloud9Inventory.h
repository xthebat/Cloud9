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

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Cloud9.h"
#include "Components/ActorComponent.h"
#include "Cloud9CharacterComponent.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "Cloud9Inventory.generated.h"

class ACloud9WeaponBase;
class ACloud9WeaponFirearm;
class ACloud9WeaponMelee;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9Inventory
	: public UActorComponent
	, public ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	UCloud9Inventory();

	/**
	 * Function selects weapon in new slot.
	 * 
	 * @param Slot New slot weapon slot.
	 * @param Instant If set then no animation will be shown and switching will be instant.
	 */
	UFUNCTION(BlueprintCallable)
	bool SelectWeapon(EWeaponSlot Slot, bool Instant = false);

	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetWeaponAt(EWeaponSlot Slot) const;

	/**
	 * Function shoves existed weapon into inventory
	 * 
	 * @param Slot Weapon slot to shove existed weapon
	 * @param Weapon Existed weapon object
	 */
	UFUNCTION(BlueprintCallable)
	bool ShoveWeapon(EWeaponSlot Slot, ACloud9WeaponBase* Weapon);

	/**
	 * Function drops to the ground weapon from inventory by defined slot
	 * 
	 * @param Slot Weapon slot to drop weapon from
	 */
	UFUNCTION(BlueprintCallable)
	bool DropWeapon(EWeaponSlot Slot);

	/**
	 * Function creates and add weapon to the inventory by specified config
	 * 
	 * @param Config Parameter to creates and shove weapon into inventory
	 * @param Select If set then function will select added weapon 
	 * @param Force Remote weapon if slot already occupied
	 */
	UFUNCTION(BlueprintCallable)
	bool AddWeapon(const FWeaponConfig& Config, bool Select = false, bool Force = false);

	UFUNCTION(BlueprintCallable)
	bool RemoveWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetSelectedWeapon() const;

	UFUNCTION(BlueprintCallable)
	bool IsWeaponChanging() const;

protected:
	virtual void BeginPlay() override;

	template <typename WeaponType = ACloud9WeaponBase>
	WeaponType* WeaponAt(EWeaponSlot Slot) const
	{
		let Index = static_cast<int>(Slot);
		return WeaponSlots[Index];
	}

	template <typename WeaponType = ACloud9WeaponBase>
	WeaponType*& WeaponAt(EWeaponSlot Slot)
	{
		let Index = static_cast<int>(Slot);
		return WeaponSlots[Index];
	}

protected:
	UPROPERTY()
	TArray<ACloud9WeaponBase*> WeaponSlots;

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	EWeaponSlot SelectedWeaponSlot;
};

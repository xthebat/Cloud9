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
#include "Components/ActorComponent.h"
#include "Cloud9CharacterComponent.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Utils/DefaultWeaponConfig.h"
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

	UFUNCTION(BlueprintCallable)
	bool SelectWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetWeaponAt(EWeaponSlot Slot) const;

	UFUNCTION(BlueprintCallable)
	bool ShoveWeapon(EWeaponSlot Slot, ACloud9WeaponBase* Weapon);

	UFUNCTION(BlueprintCallable)
	bool DropWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable)
	bool ReplaceWeaponAt(EWeaponSlot Slot, ACloud9WeaponBase* Weapon);

	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetSelectedWeapon() const;

	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetPendingWeapon() const;

	UFUNCTION(BlueprintCallable)
	bool IsWeaponChanging() const;

	UFUNCTION(BlueprintCallable)
	void OnWeaponChangeFinished();

protected:
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
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<ACloud9WeaponBase*> WeaponSlots;

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	EWeaponSlot SelectedWeaponSlot;

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	EWeaponSlot PendingWeaponSlot;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	TArray<FDefaultWeaponConfig> DefaultWeaponConfigs;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	EWeaponSlot InitialWeaponSlot;
};

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
#include "UObject/Object.h"

#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Enums/WeaponBond.h"
#include "Cloud9/Weapon/Enums/WeaponAction.h"

#include "WeaponState.generated.h"

USTRUCT(BlueprintType)
struct FWeaponState
{
	GENERATED_BODY()

	FWeaponState();

	void Reset();

	void ActivateAction(EWeaponAction Action, bool IsReleased);

	bool IsActionActive(EWeaponAction Action) const;

	void ClearAction(EWeaponAction Action);

	FORCEINLINE bool IsWeaponBond(EWeaponBond Check) const { return Bond == Check; }

	void OnUpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponBond NewBond, bool Instant);

	void OnRemovedFromInventory();

	EWeaponSlot GetWeaponSlot() const;

protected:
	void ClearAllActions();

	FORCEINLINE bool operator[](EWeaponAction Action) const { return Actions[Action | EUEnum::To<int>{}]; }

	FORCEINLINE bool& operator[](EWeaponAction Action) { return Actions[Action | EUEnum::To<int>{}]; }

	/**
	 * Current weapon slot (main/pistol/knife/grenade)
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponSlot Slot;

	/**
	 * Current weapon state (armed/holstered/dropped)
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponBond Bond;

	/**
	 * Current weapon actions in process
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<bool> Actions;
};

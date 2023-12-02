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
#include "Cloud9/Weapon/Enums/Cloud9FirearmNames.h"
#include "Cloud9/Weapon/Enums/Cloud9GrenadeNames.h"
#include "Cloud9/Weapon/Enums/Cloud9MeleeNames.h"
#include "Cloud9/Weapon/Enums/Cloud9WeaponType.h"
#include "Engine/GameInstance.h"
#include "Cloud9GameInstance.generated.h"

class UDataTable;
class ACloud9Character;
class ACloud9WeaponMelee;
class ACloud9WeaponGrenade;
class ACloud9WeaponFirearm;

USTRUCT(BlueprintType)
struct FWeaponActionMontages
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action)
	UAnimMontage* PrimaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action)
	UAnimMontage* SecondaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Reload)
	UAnimMontage* ReloadMontage = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponPosesMontages
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FWeaponActionMontages OnStand;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FWeaponActionMontages OnCrouch;

	const FWeaponActionMontages* GetPoseMontages(bool bIsCrouch) const { return bIsCrouch ? &OnCrouch : &OnStand; }
};

UCLASS()
class CLOUD9_API UCloud9GameInstance : public UGameInstance
{
	GENERATED_BODY()

public: // functions
	/**
	 * Function spawns firearm weapon to given owner (Character)
	 */
	ACloud9WeaponFirearm* SpawnFirearmWeapon(
		EFirearm WeaponId,
		ACloud9Character* Character = nullptr,
		const FTransform& Transform = FTransform::Identity) const;

	/**
	 * Function spawns melee weapon to given owner (Character)
	 */
	ACloud9WeaponMelee* SpawnMeleeWeapon(
		EMelee WeaponId,
		ACloud9Character* Character = nullptr,
		const FTransform& Transform = FTransform::Identity) const;

	/**
	 * Function spawns grenade weapon to given owner (Character)
	 */
	ACloud9WeaponGrenade* SpawnGrenadeWeapon(
		EGrenade WeaponId,
		ACloud9Character* Character = nullptr,
		const FTransform& Transform = FTransform::Identity) const;

protected: // properties
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* FirearmsWeaponsInfoTable;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* MeleeWeaponsInfoTable;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* GrenadeWeaponInfoTable;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	TMap<EWeaponType, FWeaponPosesMontages> WeaponActionMontages;

	/**
	 * Weapon tracer
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UParticleSystem* FirearmTracer = nullptr;

	template <
		typename WeaponActorType,
		typename WeaponInfoType,
		typename WeaponEnumType,
		typename ValidatorType
	>
	WeaponActorType* SpawnWeaponIntern(
		UDataTable* WeaponsInfoTable,
		ValidatorType Validator,
		WeaponEnumType WeaponId,
		ACloud9Character* Character,
		const FTransform& Transform) const;
};

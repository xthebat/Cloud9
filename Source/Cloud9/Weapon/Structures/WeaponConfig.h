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
#include "Cloud9/Tools/Extensions/UWorld.h"
#include "Cloud9/Weapon/Enums/FirearmNames.h"
#include "Cloud9/Weapon/Enums/GrenadeNames.h"
#include "Cloud9/Weapon/Enums/MeleeNames.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Enums/WeaponId.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Tables/WeaponTableBase.h"
#include "WeaponConfig.generated.h"

class ACloud9WeaponBase;

USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_BODY()

	friend bool IsValid(const FWeaponConfig& Config)
	{
		return Config.bIsEnabled and Config.WeaponClass != EWeaponClass::NoClass;
	}

	bool IsMeleeWeapon() const { return WeaponClass == EWeaponClass::Melee; }

	bool IsFirearmWeapon() const { return WeaponClass == EWeaponClass::Firearm; }

	bool IsGrenadeWeapon() const { return WeaponClass == EWeaponClass::Grenade; }

	FName GetClassName() const { return WeaponClass | EUEnum::GetValueName(); }

	EWeaponSlot GetWeaponSlot() const;

	FWeaponId GetWeaponId() const;

	FName GetWeaponName() const;

	FName GetSkinName() const { return SkinName; }

	TSubclassOf<AActor> GetWeaponStaticClass() const;

	bool Initialize(AActor* Actor) const;

	template <typename WeaponClassType>
	bool Initialize(WeaponClassType* Weapon) const
	{
		if (not IsValid(Weapon))
		{
			log(Error, "[Config='%s'] Weapon to initialize is invalid", *ToString());
			return false;
		}

		log(Display, "[Config='%s'] Initializing weapon...", *ToString());
		return Weapon->Initialize(GetWeaponId(), GetSkinName());
	}

	ACloud9WeaponBase* SpawnWeapon(UWorld* World) const;

	template <typename WeaponClassType>
	WeaponClassType* SpawnWeapon(UWorld* World) const
	{
		return World | EUWorld::SpawnActor<WeaponClassType>{
			.Initializer = [this](let It) { return Initialize<WeaponClassType>(It); },
		};
	}

	FString ToString() const;

protected:
	/**
	 * Is weapon enabled (useful when testing different kind of weapons)
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly)
	bool bIsEnabled = true;

	/**
	 * Weapon configured class
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsEnabled", EditConditionHides))
	EWeaponClass WeaponClass = EWeaponClass::NoClass;

	/**
	 * Melee weapon name (if melee class selected)
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Melee", EditConditionHides))
	EMelee MeleeWeaponId = EMelee::Knife;

	/**
	 * Firearm weapon name (if firearm class selected)
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	EFirearm FirearmWeaponId = EFirearm::Ak47;

	/**
	 * Grenade weapon name (if grenade class selected)
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Grenade", EditConditionHides))
	EGrenade GrenadeWeaponId = EGrenade::Explosive;

	/**
	 * Is firearm in primary or in secondary slot (i.e. main or pistol) 
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	bool bIsPrimary = false;

	/**
	 * Skin name of the weapon 
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsEnabled", EditConditionHides))
	FName SkinName = FWeaponSkin::Default;
};

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
#include "Cloud9/Tools/Extensions/WhenOrNone.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponMelee.h"
#include "Cloud9/Weapon/Enums/FirearmNames.h"
#include "Cloud9/Weapon/Enums/GrenadeNames.h"
#include "Cloud9/Weapon/Enums/MeleeNames.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Tables/WeaponTableBase.h"
#include "WeaponConfig.generated.h"

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

	EWeaponSlot GetWeaponSlot() const
	{
		switch (WeaponClass)
		{
		case EWeaponClass::Melee:
			return EWeaponSlot::Knife;
		case EWeaponClass::Firearm:
			return bIsPrimary ? EWeaponSlot::Main : EWeaponSlot::Pistol;
		case EWeaponClass::Grenade:
			return EWeaponSlot::Grenade;
		default:
			return EWeaponSlot::NotSelected;
		}
	}

	FWeaponId GetWeaponId() const
	{
		switch (WeaponClass)
		{
		case EWeaponClass::Melee:
			return ETVariant::Convert<FWeaponId>(MeleeWeaponId);
		case EWeaponClass::Firearm:
			return ETVariant::Convert<FWeaponId>(FirearmWeaponId);
		case EWeaponClass::Grenade:
			return ETVariant::Convert<FWeaponId>(GrenadeWeaponId);
		default:
			return ETVariant::Convert<FWeaponId>(EBadWeapon::NoClass);
		}
	}

	FName GetWeaponName() const { return GetWeaponId() | EFWeaponId::ToName(); }

	FName GetSkinName() const { return SkinName; }

	TSubclassOf<ACloud9WeaponBase> GetWeaponStaticClass() const
	{
		switch (WeaponClass)
		{
		case EWeaponClass::Melee:
			return ACloud9WeaponMelee::StaticClass();
		case EWeaponClass::Firearm:
			return ACloud9WeaponFirearm::StaticClass();
		case EWeaponClass::Grenade:
			return ACloud9WeaponGrenade::StaticClass();
		default:
			return nullptr;
		}
	}

	bool Initialize(AActor* Actor) const
	{
		if (not IsValid(Actor))
		{
			log(Error, "Actor is invalid")
			return false;
		}

		let IsInitialized = Actor | WhenOrNone{
			[this](ACloud9WeaponMelee* It) { return Initialize(It); },
			[this](ACloud9WeaponFirearm* It) { return Initialize(It); },
			[this](ACloud9WeaponGrenade* It) { return Initialize(It); }
		};

		if (not IsInitialized.Get(false))
		{
			log(Error, "[Actor='%s' Config='%s'] Weapon initialization failure", *Actor->GetName(), *ToString());
			return false;
		}

		return true;
	}

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

	ACloud9WeaponBase* Spawn(UWorld* World) const
	{
		switch (WeaponClass)
		{
		case EWeaponClass::Melee:
			return SpawnIntern<ACloud9WeaponMelee>(World);
		case EWeaponClass::Firearm:
			return SpawnIntern<ACloud9WeaponFirearm>(World);
		case EWeaponClass::Grenade:
			return SpawnIntern<ACloud9WeaponGrenade>(World);
		default:
			return nullptr;
		}
	}

	template <typename WeaponClassType>
	ACloud9WeaponBase* SpawnIntern(UWorld* World) const
	{
		return World | EUWorld::SpawnActor<WeaponClassType>{
			.Initializer = [this](let It) { return Initialize<WeaponClassType>(It); },
		};
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT("WeaponConfig { Class='%s' Name='%s' Skin='%s' }"),
			GetClassName() | EFName::ToCStr(),
			GetWeaponName() | EFName::ToCStr(),
			GetSkinName() | EFName::ToCStr()
		);
	}

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

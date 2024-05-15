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
#include "WeaponDefinition.h"
#include "UObject/Object.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/UWorld.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
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

	FWeaponConfig() = default;

	explicit FWeaponConfig(EMelee WeaponId, FName SkinName = FWeaponSkin::Default)
		: WeaponClass(EWeaponClass::Melee)
		, MeleeWeaponId(WeaponId)
		, SkinName(SkinName) {}

	explicit FWeaponConfig(EGrenade WeaponId, FName SkinName = FWeaponSkin::Default)
		: WeaponClass(EWeaponClass::Grenade)
		, GrenadeWeaponId(WeaponId)
		, SkinName(SkinName) {}

	FWeaponConfig(
		EFirearm WeaponId,
		bool IsPrimary,
		int AmmoInMagazine,
		int AmmoInReserve,
		FName SkinName = FWeaponSkin::Default
	) : WeaponClass(EWeaponClass::Firearm)
	  , FirearmWeaponId(WeaponId)
	  , bIsPrimary(IsPrimary)
	  , AmmoInMagazine(AmmoInMagazine)
	  , AmmoInReserve(AmmoInReserve)
	  , SkinName(SkinName) {}

	friend bool IsValid(const FWeaponConfig& Config)
	{
		return Config.bIsEnabled and Config.WeaponClass != EWeaponClass::NoClass;
	}

	bool IsMeleeWeapon() const { return WeaponClass == EWeaponClass::Melee; }

	bool IsFirearmWeapon() const { return WeaponClass == EWeaponClass::Firearm; }

	bool IsGrenadeWeapon() const { return WeaponClass == EWeaponClass::Grenade; }

	FName GetClassName() const { return WeaponClass | EUEnum::GetValueName{}; }

	EWeaponSlot GetWeaponSlot() const;

	FWeaponId GetWeaponId() const;

	FName GetWeaponName() const;

	FName GetSkinName() const { return SkinName; }

	/**
	 * WARNING: Can return -1 if AmmoInMagazine not configured
	 */
	int GetAmmoInMagazine(int Default = -1) const
	{
		return AmmoInMagazine < 0 ? Default : AmmoInMagazine;
	}

	/**
	 * WARNING: Can return -1 if AmmoInReserve not configured
	 */
	int GetAmmoInReserve(int Default = -1) const
	{
		return AmmoInReserve < 0 ? Default : AmmoInReserve;
	}

	TSubclassOf<AActor> GetWeaponStaticClass() const;

	bool Initialize(AActor* Actor) const;

	template <typename WeaponClassType>
	bool Initialize(WeaponClassType* Weapon) const
	{
		RETURN_IF_FAIL(IsValid(Weapon), false, Error, "Weapon to initialize is invalid");
		FUNCTION_VERBOSE("[%s] Initializing weapon...", *ToString());
		return Weapon->Initialize(*this);
	}

	ACloud9WeaponBase* SpawnWeapon(UWorld* World) const;

	template <typename WeaponClassType>
	WeaponClassType* SpawnWeapon(UWorld* World) const
	{
		return World | EUWorld::SpawnActor<WeaponClassType>{
			.Initializer = [this](let It) { return Initialize<WeaponClassType>(It); },
		};
	}

	/**
	 * Function converts WeaponConfig to a human-readable string
	 */
	FString ToString() const;

	/**
	 * Creates WeaponConfig from a specified Weapon (required for map change with save Weapon)
	 * 
	 * @param Weapon Weapon to save configuration and state
	 */
	static FWeaponConfig FromWeapon(const ACloud9WeaponBase* Weapon);

protected:
	/**
 	 * Required for WeaponSpawner to correct AmmoInMagazine and AmmoInReserve for a Weapon
 	 */
	void PostEditChangeProperty()
	{
		let Asset = ACloud9WeaponBase::GetWeaponDefinitionsAsset();
		VOID_IF_FAIL(IsValid(Asset), Error, "WCan't get WeaponDefinitionsAsset");

		if (WeaponClass == EWeaponClass::Firearm)
		{
			FWeaponDefinition WeaponDefinition;
			VOID_IF_FAIL(
				Asset->GetWeaponDefinition(GetWeaponId(), WeaponDefinition),
				Error, "Can't get WeaponDefinition for firearm = %d", FirearmWeaponId);

			let FirearmWeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();

			AmmoInMagazine = FirearmWeaponInfo->MagazineSize;
			AmmoInReserve = FirearmWeaponInfo->MaxAmmoInReserve;
		}
	}

	friend class AWeaponSpawner;

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
	 * Number of ammo to set in magazine for this weapon
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Firearm",
			UIMin="0", UIMax="150", ClampMin="0", ClampMax="150", EditConditionHides))
	int AmmoInMagazine = -1; // set to max clamped in PostEditChangeProperty event

	/**
	 * Number of ammo to set in reserve for this weapon
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="bIsEnabled && WeaponClass == EWeaponClass::Firearm",
			UIMin="0", UIMax="150", ClampMin="0", ClampMax="150", EditConditionHides))
	int AmmoInReserve = -1; // set to max clamped in PostEditChangeProperty event

	/**
	 * Skin name of the weapon 
	 */
	UPROPERTY(Category=Weapon, EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bIsEnabled", EditConditionHides))
	FName SkinName = FWeaponSkin::Default;
};

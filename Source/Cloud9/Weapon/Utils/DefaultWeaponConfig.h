﻿// Copyright (c) 2023 Alexei Gladkikh
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
#include "Cloud9/Tools/Extensions/UObject.h"
#include "Cloud9/Weapon/Enums/FirearmNames.h"
#include "Cloud9/Weapon/Enums/GrenadeNames.h"
#include "Cloud9/Weapon/Enums/MeleeNames.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Tables/WeaponTableBase.h"
#include "DefaultWeaponConfig.generated.h"

USTRUCT(BlueprintType)
struct CLOUD9_API FDefaultWeaponConfig
{
	GENERATED_BODY()

public:
	bool IsMeleeWeapon() const { return WeaponClass == EWeaponClass::Melee; }

	bool IsFirearmWeapon() const { return WeaponClass == EWeaponClass::Firearm; }

	bool IsGrenadeWeapon() const { return WeaponClass == EWeaponClass::Grenade; }

	EWeaponSlot GetWeaponSlot() const
	{
		if (IsMeleeWeapon())
		{
			return EWeaponSlot::Knife;
		}

		if (IsFirearmWeapon())
		{
			return bIsPrimary ? EWeaponSlot::Main : EWeaponSlot::Pistol;
		}

		if (IsGrenadeWeapon())
		{
			return EWeaponSlot::Grenade;
		}

		return EWeaponSlot::NotSelected;
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

	EMelee GetMeleeWeaponName() const { return MeleeWeaponName; }

	EFirearm GetFirearmWeaponName() const { return FirearmWeaponName; }

	EGrenade GetGrenadeWeaponName() const { return GrenadeWeaponName; }

	FName GetClassName() const { return WeaponClass | EUEnum::GetValueName(); }

	FName GetWeaponName() const
	{
		if (IsMeleeWeapon())
		{
			return MeleeWeaponName | EUEnum::GetValueName();
		}

		if (IsFirearmWeapon())
		{
			return FirearmWeaponName | EUEnum::GetValueName();
		}

		if (IsGrenadeWeapon())
		{
			return GrenadeWeaponName | EUEnum::GetValueName();
		}

		return NAME_None;
	}

	FName GetSkinName() const { return SkinName; }

protected:
	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	EWeaponClass WeaponClass = EWeaponClass::NoClass;

	UPROPERTY(Category=Weapon, EditDefaultsOnly,
		meta=(EditCondition="WeaponClass == EWeaponClass::Melee", EditConditionHides))
	EMelee MeleeWeaponName = EMelee::Knife;

	UPROPERTY(Category=Weapon, EditDefaultsOnly,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	EFirearm FirearmWeaponName = EFirearm::Ak47;

	UPROPERTY(Category=Weapon, EditDefaultsOnly,
		meta=(EditCondition="WeaponClass == EWeaponClass::Grenade", EditConditionHides))
	EGrenade GrenadeWeaponName = EGrenade::Explosive;

	UPROPERTY(Category=Weapon, EditDefaultsOnly,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	bool bIsPrimary = false;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	FName SkinName = FWeaponSkin::Default;
};

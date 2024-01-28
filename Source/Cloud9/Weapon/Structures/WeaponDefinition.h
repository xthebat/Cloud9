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

#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Tables/WeaponCommonData.h"
#include "Cloud9/Weapon/Tables/WeaponTableBase.h"
#include "Cloud9/Weapon/Tables/WeaponMontages.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"
#include "Cloud9/Weapon/Tables/WeaponTableGrenade.h"
#include "Cloud9/Weapon/Tables/WeaponTableMelee.h"

#include "WeaponDefinition.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDefinition
{
	GENERATED_BODY()

	friend bool IsValid(const FWeaponDefinition& WeaponDefinition)
	{
		return WeaponDefinition.WeaponClass != EWeaponClass::NoClass;
	}

	FWeaponDefinition() = default;

	FWeaponDefinition(FMeleeWeaponInfo WeaponInfo, FWeaponPosesMontages Montages, FWeaponCommonData WeaponCommonData)
		: WeaponClass(EWeaponClass::Melee)
		, MeleeWeaponInfo(MoveTemp(WeaponInfo))
		, Montages(MoveTemp(Montages))
		, WeaponCommonData(MoveTemp(WeaponCommonData)) {}

	FWeaponDefinition(FFirearmWeaponInfo WeaponInfo, FWeaponPosesMontages Montages, FWeaponCommonData WeaponCommonData)
		: WeaponClass(EWeaponClass::Firearm)
		, FirearmWeaponInfo(MoveTemp(WeaponInfo))
		, Montages(MoveTemp(Montages))
		, WeaponCommonData(MoveTemp(WeaponCommonData)) {}

	FWeaponDefinition(FGrenadeWeaponInfo WeaponInfo, FWeaponPosesMontages Montages, FWeaponCommonData WeaponCommonData)
		: WeaponClass(EWeaponClass::Grenade)
		, GrenadeWeaponInfo(MoveTemp(WeaponInfo))
		, Montages(MoveTemp(Montages))
		, WeaponCommonData(MoveTemp(WeaponCommonData)) {}

	// TODO: Refactor FWeaponDefinition somehow...
	template <typename WeaponInfoType = FBaseWeaponInfo>
	FORCEINLINE const WeaponInfoType* GetWeaponInfo() const
	{
		if constexpr (TIsSame<WeaponInfoType, FMeleeWeaponInfo>::Value)
		{
			if (WeaponClass == EWeaponClass::Melee)
			{
				return static_cast<const WeaponInfoType*>(&MeleeWeaponInfo);
			}
		}

		if constexpr (TIsSame<WeaponInfoType, FFirearmWeaponInfo>::Value)
		{
			if (WeaponClass == EWeaponClass::Firearm)
			{
				return static_cast<const WeaponInfoType*>(&FirearmWeaponInfo);
			}
		}

		if constexpr (TIsSame<WeaponInfoType, FGrenadeWeaponInfo>::Value)
		{
			if (WeaponClass == EWeaponClass::Grenade)
			{
				return static_cast<const WeaponInfoType*>(&GrenadeWeaponInfo);
			}
		}

		if constexpr (TIsSame<WeaponInfoType, FBaseWeaponInfo>::Value)
		{
			if (WeaponClass == EWeaponClass::Melee)
			{
				return static_cast<const WeaponInfoType*>(&MeleeWeaponInfo);
			}

			if (WeaponClass == EWeaponClass::Firearm)
			{
				return static_cast<const WeaponInfoType*>(&FirearmWeaponInfo);
			}

			if (WeaponClass == EWeaponClass::Grenade)
			{
				return static_cast<const WeaponInfoType*>(&GrenadeWeaponInfo);
			}
		}

		log(Error, "WeaponInfo class wasn't set")
		return nullptr;
	}

	FORCEINLINE const FWeaponActionMontages* GetPoseMontages(bool bIsCrouch) const
	{
		return not bIsCrouch ? &Montages.OnStand : &Montages.OnCrouch;
	}

	FORCEINLINE const FWeaponCommonData* GetCommonData() const { return &WeaponCommonData; }

	void Reset()
	{
		if (WeaponClass == EWeaponClass::Melee)
		{
			MeleeWeaponInfo = {};
		}

		if (WeaponClass == EWeaponClass::Firearm)
		{
			FirearmWeaponInfo = {};
		}

		if (WeaponClass == EWeaponClass::Grenade)
		{
			GrenadeWeaponInfo = {};
		}

		WeaponClass = EWeaponClass::NoClass;
		WeaponCommonData = {};
	}

protected:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponClass WeaponClass = EWeaponClass::NoClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FMeleeWeaponInfo MeleeWeaponInfo{};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FFirearmWeaponInfo FirearmWeaponInfo{};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FGrenadeWeaponInfo GrenadeWeaponInfo{};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FWeaponPosesMontages Montages{};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FWeaponCommonData WeaponCommonData{};
};

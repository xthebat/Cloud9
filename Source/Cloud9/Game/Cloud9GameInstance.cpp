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

#include "Cloud9GameInstance.h"

#include "Cloud9/Tools/Extensions/UEnum.h"
#include "Cloud9/Tools/Extensions/UWorld.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"
#include "Cloud9/Weapon/Tables/WeaponTableGrenade.h"
#include "Cloud9/Weapon/Tables/WeaponTableMelee.h"

template <typename WeaponInfoType, typename ValidatorType>
TOptional<FWeaponInstance> UCloud9GameInstance::GetWeaponInstance(
	UDataTable* WeaponsInfoTable,
	ValidatorType Validator,
	FName WeaponName) const
{
	if (WeaponName.IsNone())
	{
		log(Error, "WeaponName is invalid");
		return {};
	}

	if (WeaponsInfoTable == nullptr)
	{
		log(Error, "WeaponsInfoTable isn't set");
		return {};
	}

	let WeaponInfo = WeaponsInfoTable->FindRow<WeaponInfoType>(WeaponName, "", false);

	if (WeaponInfo == nullptr)
	{
		log(Error, "Can't get weapon info for '%s'", *WeaponName.ToString());
		return {};
	}

	if (not Validator(WeaponInfo->Type))
	{
		let TypeName = WeaponInfo->Type | EUEnum::GetEnumFullValueName();
		log(Error, "Specified weapon type '%s' is invalid", *TypeName.ToString());
		return {};
	}

	let Montages = WeaponActionMontages.Find(WeaponInfo->Type);

	if (Montages == nullptr)
	{
		let TypeName = WeaponInfo->Type | EUEnum::GetValueName();
		log(Error, "Animation montages not defined for weapon type '%s'", *TypeName.ToString());
		return {};
	}

	return FWeaponInstance{WeaponInfo, Montages, FirearmTracer};
}

TOptional<FWeaponInstance> UCloud9GameInstance::GetWeaponInstance(EWeaponClass WeaponClass, FName WeaponName) const
{
	switch (WeaponClass)
	{
	case EWeaponClass::Firearm:
		return GetWeaponInstance<FFirearmWeaponInfo>(FirearmsWeaponsInfoTable, &UWeaponType::IsFirearm, WeaponName);
	case EWeaponClass::Melee:
		return GetWeaponInstance<FMeleeWeaponInfo>(MeleeWeaponsInfoTable, &UWeaponType::IsMelee, WeaponName);
	case EWeaponClass::Grenade:
		return GetWeaponInstance<FGrenadeWeaponInfo>(GrenadeWeaponInfoTable, &UWeaponType::IsGrenade, WeaponName);
	default:
		return {};
	}
}

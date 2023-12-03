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
#include "Cloud9/Weapon/Enums/Cloud9WeaponType.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableFirearm.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableGrenade.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableMelee.h"

template <
	typename WeaponActorType,
	typename WeaponInfoType,
	typename WeaponEnumType,
	typename ValidatorType
>
WeaponActorType* UCloud9GameInstance::SpawnWeaponIntern(
	UDataTable* WeaponsInfoTable,
	ValidatorType Validator,
	WeaponEnumType WeaponId,
	ACloud9Character* Character,
	const FTransform& Transform) const
{
	if (WeaponsInfoTable == nullptr)
	{
		log(Error, "WeaponsInfoTable isn't set");
		return nullptr;
	}

	let Name = WeaponId | EUEnum::GetValueName();

	if (Name.IsNone())
	{
		log(Error, "Can't get weapon identifier name");
		return nullptr;
	}

	let WeaponInfo = WeaponsInfoTable->FindRow<WeaponInfoType>(Name, "", false);

	if (WeaponInfo == nullptr)
	{
		log(Error, "Can't get weapon info for '%s'", *Name.ToString());
		return nullptr;
	}

	if (not Validator(WeaponInfo->Type))
	{
		let TypeName = WeaponInfo->Type | EUEnum::GetEnumFullValueName();
		log(Fatal, "Specified weapon type '%s' is invalid", *TypeName.ToString());
		return nullptr;
	}

	let Montages = WeaponActionMontages.Find(WeaponInfo->Type);

	if (Montages == nullptr)
	{
		let WeaponName = WeaponInfo->Type | EUEnum::GetValueName();
		log(Fatal, "Animation montages not defined for weapon type '%s'", *WeaponName.ToString());
		return nullptr;
	}

	return GetWorld() | EUWorld::SpawnActorInitialized(
		WeaponInfo->Class,
		[=](let It) { return It->Initialize(WeaponInfo, Montages); },
		Transform,
		Character
	);
}

ACloud9WeaponFirearm* UCloud9GameInstance::SpawnFirearmWeapon(
	EFirearm WeaponId,
	ACloud9Character* Character,
	const FTransform& Transform) const
{
	return SpawnWeaponIntern<ACloud9WeaponFirearm, FFirearmWeaponInfo>(
		FirearmsWeaponsInfoTable,
		&UCloud9WeaponType::IsFirearm,
		WeaponId,
		Character,
		Transform
	);
}

ACloud9WeaponMelee* UCloud9GameInstance::SpawnMeleeWeapon(
	EMelee WeaponId,
	ACloud9Character* Character,
	const FTransform& Transform) const
{
	return SpawnWeaponIntern<ACloud9WeaponMelee, FMeleeWeaponInfo>(
		MeleeWeaponsInfoTable,
		&UCloud9WeaponType::IsMelee,
		WeaponId,
		Character,
		Transform
	);
}

ACloud9WeaponGrenade* UCloud9GameInstance::SpawnGrenadeWeapon(
	EGrenade WeaponId,
	ACloud9Character* Character,
	const FTransform& Transform) const
{
	return SpawnWeaponIntern<ACloud9WeaponGrenade, FGrenadeWeaponInfo>(
		GrenadeWeaponInfoTable,
		&UCloud9WeaponType::IsGrenade,
		WeaponId,
		Character,
		Transform
	);
}

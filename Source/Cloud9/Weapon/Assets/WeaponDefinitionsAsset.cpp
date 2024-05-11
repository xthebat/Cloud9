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

#include "WeaponDefinitionsAsset.h"

#include "Cloud9/Tools/Extensions/FString.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Weapon/Extensions/EWeaponId.h"

const FPrimaryAssetId UWeaponDefinitionsAsset::PrimaryAssetId = FPrimaryAssetId("Weapon:Definitions");
const FString UWeaponDefinitionsAsset::Path = TEXT("/Game/Weapons");

template <typename WeaponIdType, typename ValidatorType>
FWeaponDefinition UWeaponDefinitionsAsset::GetWeaponDefinition(
	UDataTable* WeaponsInfoTable,
	ValidatorType Validator,
	WeaponIdType WeaponId) const
{
	using WeaponInfoType = typename EFWeaponId::WeaponInfo<WeaponIdType>::Type;

	let WeaponName = WeaponId | EUEnum::GetValueName{};
	AssertOrReturn(not WeaponName.IsNone(), {}, Error, "WeaponName/WeaponId is invalid");
	AssertOrReturn(WeaponsInfoTable, {}, Error, "WeaponsInfoTable isn't set");

	let WeaponInfo = WeaponsInfoTable->FindRow<WeaponInfoType>(WeaponName, "", false);
	AssertOrReturn(WeaponInfo, {}, Error, "Can't get weapon info for '%s'", *WeaponName.ToString());
	AssertOrReturn(
		Validator(WeaponInfo->Type), {},
		Error, "Specified weapon type '%s' is invalid",
		WeaponInfo->Type | EUEnum::GetEnumFullValueName{} | EFName::ToCStr{});

	let Montages = WeaponActionMontages.Find(WeaponInfo->Type);
	AssertOrReturn(
		Montages, {},
		Error, "Animation montages not defined for weapon type '%s'",
		WeaponInfo->Type | EUEnum::GetValueName{} | EFName::ToCStr{});

	return FWeaponDefinition(*WeaponInfo, *Montages, WeaponCommonData);
}

bool UWeaponDefinitionsAsset::GetWeaponDefinition(const FWeaponId& WeaponId, FWeaponDefinition& WeaponDefinition) const
{
	WeaponDefinition = Visit(
		ETVariant::Overloaded{
			[&](EFirearm Id) { return GetWeaponDefinition(FirearmsWeaponsInfoTable, UWeaponType::IsFirearm, Id); },
			[&](EMelee Id) { return GetWeaponDefinition(MeleeWeaponsInfoTable, UWeaponType::IsMelee, Id); },
			[&](EGrenade Id) { return GetWeaponDefinition(GrenadeWeaponInfoTable, UWeaponType::IsGrenade, Id); },
			[](auto) { return FWeaponDefinition{}; }
		}, WeaponId
	);

	AssertOrReturn(
		IsValid(WeaponDefinition), false,
		Error, "Can't get weapon definition for WeaponId='%s'",
		WeaponId | EFWeaponId::ToName() | EFName::ToCStr());

	return true;
}

FPrimaryAssetId UWeaponDefinitionsAsset::GetPrimaryAssetId() const { return PrimaryAssetId; }

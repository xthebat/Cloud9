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
#include "FirearmNames.h"
#include "GrenadeNames.h"
#include "MeleeNames.h"
#include "UnknownNames.h"
#include "Cloud9/Tools/Extensions/UEnum.h"
#include "Cloud9/Tools/Macro/Operator.h"

class ACloud9WeaponGrenade;
class ACloud9WeaponMelee;
class ACloud9WeaponFirearm;

struct FGrenadeWeaponInfo;
struct FMeleeWeaponInfo;
struct FFirearmWeaponInfo;

using FWeaponId = TVariant<EBadWeapon, EMelee, EFirearm, EGrenade>;

namespace EFWeaponId
{
	struct ToName
	{
	public:
		FORCEINLINE FName operator()(const FWeaponId& Self) const
		{
			return Visit([this](let It) { return It | EUEnum::GetValueName(); }, Self);
		}

		OPERATOR_BODY(ToName)
	};

	/* WeaponInfo */

	template <typename WeaponIdType>
	struct WeaponInfo;

	template <>
	struct WeaponInfo<EFirearm>
	{
		using Type = FFirearmWeaponInfo;
	};

	template <>
	struct WeaponInfo<EMelee>
	{
		using Type = FMeleeWeaponInfo;
	};

	template <>
	struct WeaponInfo<EGrenade>
	{
		using Type = FGrenadeWeaponInfo;
	};

	/* WeaponClass */

	template <typename WeaponIdType>
	struct WeaponClass;

	template <>
	struct WeaponClass<EFirearm>
	{
		using Type = ACloud9WeaponFirearm;
	};

	template <>
	struct WeaponClass<EMelee>
	{
		using Type = ACloud9WeaponMelee;
	};

	template <>
	struct WeaponClass<EGrenade>
	{
		using Type = ACloud9WeaponGrenade;
	};
}

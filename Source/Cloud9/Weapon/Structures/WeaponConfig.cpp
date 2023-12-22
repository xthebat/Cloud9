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

#include "WeaponConfig.h"

#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Tools/Extensions/WhenOrNone.h"
#include "Cloud9/Weapon/Extensions/EWeaponId.h"

EWeaponSlot FWeaponConfig::GetWeaponSlot() const
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

FWeaponId FWeaponConfig::GetWeaponId() const
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

FName FWeaponConfig::GetWeaponName() const { return GetWeaponId() | EFWeaponId::ToName(); }

TSubclassOf<AActor> FWeaponConfig::GetWeaponStaticClass() const
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

bool FWeaponConfig::Initialize(AActor* Actor) const
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

ACloud9WeaponBase* FWeaponConfig::SpawnWeapon(UWorld* World) const
{
	switch (WeaponClass)
	{
	case EWeaponClass::Melee:
		return SpawnWeapon<ACloud9WeaponMelee>(World);
	case EWeaponClass::Firearm:
		return SpawnWeapon<ACloud9WeaponFirearm>(World);
	case EWeaponClass::Grenade:
		return SpawnWeapon<ACloud9WeaponGrenade>(World);
	default:
		return nullptr;
	}
}

FString FWeaponConfig::ToString() const
{
	return FString::Printf(
		TEXT("WeaponConfig { Class='%s' Name='%s' Skin='%s' }"),
		GetClassName() | EFName::ToCStr(),
		GetWeaponName() | EFName::ToCStr(),
		GetSkinName() | EFName::ToCStr()
	);
}

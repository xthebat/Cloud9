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

#include "WeaponConfigUtils.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponMelee.h"

EWeaponSlot UWeaponConfigUtils::GetWeaponSlot(const FWeaponConfig& Config)
{
	switch (Config.WeaponClass)
	{
	case EWeaponClass::Melee:
		return EWeaponSlot::Knife;
	case EWeaponClass::Firearm:
		return Config.bIsPrimary ? EWeaponSlot::Main : EWeaponSlot::Pistol;
	case EWeaponClass::Grenade:
		return EWeaponSlot::Grenade;
	default:
		return EWeaponSlot::NotSelected;
	}
}

FName UWeaponConfigUtils::GetClassName(const FWeaponConfig& Config)
{
	return Config.WeaponClass | EUEnum::GetValueName();
}

FName UWeaponConfigUtils::GetWeaponName(const FWeaponConfig& Config)
{
	switch (Config.WeaponClass)
	{
	case EWeaponClass::Melee:
		return Config.MeleeWeaponName | EUEnum::GetValueName();
	case EWeaponClass::Firearm:
		return Config.FirearmWeaponName | EUEnum::GetValueName();
	case EWeaponClass::Grenade:
		return Config.GrenadeWeaponName | EUEnum::GetValueName();
	default:
		return NAME_None;
	}
}

FName UWeaponConfigUtils::GetSkinName(const FWeaponConfig& Config) { return Config.SkinName; }

ACloud9WeaponBase* UWeaponConfigUtils::Spawn(const FWeaponConfig& Config, UWorld* World)
{
	switch (Config.WeaponClass)
	{
	case EWeaponClass::Melee:
		return SpawnIntern<ACloud9WeaponMelee>(World, Config.MeleeWeaponName, Config.SkinName);
	case EWeaponClass::Firearm:
		return SpawnIntern<ACloud9WeaponFirearm>(World, Config.FirearmWeaponName, Config.SkinName);
	case EWeaponClass::Grenade:
		return SpawnIntern<ACloud9WeaponGrenade>(World, Config.GrenadeWeaponName, Config.SkinName);
	default:
		return nullptr;
	}
}

TSubclassOf<ACloud9WeaponBase> UWeaponConfigUtils::GetWeaponStaticClass(const FWeaponConfig& Config)
{
	switch (Config.WeaponClass)
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

FString UWeaponConfigUtils::ToString(const FWeaponConfig& Config)
{
	return FString::Printf(
		TEXT("WeaponConfig { Class='%s' Name='%s' Skin='%s' }"),
		GetClassName(Config) | EFName::ToCStr(),
		GetWeaponName(Config) | EFName::ToCStr(),
		GetSkinName(Config) | EFName::ToCStr()
	);
}

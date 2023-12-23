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

#include "WeaponTableBase.h"

#include "WeaponTableMelee.generated.h"

class ACloud9WeaponMelee;

USTRUCT(BlueprintType)
struct FMeleeWeaponSounds
{
	GENERATED_BODY()

	/**
	 * Sounds to play when melee primary attack executed with hit a character
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> HitSounds;

	/**
	 * Sounds to play when melee secondary attack executed with hit a character 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> StabSounds;

	/**
	 * Sounds to play when melee attack executed without hit 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> SlashSounds;

	/**
	 * Sounds to play when melee attack executed with hit a thing
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> HitWallSounds;

	/**
	 * Sound to play when weapon being deployed
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* DeploySound = nullptr;
};

/**
 * Melee weapon description structure.
 */
USTRUCT(BlueprintType)
struct FMeleeDamageInfo
{
	GENERATED_BODY()

	/**
	 * Amount of damage inflicted per first attack
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float Base;

	/**
	 * Amount of damage inflicted per attack from behind
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float FromBehind;

	/**
     * Amount of damage inflicted per consequent attack (0 = Base)
     */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage, AdvancedDisplay,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float Consequent;
};

/**
 * Melee weapon description structure.
 */
USTRUCT(BlueprintType)
struct FMeleeWeaponInfo : public FBaseWeaponInfo
{
	GENERATED_BODY()

	/**
	 * Actual weapon class (if overloaded)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	TSubclassOf<ACloud9WeaponMelee> Class;

	/**
	 * Amount of damage inflicted by slash on unarmored character
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	FMeleeDamageInfo UnarmoredSlashDamage;

	/**
	 * Amount of damage inflicted by slash on armored character
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	FMeleeDamageInfo ArmoredSlashDamage;

	/**
	 * Amount of damage inflicted by stab on unarmored character
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	FMeleeDamageInfo UnarmoredStabDamage;

	/**
	 * Amount of damage inflicted by stab on armored character
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	FMeleeDamageInfo ArmoredStabDamage;

	/**
	 * Minimum interval between slashes (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float SlashCycleTime;

	/**
	 * Minimum interval between stabs (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float StabCycleTime;

	/**
	 * Time to deploy weapon i.e. from start deploying to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float DeployTime;

	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="500", ClampMin="0", ClampMax="500"))
	float MaxPlayerSpeed;

	/**
	 * Weapon sound on different actions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	FMeleeWeaponSounds Sounds;

	/**
	 * Base mesh corpus to show
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* WeaponModel = nullptr;

	/**
	 * Available skins for weapon
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	TArray<FWeaponSkin> Skins;

	/**
	 * Money award for kill enemy with this gun
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Economy,
		meta=(UIMin="0", UIMax="20000", ClampMin="0", ClampMax="20000"))
	int KillAward;
};

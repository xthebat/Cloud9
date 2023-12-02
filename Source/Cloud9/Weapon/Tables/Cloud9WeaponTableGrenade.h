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

#include "NiagaraComponent.h"
#include "Cloud9WeaponTableBase.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"
#include "Cloud9WeaponTableGrenade.generated.h"


USTRUCT(BlueprintType)
struct FGrenadeWeaponSounds
{
	GENERATED_BODY()

	/**
	 * Sounds to play when grenade activates (i.e. explode, start burn, etc)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> ExplodeSounds;

	/**
	 * Sounds to play when grenade hit something
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> HitSounds;

	/**
	 * Sound to play when weapon deployed
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* DeploySound = nullptr;

	/**
	 * Sound to play when a grenade pin pulled off
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* PinpullSound = nullptr;

	/**
	 * Sound to play when weapon zoom enabled
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* ThrowSound = nullptr;

	/**
	 * Sounds to play when grenade is active (molotov burning or decoy shooting)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* LoopSound = nullptr;
};

USTRUCT(BlueprintType)
struct FGrenadeWeaponEffects
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	UNiagaraSystem* Explode = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	UNiagaraSystem* Action = nullptr;
};

USTRUCT(BlueprintType)
struct FGrenadeWeaponSkin : public FBaseWeaponSkin
{
	GENERATED_BODY()

	/**
	 * Base mesh corpus to show
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* Weapon = nullptr;
};

/**
 * Grenade weapon description structure.
 */
USTRUCT(BlueprintType)
struct FGrenadeWeaponInfo : public FBaseWeaponInfo
{
	GENERATED_BODY()

	/**
	 * Actual weapon class (if overloaded)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	TSubclassOf<ACloud9WeaponGrenade> Class = ACloud9WeaponGrenade::StaticClass();

	/**
	 * --
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float Damage;

	/**
	 * Damage against armored opponents is multiplied by ArmorPenetration * 0.5
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="2.0"))
	float ArmorPenetration = 2.0f;

	/**
	 * --
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float RangeModifier = 1.0f;

	/**
	 * Maximum fire weapon range (no any impact after this distance)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Distance,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float ThrowDistance = 100.0f;

	/**
	 * Distance multiplier when making run throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Distance,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float RunDistanceMultiplier = 1.0f;

	/**
	 * Distance multiplier when making jump throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Distance,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float JumpDistanceMultiplier = 1.0f;

	/**
	 * Time to pull out pin of grenade before throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float PinpullTime;

	/**
	 * Time to deploy weapon i.e. from start deploying to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float DeployTime;

	/**
	 * How much time grenade is in active state
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ActionTime;

	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="500", ClampMin="0", ClampMax="500"))
	float MaxPlayerSpeed;

	/**
	 * Maximum grenades of this type in inventory 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Ammo,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	int MaxCount; // TODO: Rename

	/**
	 * Weapon sound on different actions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	FGrenadeWeaponEffects Effects;

	/**
	 * Weapon sound on different actions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	FGrenadeWeaponSounds Sounds;

	/**
	 * Available skins for weapon and it magazines
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	TArray<FGrenadeWeaponSkin> Skins;

	/**
	 * Weapon price in buy menu
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Economy,
		meta=(UIMin="0", UIMax="20000", ClampMin="0", ClampMax="20000"))
	int Price;

	/**
	 * Money award for kill enemy with this gun
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Economy,
		meta=(UIMin="0", UIMax="20000", ClampMin="0", ClampMax="20000"))
	int KillAward;

	/**
	 * The factor a target is slowed to (the lower the more effective)
	 *
	 * NOTE: For future use
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Flinch, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float FlinchVelocityModifierFirst = 1.0f;

	/**
	 * The factor a target is slowed to for second and consequent shoots
	 *
	 * NOTE: For future use
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Flinch, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float FlinchVelocityModifierNext = 1.0f;
};

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

#include "WeaponTableBase.h"

#include "WeaponTableGrenade.generated.h"

class ACloud9WeaponGrenade;

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
	UNiagaraSystem* OnDetonationEffect = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	FVector OnDetonationScale = FVector::OneVector;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	UNiagaraSystem* OnActiveEffect = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	FVector OnActiveScale = FVector::OneVector;
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
	TSubclassOf<ACloud9WeaponGrenade> Class;

	/**
	 * Only relevant for frag (explosive) and molotov grenade
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float Damage;

	/**
	 * Effective radius for grenade
	 * - Explosive - Explosive radius
	 * - Molotov - Radius of burning
	 * - Smoke - Radius of smoke
	 * - Flash - Not relevant
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	float Radius = 200.0f;

	/**
	 * Damage against armored opponents is multiplied by ArmorPenetration * 0.5
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="2.0"))
	float ArmorPenetration = 2.0f;

	/**
	 * Time to pull out pin of grenade before throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float PinpullTime = 1.0f;

	/**
	 * Time to deploy weapon i.e. from start deploying to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float DeployTime = 1.0f;

	/**
	 * Time to throw grenade (in CS:GO action can be performed instantly after throw)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ThrowTime = 0.05f;

	/**
	 * How much time required grenade to detonate (if zero detonate as soon as stop bounce if bCanDetonateInAir=false) 
	 *
	 * NOTE:
	 *	- Molotov - detonate when "bounce" on near horizontal plane (i.e. floor)
	 *	- Smoke - detonate instantly as soon as stops bounce
	 *	- Flash - 1.775 sec.
	 *	- HE Grenade - 1.775 sec.
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float TimeToDetonate = 1.0f;

	/**
	 * How much time grenade is in active state
	 *
	 * NOTE:
	 *	- Molotov - time to burn
	 *	- Smoke - how much time are smoked
	 *	- Flash - how much time character blind
	 *	- HE Grenade - no meaning
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="50", ClampMin="0", ClampMax="50"))
	float ActionTime = 0.0f;

	/**
	 * Whether or not grenade can bounce from wall or detonate when touch it
	 *
	 * NOTE: Not quit valid because molotov can bounce from vertical walls
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Bounce)
	bool bCanBounce = true;

	/**
	 * Whether or not grenade can detonate in air or not (smoke and decoy grenades can't detonate in air)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Bounce)
	bool bCanDetonateInAir = true;

	/**
	 * Whether or not grenade can destroyed (make invisible) when detonated (not for smoke)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Bounce)
	bool bIsDestroyedOnDetonation = true;

	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 *
	 * NOTE: All grenades made MaxPlayerSpeed to 245.0f units
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="250", ClampMin="0", ClampMax="250"))
	float MaxPlayerSpeed = 245.0f;

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
	 * Base mesh corpus to show
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* WeaponModel = nullptr;

	/**
	 * Available skins for weapon and it magazines
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	TArray<FWeaponSkin> Skins;

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

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

#include "WeaponTableFirearm.generated.h"

class ACloud9WeaponFirearm;

USTRUCT(BlueprintType)
struct FFirearmWeaponSounds
{
	GENERATED_BODY()

	/**
	 * Sounds to play when weapon firing (random one will be selected to play)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> FireSounds;

	/**
	 * Sequence of sounds to play when reload weapon
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TArray<USoundBase*> ReloadSounds;

	/**
	 * Sound to play when weapon deployed
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* DeploySound = nullptr;

	/**
	 * Sound to play when weapon zoom enabled
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* ZoomInSound = nullptr;

	/**
	 * Sound to play when weapon zoom disabled (if nullptr then ZoomInSound will be used)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* ZoomOutSound = nullptr;
};

USTRUCT(BlueprintType)
struct FFirearmWeaponEffects
{
	GENERATED_BODY()

	/**
	 * Muzzle flash effect to play on weapon fire
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	UNiagaraSystem* MuzzleFlash = nullptr;

	/**
	 * Muzzle smoke effect after weapon shot
	 *
	 * NOTE: For future use
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Effects)
	UNiagaraSystem* MuzzleSmoke = nullptr;
};

USTRUCT(BlueprintType)
struct FFirearmInaccuracy
{
	GENERATED_BODY()

	/**
	 * Additional inaccuracy when crouch
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnCrouch;

	/**
	 * Additional inaccuracy when stand still
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnStand;

	/**
	 * Additional inaccuracy after firing
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnFire;

	/**
	 * Additional inaccuracy whilst moving at MaxPlayerSpeed
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnMove;

	/**
	 * Additional inaccuracy whilst jump
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnJump;

	/**
	 * Additional inaccuracy whilst character on ladder
	 *
	 * NOTE: For future use
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float OnLadder;
};

USTRUCT(BlueprintType)
struct FRecoveryTime
{
	GENERATED_BODY()

	/**
	 * When crouching it is the decay rate for InaccuracyFire, InaccuracyJump, InaccuracyLadder
	 * the difference between InaccuracyStand and InaccuracyCrouch and
	 * the difference between InaccuracyCrouch and InaccuracyCrouchAlt
	 * using the following formula: 'Inaccuracy * (0.1 ^ (time/RecoveryTime))'
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float OnCrouch;

	/**
	 * When standing it is the decay rate for InaccuracyFire, InaccuracyJump, InaccuracyLadder, and
	 * the difference between InaccuracyStand and InaccuracyStandAlt using
	 * the following formula: 'Inaccuracy * (0.1 ^ (time/RecoveryTime))'
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float OnStand;
};

/**
 * Firearm weapon description structure.
 *
 * NOTE: All distances specified in CSGO Units (when import distance 
 */
USTRUCT(BlueprintType)
struct FFirearmWeaponInfo : public FBaseWeaponInfo
{
	GENERATED_BODY()

	/**
	 * Actual weapon class (if overloaded)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	TSubclassOf<ACloud9WeaponFirearm> Class;

	/**
	 * Amount of damage inflicted per bullet before any modifiers
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float Damage;

	/**
	 * Damage against armored opponents is multiplied by ArmorPenetration * 0.5
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="2.0", ClampMin="0", ClampMax="2.0"))
	float ArmorPenetration = 2.0f;

	/**
	 * Maximum number of character or items to penetrate
	 *
	 * NOTE: For future use
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float PenetrationPower = 1.0f;

	/**
	 * Damage is multiplied by 'RangeModifier ^ (Distance(u)/500u)'
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float RangeModifier = 1.0f;

	/**
	 * The amount damage is multiplied for headshots
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float HeadshotMultiplier = 1.0f;

	/**
	 * Maximum fire weapon range (no any impact after this distance)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage, AdvancedDisplay,
		meta=(UIMin="0", UIMax="10000", ClampMin="0", ClampMax="10000"))
	float MaxBulletRange = 8192.0f;

	/**
	 * Minimum interval between firing next bullet (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float CycleTime;

	/**
	 * Time to deploy weapon i.e. from start deploying to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float DeployTime;

	/**
	 * Time to reload weapon i.e. from start reloading to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ReloadTime;

	/**
	 * Minimum interval between zoom enable and shoot (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time, AdvancedDisplay,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ZoomTime;

	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="500", ClampMin="0", ClampMax="500"))
	float MaxPlayerSpeed;

	/**
	 * Number of rounds (or shots) per weapon magazine
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Ammo,
		meta=(UIMin="0", UIMax="300", ClampMin="0", ClampMax="300"))
	int MagazineSize;

	/**
	 * Total number of rounds available to carry for this weapon type
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Ammo,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	int MaxAmmoInReserve;

	/**
	 * Weapon fires automatically whilst primary action is toggled
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mode)
	bool bIsFullAuto = false; // aka HoldToShoot

	/**
	 * The frequency at which tracers are applied to bullets (0 = never)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX,
		meta=(UIMin="0", UIMax="5", ClampMin="0", ClampMax="5"))
	int TracerFrequency = 1;

	/**
	 * Weapon visual effects on different actions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	FFirearmWeaponEffects Effects;

	/**
	 * Weapon sound on different actions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	FFirearmWeaponSounds Sounds;

	/**
	 * Base mesh corpus to show
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* WeaponModel = nullptr;

	/**
	 * Mesh for dropout magazine on reload
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* MagazineModel = nullptr;

	/**
	 * Mesh for silencer (if weapon has one)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* SilencerModel = nullptr;

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

	// TODO: Implement spread
	// float fDecayFactor = logf( 10.0f ) / GetRecoveryTime( );
	// m_fAccuracyPenalty = Lerp( expf( TICK_INTERVAL * -fDecayFactor ), fNewPenalty, ( float ) m_fAccuracyPenalty );

	/**
	 * Basic weapon bullet shooting spread (inaccuracy)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float Spread;

	/**
	 * Weapon bullet shooting spread inaccuracy at different conditions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy)
	FFirearmInaccuracy Inaccuracy;

	/**
	 * Inaccuracy recovery time at different conditions
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Accuracy)
	FRecoveryTime RecoveryTime;

	/**
	 * The number of bullets (pellets for a shotgun) fired from a cartridge
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Additional, AdvancedDisplay,
		meta=(EditCondition="Type == EWeaponType::Shotgun", UIMin="1", UIMax="10", ClampMin="0", ClampMax="10"))
	int BulletsPerShot = 1;

	/**
	 * The field of view while in the 1st zoom level (default unscoped is 90)
	 *
	 * NOTE: Currently is uncleared how zoom will work
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Zoom, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="90.0", ClampMin="0", ClampMax="90.0"))
	float ZoomFieldOfView1;

	/**
	 * The field of view while in the 2nd zoom level (default unscoped is 90)
	 *
	 * NOTE: Currently is uncleared how zoom will work
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Zoom, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="90.0", ClampMin="0", ClampMax="90.0"))
	float ZoomFieldOfView2;

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

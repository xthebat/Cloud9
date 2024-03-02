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
	TMap<FName, USoundBase*> ReloadSounds;

	/**
	 * Sound to play when weapon deployed
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	USoundBase* DeploySound = nullptr;

	/**
	 * Sound to play when weapon zoom enabled or disabled
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Sounds)
	TMap<FName, USoundBase*> ZoomSounds;
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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	bool bCanZoom = false;

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
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float PenetrationPower = 1.0f;

	/**
	 * Damage is multiplied by 'RangeModifier ^ (Distance(u)/1000u)'
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float RangeModifier = 1.0f;

	/**
	 * The amount damage is multiplied for headshots
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float HeadshotMultiplier = 4.0f;

	/**
	 * The amount damage is multiplied for body and arms shots
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float UpperBodyMultiplier = 1.0f;

	/**
	 * The amount damage is multiplied for shot in pelvis part of body
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float LowerBodyMultiplier = 1.25f;

	/**
	 * The amount damage is multiplied for shot in any part of legs
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float LegMultiplier = 0.75f;

	/**
	 * Maximum fire weapon range (no any impact after this distance)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage,
		meta=(UIMin="0", UIMax="10000", ClampMin="0", ClampMax="10000"))
	float MaxBulletRange = 8192.0f;

	/**
	 * Minimum interval between firing next bullet (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float CycleTime = 0.1f;

	/**
	 * Time to deploy weapon i.e. from start deploying to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float DeployTime = 1.0f;

	/**
	 * Time to reload weapon i.e. from start reloading to available for use (shoot)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ReloadTime = 1.0f;

	/**
	 * Time to reload one shell of the weapon
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(EditCondition="Type == EWeaponType::Shotgun", EditConditionHides,
			UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ReloadLoopTime = 1.0f;

	/**
	 * Time to stop reload the weapon
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(EditCondition="Type == EWeaponType::Shotgun", EditConditionHides,
			UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ReloadEndTime = 1.0f;

	/**
	 * Minimum interval between zoom enable and shoot (measured in seconds)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Time,
		meta=(EditCondition="bCanZoom", EditConditionHides,
			UIMin="0", UIMax="10", ClampMin="0", ClampMax="10"))
	float ZoomTime = 0.1f;

	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="250", ClampMin="0", ClampMax="250"))
	float MaxPlayerSpeed = 250.0f;

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
	 * Ammo when start play low ammo count sound
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Ammo,
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	int LowAmmoCount;

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
	 * Weapon fires automatically whilst primary action is toggled
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mode)
	bool bIsFullAuto = false;

	/**
	 * If true inverse kinematic will be disabled during shoot
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mode, AdvancedDisplay)
	bool bIsManualBoltCycle = false;

	/**
	 * Probability of tracer for weapon
	 * 
	 * NOTE: Originally used TracerFrequency - the frequency at which tracers are applied to bullets (0 = never)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX,
		meta=(UIMin="0", UIMax="1", ClampMin="0", ClampMax="1"))
	float TracerProbability = 0.3f;

	/**
	 * The number of bullets (pellets for a shotgun) fired from a cartridge
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Additional, AdvancedDisplay,
		meta=(EditCondition="Type == EWeaponType::Shotgun", EditConditionHides,
			UIMin="1", UIMax="10", ClampMin="0", ClampMax="10"))
	int BulletsPerShot = 1;

	/**
	 * The field of view while in the 1st zoom level (default unscoped is 90)
	 *
	 * FoV <= 0.0f - mean no zoom
	 * 
	 * NOTE: Currently is uncleared how zoom will work
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Zoom,
		meta=(EditCondition="bCanZoom", EditConditionHides,
			UIMin="0.0", UIMax="90.0", ClampMin="0", ClampMax="90.0"))
	float ZoomFieldOfView1 = 0.0f;

	/**
	 * The field of view while in the 2nd zoom level (default unscoped is 90)
	 *
	 * FoV <= 0.0f - mean no zoom
	 *
	 * NOTE: Currently is uncleared how zoom will work
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Zoom,
		meta=(EditCondition="bCanZoom", EditConditionHides,
			UIMin="0.0", UIMax="90.0", ClampMin="0", ClampMax="90.0"))
	float ZoomFieldOfView2 = 0.0f;

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
	 * Model for eject case after shoot 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UStaticMesh* CaseModel = nullptr;

	/**
	 * Whether or not magazine also has skin (required for shotguns) 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	bool bIsMagazinePainted = true;

	/**
	 * Available skins for weapon and it magazines
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=VFX)
	TArray<FWeaponSkin> Skins;

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
	 * The factor a target is slowed to (the lower the more effective)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Flinch, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float FlinchVelocityModifierFirst = 1.0f;

	/**
	 * The factor a target is slowed to for second and consequent shoots
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Flinch, AdvancedDisplay,
		meta=(UIMin="0.0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float FlinchVelocityModifierNext = 1.0f;
};

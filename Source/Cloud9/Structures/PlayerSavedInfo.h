// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "Cloud9/Character/Structures/HealthConfig.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"

#include "PlayerSavedInfo.generated.h"

class UCloud9CharacterEffectTrait;

/**
 * Player's saved info to transfer between maps and initialize players.
 * Transferring each of the properties depends on actual GameMode.  
 */
USTRUCT(BlueprintType)
struct FPlayerSavedInfo
{
	GENERATED_BODY()

	/**
	 * Initial player's health
	 */
	UPROPERTY(Category=Health, EditDefaultsOnly, BlueprintReadOnly)
	FHealthConfig HealthConfig;

	/**
	 * Initial player's weapon starting slot
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, BlueprintReadOnly)
	EWeaponSlot WeaponSlot = EWeaponSlot::NotSelected;

	/**
	 * Initial player's weapons in inventory
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWeaponConfig> WeaponConfigs;

	/**
	 * Initial player's applied effects (e.g., shield)
	 */
	UPROPERTY(Category=Effects, EditDefaultsOnly, BlueprintReadOnly)
	TSet<TSubclassOf<UCloud9CharacterEffectTrait>> Effects;

	void Reset();
};

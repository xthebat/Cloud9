// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "Cloud9/Character/Structures/HealthConfig.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"

#include "PlayerSavedInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSavedInfo
{
	GENERATED_BODY()

	UPROPERTY(Category=Health, EditDefaultsOnly, BlueprintReadOnly)
	FHealthConfig HealthConfig;

	UPROPERTY(Category=Weapon, EditDefaultsOnly, BlueprintReadOnly)
	EWeaponSlot WeaponSlot = EWeaponSlot::NotSelected;

	UPROPERTY(Category=Weapon, EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWeaponConfig> WeaponConfigs;

	void Reset();
};

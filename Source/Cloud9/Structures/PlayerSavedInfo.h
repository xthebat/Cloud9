// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "Cloud9/Weapon/Structures/WeaponConfig.h"

#include "PlayerSavedInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSavedInfo
{
	GENERATED_BODY()

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	TArray<FWeaponConfig> WeaponConfigs;

	void Reset();
};

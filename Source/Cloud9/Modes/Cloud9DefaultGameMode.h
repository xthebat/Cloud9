// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameMode.h"
#include "Cloud9DefaultGameMode.generated.h"

UCLASS()
class CLOUD9_API ACloud9DefaultGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

	ACloud9DefaultGameMode();

protected:
	virtual bool OnWorldLoadComplete(FPlayerSavedInfo& SavedInfo) override;

	virtual bool OnWorldTearDown(FPlayerSavedInfo& SavedInfo) override;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	TArray<FWeaponConfig> InitialWeaponsConfig;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	EWeaponSlot InitialWeaponSlot;
};

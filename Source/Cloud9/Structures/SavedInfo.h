// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "PlayerSavedInfo.h"

#include "SavedInfo.generated.h"

USTRUCT(BlueprintType)
struct FSavedInfo
{
	GENERATED_BODY()

	// TODO: Add multiplayer save-load game (!!!)

	// UPROPERTY(Category=Weapon, BlueprintReadOnly)
	// TMap<FName, FPlayerSavedInfo> Players;

	UPROPERTY(Category=Player, BlueprintReadOnly)
	FPlayerSavedInfo Player;

	UPROPERTY(Category=Player, BlueprintReadOnly)
	bool IsPlayerStored = false;

	void Reset();
};

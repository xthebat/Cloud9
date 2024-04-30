// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "PlayerSavedInfo.h"

#include "SavedInfo.generated.h"

USTRUCT(BlueprintType)
struct FSavedInfo
{
	GENERATED_BODY()

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	TMap<int32, FPlayerSavedInfo> Players;

	void Reset();
};

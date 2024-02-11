// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "PlayerSavedInfo.h"

#include "SavedInfo.generated.h"

USTRUCT(BlueprintType)
struct FSavedInfo
{
	GENERATED_BODY()

	UPROPERTY(Category=Base, BlueprintReadOnly)
	bool bIsLoadRequired;

	UPROPERTY(Category=Weapon, BlueprintReadOnly)
	TMap<int, FPlayerSavedInfo> PlayersSavedInfo;

	void Reset();
};

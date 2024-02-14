// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "HealthConfig.generated.h"

USTRUCT(BlueprintType)
struct FHealthConfig
{
	GENERATED_BODY()

	UPROPERTY(Category=Config, EditDefaultsOnly, BlueprintReadOnly,
		meta=(UIMin="0", UIMax="20000.0", ClampMin="0", ClampMax="20000.0"))
	float Health = 100.0f;

	UPROPERTY(Category=Config, EditDefaultsOnly, BlueprintReadOnly,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0"))
	float Armor = 0.0f;

	UPROPERTY(Category=Config, EditDefaultsOnly, BlueprintReadOnly)
	bool bHasHelmet = false;
};

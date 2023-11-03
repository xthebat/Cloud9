// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Cloud9GameMode.generated.h"

class UCloud9DeveloperSettings;
class ACloud9Character;

UCLASS(minimalapi)
class ACloud9GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACloud9GameMode();

	UFUNCTION(BlueprintCallable)
	ACloud9Character* GetCharacter() const;

public:
	virtual void Tick(float DeltaSeconds) override;
};

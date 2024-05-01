// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameMode.h"
#include "Cloud9DefaultGameMode.generated.h"

UCLASS()
class CLOUD9_API ACloud9DefaultGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

public:
	static FName PlayerConfigName;
	static FName BotConfigName;

	ACloud9DefaultGameMode();

protected:
	virtual void SaveCharacter(const ACloud9Character* Character) override;

	virtual void LoadCharacter(ACloud9Character* Character) override;

	UPROPERTY(Category=Config, EditDefaultsOnly)
	TMap<FName, FPlayerSavedInfo> InitialPlayerConfig;

private:
	void InitializeCharacter(ACloud9Character* Character);
};

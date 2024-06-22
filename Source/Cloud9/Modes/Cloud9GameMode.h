// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Cloud9/Structures/SavedInfo.h"

#include "Cloud9GameMode.generated.h"

class ACloud9GameHud;
class UCloud9GameInstance;

UCLASS()
class ACloud9GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	static inline FName PlayerConfigName = TEXT("God");
	static inline FName BotConfigName = TEXT("Bot");

	ACloud9GameMode();

	virtual void SaveCharacter(const ACloud9Character* Character);

	virtual void LoadCharacter(ACloud9Character* Character);

protected:
	virtual void StartPlay() override final;

	virtual void StartToLeaveMap() override final;

	UCloud9GameInstance* GetCloud9GameInstance() const;

	/** Whether try to load pawns (character) by default implementation of LoadCharacter */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	bool IsTransferPawnsByDefault;

	/** Whether try to initialize pawns (character) by default implementation of LoadCharacter */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	bool IsInitializePawnsByDefault;

	UPROPERTY(EditDefaultsOnly, Category=Config, meta=(EditCondition="IsInitializePawnsByDefault", EditConditionHides))
	TMap<FName, FPlayerSavedInfo> InitialPlayerConfig;

private:
	void InitializeCharacter(ACloud9Character* Character);
};

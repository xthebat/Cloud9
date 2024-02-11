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
	static FName GodSidePlayer;
	static FName TSidePlayer;
	static FName CtSidePlayer;

public:
	ACloud9DefaultGameMode();

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual bool OnWorldChanged(FSavedInfo& SavedInfo) override;

	virtual bool OnWorldTearDown(FSavedInfo& SavedInfo) override;

	UPROPERTY(Category=Config, EditDefaultsOnly)
	TMap<FName, FPlayerSavedInfo> InitialPlayerConfig;
};

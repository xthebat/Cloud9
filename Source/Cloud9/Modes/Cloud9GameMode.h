// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Cloud9/Structures/SavedInfo.h"

#include "Cloud9GameMode.generated.h"

class UCloud9GameInstance;

UCLASS()
class ACloud9GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACloud9GameMode();

	virtual void SaveCharacter(const ACloud9Character* Character);

	virtual void LoadCharacter(ACloud9Character* Character);

protected:
	virtual void StartPlay() override final;

	virtual void StartToLeaveMap() override final;

	UCloud9GameInstance* GetCloud9GameInstance() const;
};

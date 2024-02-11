// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Cloud9/Structures/SavedInfo.h"

#include "Cloud9GameMode.generated.h"

UCLASS()
class ACloud9GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACloud9GameMode();

	virtual bool OnWorldLoadComplete(FPlayerSavedInfo& SavedInfo);

	virtual bool OnWorldTearDown(FPlayerSavedInfo& SavedInfo);
};

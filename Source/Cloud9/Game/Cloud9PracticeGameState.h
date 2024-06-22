// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameState.h"
#include "Cloud9PracticeGameState.generated.h"

UCLASS()
class CLOUD9_API ACloud9PracticeGameState : public ACloud9GameState
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	bool IsStarted;
	
	UPROPERTY()
	int32 Killed;
};

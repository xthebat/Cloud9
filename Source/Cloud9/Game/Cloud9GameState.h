// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Cloud9GameState.generated.h"

UCLASS()
class CLOUD9_API ACloud9GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACloud9GameState();

protected:
	virtual void Tick(float DeltaSeconds) override;
};

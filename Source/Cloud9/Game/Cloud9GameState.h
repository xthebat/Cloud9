// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9GameState.generated.h"

UCLASS()
class CLOUD9_API ACloud9GameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaSeconds) override;
};

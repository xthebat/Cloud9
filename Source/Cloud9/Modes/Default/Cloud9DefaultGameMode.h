// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Modes/Base/Cloud9GameMode.h"
#include "Cloud9DefaultGameMode.generated.h"

UCLASS(Blueprintable)
class CLOUD9_API ACloud9DefaultGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

public:
	ACloud9DefaultGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};

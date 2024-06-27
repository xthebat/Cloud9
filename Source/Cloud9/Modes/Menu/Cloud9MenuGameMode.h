// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Modes/Base/Cloud9GameMode.h"
#include "Cloud9MenuGameMode.generated.h"

UCLASS(Blueprintable)
class CLOUD9_API ACloud9MenuGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

public:
	ACloud9MenuGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};

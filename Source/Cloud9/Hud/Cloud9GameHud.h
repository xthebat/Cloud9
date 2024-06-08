// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Cloud9GameHud.generated.h"

UCLASS()
class CLOUD9_API ACloud9GameHud : public AHUD
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;
};

// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner.h"
#include "AmmoSpawner.generated.h"

UCLASS()
class CLOUD9_API AAmmoSpawner : public AItemSpawner
{
	GENERATED_BODY()

public:
	virtual AActor* CreateChildActor() override;

	virtual bool ActivateSpawner(ACloud9Character* Character) override;
};

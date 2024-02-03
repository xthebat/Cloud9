// Copyright (c) 2024 Alexei Gladkikh


#include "AmmoSpawner.h"

AActor* AAmmoSpawner::CreateChildActor()
{
	return Super::CreateChildActor();
}

bool AAmmoSpawner::ActivateSpawner(ACloud9Character* Character)
{
	return Super::ActivateSpawner(Character);
}

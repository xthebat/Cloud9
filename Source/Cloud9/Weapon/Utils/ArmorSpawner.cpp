// Copyright (c) 2024 Alexei Gladkikh

#include "ArmorSpawner.h"

#include "Engine/StaticMeshActor.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9CharacterHealthComponent.h"

AArmorSpawner::AArmorSpawner()
{
	SampleMesh = nullptr;
	ArmorIncrement = 0;
	bIsAddHelmet = false;
}

bool AArmorSpawner::ActivateSpawner(ACloud9Character* Character)
{
	if (IsValid(Character))
	{
		let Health = Character->GetHealthComponent();

		Health->ChangeArmor(ArmorIncrement);
		if (bIsAddHelmet)
		{
			Health->ChangeArmor(true);
		}
	}

	return false;
}

AActor* AArmorSpawner::CreateChildActor() { return InitializeStaticMeshSample(SampleMesh); }

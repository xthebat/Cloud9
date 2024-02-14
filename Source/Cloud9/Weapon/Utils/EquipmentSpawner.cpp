// Copyright (c) 2024 Alexei Gladkikh

#include "EquipmentSpawner.h"

#include "Engine/StaticMeshActor.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9CharacterHealthComponent.h"

AEquipmentSpawner::AEquipmentSpawner()
{
	SampleMesh = nullptr;

	bChangeHealth = false;
	bChangeArmor = false;

	Health = 0.0f;
	Armor = 0.0f;
	bHelmet = false;

	bHealthActivated = false;
	bArmorActivated = false;
	bHelmetActivated = false;
}

bool AEquipmentSpawner::ActivateSpawner(ACloud9Character* Character)
{
	if (IsValid(Character))
	{
		let HealthComponent = Character->GetHealthComponent();

		if (bChangeHealth and HealthComponent->TakeHealthDamage(-Health))
		{
			bHealthActivated = true;
		}

		if (bChangeArmor and HealthComponent->TakeArmorDamage(-Armor))
		{
			bArmorActivated = true;
		}

		if (bHelmet and HealthComponent->ChangeHasHelmet(true))
		{
			bHelmetActivated = true;
		}

		return true;
	}

	return false;
}

bool AEquipmentSpawner::CanBeDestroyed() const
{
	return bChangeHealth xor bHealthActivated and
		bChangeArmor xor bArmorActivated and
		bHelmetActivated xor bHelmet;
}

AActor* AEquipmentSpawner::CreateChildActor() { return InitializeStaticMeshSample(SampleMesh); }

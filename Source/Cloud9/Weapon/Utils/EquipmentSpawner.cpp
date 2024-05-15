// Copyright (c) 2024 Alexei Gladkikh

#include "EquipmentSpawner.h"

#include "Engine/StaticMeshActor.h"

#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"

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

		var Activated = false;

		if (bChangeHealth and HealthComponent->IncreaseHealth(Health))
		{
			bHealthActivated = true;
			Activated = true;
		}

		if (bChangeArmor and HealthComponent->IncreaseArmor(Armor))
		{
			bArmorActivated = true;
			Activated = true;
		}

		if (bHelmet and HealthComponent->ChangeHasHelmet(true))
		{
			bHelmetActivated = true;
			Activated = true;
		}

		return Activated;
	}

	return false;
}

bool AEquipmentSpawner::CanBeDestroyed() const
{
	return not(bChangeHealth xor bHealthActivated) and
		not(bChangeArmor xor bArmorActivated) and
		not(bHelmetActivated xor bHelmet);
}

#if WITH_EDITOR
void AEquipmentSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (not bChangeArmor)
	{
		OBJECT_WARN("Can't add helmet without kevlar vest");
		bHelmet = false;
	}
}
#endif

AActor* AEquipmentSpawner::CreateChildActor() { return InitializeStaticMeshSample(SampleMesh); }

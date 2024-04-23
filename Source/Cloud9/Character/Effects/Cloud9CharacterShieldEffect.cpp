// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9CharacterShieldEffect.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/TContainer.h"

const FName UCloud9CharacterShieldEffect::ShieldEnableName = TEXT("Shield Enabled");

UCloud9CharacterShieldEffect::UCloud9CharacterShieldEffect()
{
	EffectTime = -1.0f;
}

void UCloud9CharacterShieldEffect::BeginPlay()
{
	Super::BeginPlay();
}

bool UCloud9CharacterShieldEffect::ToggleEffect(const ACloud9Character* Character, bool IsEnabled) const
{
	if (let Mesh = Character->GetMesh(); IsValid(Mesh))
	{
		Mesh->GetMaterials()
			| ETContainer::Transform{[](let It) { return Cast<UMaterialInstanceDynamic>(It); }}
			| ETContainer::Filter{[](let It) { return IsValid(It); }}
			| ETContainer::ForEach{
				[IsEnabled](let It) { It->SetScalarParameterValue(ShieldEnableName, IsEnabled); }
			};
	}

	return true;
}

bool UCloud9CharacterShieldEffect::Apply_Implementation(ACloud9Character* Character)
{
	return ToggleEffect(Character, true);
}

bool UCloud9CharacterShieldEffect::Remove_Implementation(ACloud9Character* Character)
{
	return ToggleEffect(Character, false);
}

bool UCloud9CharacterShieldEffect::Elapsed_Implementation(ACloud9Character* Character, float DeltaSeconds)
{
	return false;
}

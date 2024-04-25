// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9CharacterShieldEffect.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9EffectsComponent.h"
#include "Cloud9/Tools/Extensions/TContainer.h"

const FName UCloud9CharacterShieldEffect::ShieldEnableName = TEXT("Shield Enabled");

void UCloud9CharacterShieldEffect::ToggleEffect(bool IsEnabled) const
{
	let Character = GetContainer()->GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Effect='%s'] Owner is invalid", *GetName());
	}

	if (let Mesh = Character->GetMesh(); IsValid(Mesh))
	{
		Mesh->GetMaterials()
			| ETContainer::Transform{[](let It) { return Cast<UMaterialInstanceDynamic>(It); }}
			| ETContainer::Filter{[](let It) { return IsValid(It); }}
			| ETContainer::ForEach{[IsEnabled](let It) { It->SetScalarParameterValue(ShieldEnableName, IsEnabled); }};
	}
}

UCloud9CharacterShieldEffect::UCloud9CharacterShieldEffect()
{
	EffectTime = 0.0f;
	ElapsedTime = 0.0f;
}

bool UCloud9CharacterShieldEffect::IsExtinguished_Implementation() const { return ElapsedTime >= EffectTime; }

void UCloud9CharacterShieldEffect::OnApply_Implementation() { ToggleEffect(true); }

void UCloud9CharacterShieldEffect::OnRemove_Implementation() { ToggleEffect(false); }

bool UCloud9CharacterShieldEffect::CanApply_Implementation() const
{
	// TODO: Check only one shield effect applied
	return true;
}

bool UCloud9CharacterShieldEffect::CanTick_Implementation() const { return true; }

void UCloud9CharacterShieldEffect::OnTick_Implementation(float DeltaSeconds) { ElapsedTime += DeltaSeconds; }

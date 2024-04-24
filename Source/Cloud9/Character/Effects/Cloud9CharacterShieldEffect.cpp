// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9CharacterShieldEffect.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9EffectsComponent.h"
#include "Cloud9/Tools/Extensions/TContainer.h"

const FName UCloud9CharacterShieldEffect::ShieldEnableName = TEXT("Shield Enabled");

bool UCloud9CharacterShieldEffect::ToggleEffect(const UCloud9EffectsComponent* Container, bool IsEnabled) const
{
	let Character = Container->GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Effect='%s'] Owner is invalid", *GetName());
		return false;
	}

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

UCloud9CharacterShieldEffect::UCloud9CharacterShieldEffect()
{
	EffectTime = 0.0f;
	ElapsedTime = 0.0f;
}

bool UCloud9CharacterShieldEffect::IsExtinguished_Implementation() const
{
	return ElapsedTime >= EffectTime;
}

bool UCloud9CharacterShieldEffect::OnApply_Implementation(UCloud9EffectsComponent* Container)
{
	return ToggleEffect(Container, true);
}

bool UCloud9CharacterShieldEffect::OnRemove_Implementation(UCloud9EffectsComponent* Container)
{
	return ToggleEffect(Container, false);
}

bool UCloud9CharacterShieldEffect::CanApply_Implementation(const UCloud9EffectsComponent* Container)
{
	return true;
}

bool UCloud9CharacterShieldEffect::CanTick_Implementation() { return true; }

bool UCloud9CharacterShieldEffect::CanDamaged_Implementation() { return false; }

void UCloud9CharacterShieldEffect::OnTick_Implementation(const UCloud9EffectsComponent* Container, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
}

void UCloud9CharacterShieldEffect::OnApplyDamage_Implementation(
	const UCloud9EffectsComponent* Container,
	float Damage) {}

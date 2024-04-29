// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9CharacterEffectTrait.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Components/Cloud9EffectsComponent.h"

bool UCloud9CharacterEffectTrait::IsExtinguished_Implementation() const { return true; }

void UCloud9CharacterEffectTrait::OnApply_Implementation() {}

void UCloud9CharacterEffectTrait::OnRemove_Implementation() {}

bool UCloud9CharacterEffectTrait::CanApply_Implementation() const { return true; }

bool UCloud9CharacterEffectTrait::CanTick_Implementation() const { return false; }

bool UCloud9CharacterEffectTrait::CanDamaged_Implementation() const { return false; }

void UCloud9CharacterEffectTrait::OnTick_Implementation(float DeltaSeconds) {}

void UCloud9CharacterEffectTrait::OnApplyDamage_Implementation(float Damage) {}

const UCloud9EffectsComponent* UCloud9CharacterEffectTrait::GetContainer() const
{
	let MyOuter = GetOuter();

	if (not IsValid(MyOuter))
	{
		log(Fatal, "Effect outer parent object wasn't specified");
		return nullptr;
	}

	let Container = Cast<UCloud9EffectsComponent>(MyOuter);
	if (not IsValid(Container))
	{
		log(Fatal, "Effect outer parent object is invalid");
		return nullptr;
	}

	return Container;
}

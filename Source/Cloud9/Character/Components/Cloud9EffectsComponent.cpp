// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9EffectsComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Components/Cloud9HealthComponent.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/TContainer.h"

UCloud9EffectsComponent::UCloud9EffectsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCloud9EffectsComponent::AddEffect(TSubclassOf<UCloud9CharacterEffectInterface> EffectClass)
{
	if (let Effect = NewObject<UObject>(this, EffectClass->StaticClass());
		ICloud9CharacterEffectInterface::Execute_CanApply(Effect, this))
	{
		ICloud9CharacterEffectInterface::Execute_OnApply(Effect, this);

		Effects.Add(Effect);

		if (ICloud9CharacterEffectInterface::Execute_CanTick(Effect))
		{
			CanTickEffects.Add(Effect);
			SetComponentTickEnabled(true);
		}
		else if (ICloud9CharacterEffectInterface::Execute_CanDamaged(Effect))
		{
			CanDamagedEffects.Add(Effect);
		}

		return true;
	}

	return false;
}

bool UCloud9EffectsComponent::RemoveEffect(UObject* Effect)
{
	if (Effects.Remove(Effect) == 0)
	{
		log(Warning, "[Actor='%s'] Effect '%s' not found", *GetName(), *Effect->GetName());
		return false;
	}

	CanDamagedEffects.Remove(Effect);
	CanTickEffects.Remove(Effect);

	SetComponentTickEnabled(CanTickEffects.Num() != 0);

	ICloud9CharacterEffectInterface::Execute_OnRemove(Effect, this);
	return true;
}

void UCloud9EffectsComponent::OnRegister()
{
	Super::OnRegister();

	let Character = GetOwner<ACloud9Character>();
	if (not IsValid(Character))
	{
		log(Error, "[Component='%s'] Owner is invalid", *GetName());
		return;
	}

	let HealthComponent = Character->GetHealthComponent();
	if (not IsValid(HealthComponent))
	{
		log(Error, "[Component='%s'] Owner HealthComponent is invalid", *GetName());
		return;
	}

	HealthComponent->OnHealthChange.AddDynamic(this, &UCloud9EffectsComponent::OnDamageApplied);
}

void UCloud9EffectsComponent::OnDamageApplied(float Damage)
{
	let Extinguished = CanDamagedEffects
		| ETContainer::Filter{
			[this, Damage](let It)
			{
				ICloud9CharacterEffectInterface::Execute_OnApplyDamage(It, this, Damage);
				return ICloud9CharacterEffectInterface::Execute_IsExtinguished(It);
			}
		} | ETContainer::ToArray{};

	// Cache effects to remove
	Extinguished | ETContainer::ForEach{[this](let It) { RemoveEffect(It); }};
}

void UCloud9EffectsComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	let Extinguished = CanTickEffects
		| ETContainer::Filter{
			[this, DeltaTime](let It)
			{
				ICloud9CharacterEffectInterface::Execute_OnTick(It, this, DeltaTime);
				return ICloud9CharacterEffectInterface::Execute_IsExtinguished(It);
			}
		} | ETContainer::ToArray{};

	// Cache effects to remove
	Extinguished | ETContainer::ForEach{[this](let It) { RemoveEffect(It); }};
}

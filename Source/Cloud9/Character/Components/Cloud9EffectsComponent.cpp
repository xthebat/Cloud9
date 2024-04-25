// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9EffectsComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Components/Cloud9HealthComponent.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Effects/Cloud9CharacterEffectTrait.h"
#include "Cloud9/Tools/Extensions/TContainer.h"

UCloud9EffectsComponent::UCloud9EffectsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UCloud9CharacterEffectTrait* UCloud9EffectsComponent::AddEffect(
	TSubclassOf<UCloud9CharacterEffectTrait> EffectClass)
{
	if (let Effect = NewObject<UCloud9CharacterEffectTrait>(this, EffectClass->StaticClass()); Effect->CanApply())
	{
		Effect->OnApply();

		Effects.Add(Effect);

		if (Effect->CanTick())
		{
			CanTickEffects.Add(Effect);
			SetComponentTickEnabled(true);
		}
		else if (Effect->CanDamaged())
		{
			CanDamagedEffects.Add(Effect);
		}

		return Effect;
	}

	return nullptr;
}

bool UCloud9EffectsComponent::RemoveEffect(UCloud9CharacterEffectTrait* Effect)
{
	if (Effects.Remove(Effect) == 0)
	{
		log(Warning, "[Actor='%s'] Effect '%s' not found", *GetName(), *Effect->GetName());
		return false;
	}

	CanDamagedEffects.Remove(Effect);
	CanTickEffects.Remove(Effect);

	SetComponentTickEnabled(CanTickEffects.Num() != 0);

	Effect->OnRemove();
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
			[Damage](let It)
			{
				It->OnApplyDamage(Damage);
				return It->IsExtinguished();
			}
		} | ETContainer::ToArray{};

	// Cache effects to remove preventing modification during iteration
	Extinguished | ETContainer::ForEach{[this](let It) { RemoveEffect(It); }};
}

void UCloud9EffectsComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	let Extinguished = CanTickEffects
		| ETContainer::Filter{
			[DeltaTime](let It)
			{
				It->OnTick(DeltaTime);
				return It->IsExtinguished();
			}
		} | ETContainer::ToArray{};

	// Cache effects to remove preventing modification during iteration
	Extinguished | ETContainer::ForEach{[this](let It) { RemoveEffect(It); }};
}

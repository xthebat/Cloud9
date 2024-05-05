﻿// Copyright (c) 2024 Alexei Gladkikh

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

void UCloud9EffectsComponent::BeginPlay()
{
	Super::BeginPlay();
	InitialEffects | ETContainer::ForEach{[this](let It) { AddEffect(It); }};
}

UCloud9CharacterEffectTrait* UCloud9EffectsComponent::AddEffect(
	TSubclassOf<UCloud9CharacterEffectTrait> EffectClass)
{
	if (IsValid(EffectClass))
	{
		if (let Effect = NewObject<UCloud9CharacterEffectTrait>(this, EffectClass); Effect->CanApply())
		{
			Effect->OnApply();

			AppliedEffects.Add(Effect);

			if (Effect->CanTick())
			{
				AppliedCanTickEffects.Add(Effect);
				SetComponentTickEnabled(true);
			}
			else if (Effect->CanDamaged())
			{
				AppliedCanDamagedEffects.Add(Effect);
			}

			log(Verbose, "[Component='%s'] Apply effect class='%s' effect='%s' (%p) on owner='%s'",
			    *GetName(), *EffectClass->GetName(), *Effect->GetName(), Effect, *GetOwnerName());

			return Effect;
		}
	}

	return nullptr;
}

bool UCloud9EffectsComponent::RemoveEffect(UCloud9CharacterEffectTrait* Effect)
{
	if (AppliedEffects.Remove(Effect) == 0)
	{
		log(Warning, "[Component='%s'] Effect '%s' not found", *GetName(), *Effect->GetName());
		return false;
	}

	AppliedCanDamagedEffects.Remove(Effect);
	AppliedCanTickEffects.Remove(Effect);

	SetComponentTickEnabled(AppliedCanTickEffects.Num() != 0);

	Effect->OnRemove();

	log(Verbose, "[Component='%s'] Remove effect='%s' (%p) on owner='%s'",
	    *GetName(), *Effect->GetName(), Effect, *GetOwnerName());
	return true;
}

void UCloud9EffectsComponent::RemoveAllEffects()
{
	TSet Copy(AppliedEffects);
	// Prevent modification during iteration
	Copy | ETContainer::ForEach{[this](let It) { RemoveEffect(It); }};
}

UCloud9HealthComponent* UCloud9EffectsComponent::GetHealthComponent() const
{
	let Character = GetOwner<ACloud9Character>();
	CheckIsValid(Character, Error, "Owner is invalid", nullptr);

	let HealthComponent = Character->GetHealthComponent();
	CheckIsValid(HealthComponent, Error, "Owner HealthComponent is invalid", nullptr);

	return HealthComponent;
}

void UCloud9EffectsComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	GetHealthComponent()->OnHealthChange.AddDynamic(this, &UCloud9EffectsComponent::OnDamageApplied);
}

void UCloud9EffectsComponent::OnDamageApplied(float Damage)
{
	let Extinguished = AppliedCanDamagedEffects
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

FString UCloud9EffectsComponent::GetOwnerName() const
{
	return GetOwner() ? GetOwner()->GetName() : FString{};
}

void UCloud9EffectsComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	let Extinguished = AppliedCanTickEffects
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

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9CharacterHealthComponent.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

UCloud9CharacterHealthComponent::UCloud9CharacterHealthComponent()
{
	Health = 100.0f;
	Armor = 0.0f;
	bHasHelmet = false;
}

void UCloud9CharacterHealthComponent::Initialize(FHealthConfig Config)
{
	ChangeHealth(Config.Health);
	ChangeArmor(Config.Armor);
	ChangeHasHelmet(Config.bHasHelmet);
}

bool UCloud9CharacterHealthComponent::TakeHealthDamage(float Change)
{
	return ChangeHealth(Health - Change);
}

bool UCloud9CharacterHealthComponent::TakeArmorDamage(float Change)
{
	return ChangeArmor(Armor - Change);
}

bool UCloud9CharacterHealthComponent::ChangeHealth(float NewHealth)
{
	if (let Value = FMath::Max(NewHealth, 0.0f); Value != Health)
	{
		Health = Value;
		OnHealthChange.Broadcast(Health);

		if (Health == 0.0f)
		{
			let Owner = GetOwner();

			if (not IsValid(Owner))
			{
				log(Fatal, "[Component = %s] Owner isn't valid", *GetName());
				return false;
			}

			Owner->Destroy();
		}

		return true;
	}

	return false;
}

bool UCloud9CharacterHealthComponent::ChangeArmor(float NewArmor)
{
	if (let Value = FMath::Clamp(NewArmor, 0.0f, 100.0f); Value != Armor)
	{
		Armor = Value;
		OnArmorChange.Broadcast(Armor);
		if (Armor == 0.0f)
		{
			ChangeHasHelmet(false);
		}
		return true;
	}

	return false;
}

void UCloud9CharacterHealthComponent::OnRegister()
{
	Super::OnRegister();

	let MyOwner = GetOwner();

	if (not IsValid(MyOwner))
	{
		log(Error, "HealthComponent isn't valid");
		return;
	}

	log(Display, "Register HealthComponent for '%s'", *MyOwner->GetName());
	// Register twice or delegate add twice (?)
	MyOwner->OnTakePointDamage.AddUniqueDynamic(this, &UCloud9CharacterHealthComponent::OnTakePointDamage);
	MyOwner->OnTakeRadialDamage.AddUniqueDynamic(this, &UCloud9CharacterHealthComponent::OnTakeRadialDamage);
}

bool UCloud9CharacterHealthComponent::ChangeHasHelmet(bool NewState)
{
	if (bHasHelmet != NewState)
	{
		bHasHelmet = NewState;
		OnHelmetChange.Broadcast(NewState);
		return true;
	}

	return false;
}

void UCloud9CharacterHealthComponent::OnTakePointDamage(
	AActor* DamagedActor,
	float Damage,
	AController* InstigatedBy,
	FVector HitLocation,
	UPrimitiveComponent* FHitComponent,
	FName BoneName,
	FVector ShotFromDirection,
	const UDamageType* DamageType,
	AActor* DamageCauser)
{
	TakeHealthDamage(Damage); // TODO: Add factor by armor
	TakeArmorDamage(0.0f); // TODO: Add armor damage calc

	if (DamagedActor->IsPendingKill())
	{
		log(Display, "DamageCauser=%s", *DamageCauser->GetOwner()->GetName());
		Cast<ACloud9Character>(DamageCauser->GetOwner())->AddScore();
	}
}

void UCloud9CharacterHealthComponent::OnTakeRadialDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	FVector Origin,
	FHitResult HitInfo,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	TakeHealthDamage(Damage); // TODO: Add factor by armor
	TakeArmorDamage(0.0f); // TODO: Add armor damage calc
}

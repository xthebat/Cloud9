// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9HealthComponent.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

UCloud9HealthComponent::UCloud9HealthComponent()
{
	Health = 100.0f;
	Armor = 0.0f;
	bHasHelmet = false;
	bIsAlive = true;
}

void UCloud9HealthComponent::Initialize(FHealthConfig Config)
{
	ChangeHealth(Config.Health);
	ChangeArmor(Config.Armor);
	ChangeHasHelmet(Config.bHasHelmet);
}

bool UCloud9HealthComponent::TakeHealthDamage(float Change)
{
	return ChangeHealth(Health - Change);
}

bool UCloud9HealthComponent::TakeArmorDamage(float Change)
{
	return ChangeArmor(Armor - Change);
}

bool UCloud9HealthComponent::ChangeHealth(float NewHealth)
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
				log(Fatal, "[Component=%s] Owner isn't valid", *GetName());
				return false;
			}

			bIsAlive = false;
			OnCharacterDie.Broadcast(Owner);
		}

		return true;
	}

	return false;
}

bool UCloud9HealthComponent::ChangeArmor(float NewArmor)
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

void UCloud9HealthComponent::OnRegister()
{
	Super::OnRegister();

	let MyOwner = GetOwner();

	if (not IsValid(MyOwner))
	{
		log(Error, "HealthComponent isn't valid");
		return;
	}

	// Register twice or delegate add twice (?)
	MyOwner->OnTakePointDamage.AddUniqueDynamic(this, &UCloud9HealthComponent::OnTakePointDamage);
	MyOwner->OnTakeRadialDamage.AddUniqueDynamic(this, &UCloud9HealthComponent::OnTakeRadialDamage);
}

void UCloud9HealthComponent::AddAttackerScore(const AController* InstigatedBy) const
{
	if (not bIsAlive)
	{
		if (not IsValid(InstigatedBy))
		{
			log(Error, "[Component = %s] InstigatedBy is invalid", *GetName());
			return;
		}

		// TODO: Make score component
		if (let Character = Cast<ACloud9Character>(InstigatedBy->GetCharacter()); IsValid(Character))
		{
			Character->AddScore();
		}
	}
}

bool UCloud9HealthComponent::ChangeHasHelmet(bool NewState)
{
	if (bHasHelmet != NewState)
	{
		bHasHelmet = NewState;
		OnHelmetChange.Broadcast(NewState);
		return true;
	}

	return false;
}

void UCloud9HealthComponent::OnTakePointDamage(
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
	AddAttackerScore(InstigatedBy);
}

void UCloud9HealthComponent::OnTakeRadialDamage(
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
	AddAttackerScore(InstigatedBy);
}

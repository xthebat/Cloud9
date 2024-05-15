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
	bIsInvulnerable = false;
}

void UCloud9HealthComponent::Initialize(FHealthConfig Config)
{
	ChangeHealth(Config.Health);
	ChangeArmor(Config.Armor);
	ChangeHasHelmet(Config.bHasHelmet);
}

bool UCloud9HealthComponent::TakeHealthDamage(float Change)
{
	OBJECT_RETURN_IF_FAIL(Change >= 0.0f, false, Warning, "Health change %f should be >= 0", Change);
	return ChangeHealth(Health - Change);
}

bool UCloud9HealthComponent::TakeArmorDamage(float Change)
{
	OBJECT_RETURN_IF_FAIL(Change >= 0.0f, false, Warning, "Armor change %f should be >= 0", Change);
	return ChangeArmor(Armor - Change);
}

bool UCloud9HealthComponent::IncreaseHealth(float Change)
{
	OBJECT_RETURN_IF_FAIL(Change >= 0.0f, false, Warning, "Health change %f should be >= 0", Change);
	return ChangeHealth(Health + Change);
}

bool UCloud9HealthComponent::IncreaseArmor(float Change)
{
	OBJECT_RETURN_IF_FAIL(Change >= 0.0f, false, Warning, "Armor change %f should be >= 0", Change);
	return ChangeArmor(Health + Change);
}

bool UCloud9HealthComponent::IsInvulnerable() const { return bIsInvulnerable; }

void UCloud9HealthComponent::IsInvulnerable(bool NewValue) { bIsInvulnerable = NewValue; }

bool UCloud9HealthComponent::ChangeHealth(float NewHealth)
{
	if (let Value = FMath::Max(NewHealth, 0.0f); Value != Health)
	{
		Health = Value;
		OnHealthChange.Broadcast(Health);

		if (Health == 0.0f)
		{
			let MyOwner = GetOwner();
			OBJECT_RETURN_IF_FAIL(IsValid(MyOwner), false, Fatal, "Owner isn't valid");

			bIsAlive = false;
			OnCharacterDie.Broadcast(MyOwner);
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
	OBJECT_VOID_IF_FAIL(IsValid(MyOwner), Error, "HealthComponent isn't valid");

	// Register twice or delegate add twice (?)
	MyOwner->OnTakePointDamage.AddUniqueDynamic(this, &UCloud9HealthComponent::OnTakePointDamage);
	MyOwner->OnTakeRadialDamage.AddUniqueDynamic(this, &UCloud9HealthComponent::OnTakeRadialDamage);
}

void UCloud9HealthComponent::AddAttackerScore(const AController* InstigatedBy) const
{
	if (not bIsAlive)
	{
		OBJECT_VOID_IF_FAIL(IsValid(InstigatedBy), Error, "InstigatedBy is invalid");

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
	TakeHealthDamage(Damage);
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
	TakeHealthDamage(Damage);
	AddAttackerScore(InstigatedBy);
}

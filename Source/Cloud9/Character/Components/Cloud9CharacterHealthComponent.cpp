// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9CharacterHealthComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

UCloud9CharacterHealthComponent::UCloud9CharacterHealthComponent()
{
	Health = 100.0f;
	Armor = 0.0f;
	bHasHelmet = false;
}

void UCloud9CharacterHealthComponent::ChangeHealth(float Change)
{
	Health = FMath::Max(Health + Change, 0.0f);

	OnHealthChange.Broadcast();

	if (Health == 0.0f)
	{
		let Owner = GetOwner();

		if (not IsValid(Owner))
		{
			log(Fatal, "[Component = %s] Owner isn't valid", *GetName());
			return;
		}

		Owner->MarkPendingKill();
	}
}

void UCloud9CharacterHealthComponent::ChangeArmor(float Change)
{
	Armor = FMath::Clamp(Armor + Change, 0.0f, 100.0f);
	OnArmorChange.Broadcast();
}

void UCloud9CharacterHealthComponent::ChangeHasHelmet(bool NewState)
{
	if (bHasHelmet != NewState)
	{
		bHasHelmet = NewState;
		OnHelmetChange.Broadcast();
	}
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
	ChangeHealth(-Damage);
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
	ChangeHealth(-Damage);
}

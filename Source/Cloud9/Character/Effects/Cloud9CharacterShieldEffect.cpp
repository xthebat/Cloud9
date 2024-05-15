// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9CharacterShieldEffect.h"

#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"

const FName UCloud9CharacterShieldEffect::ShieldEnableName = TEXT("Shield Enabled");
const FName UCloud9CharacterShieldEffect::ShieldReflectName = TEXT("Shield Reflect");
const FName UCloud9CharacterShieldEffect::ShieldPowerName = TEXT("Shield Power");
const FName UCloud9CharacterShieldEffect::ShieldColorName = TEXT("Shield Color");

void UCloud9CharacterShieldEffect::ToggleEffect(bool NewState)
{
	IsEnabled = NewState;
	let Character = GetCharacter();
	let HealthComponent = Character->GetHealthComponent();
	OBJECT_VOID_IF_FAIL(HealthComponent, Error, "HealthComponent is invalid");
	HealthComponent->IsInvulnerable(NewState);
	ToggleVisual(NewState);
}

void UCloud9CharacterShieldEffect::ToggleVisual(bool NewState)
{
	let Character = GetCharacter();
	let MeshComponent = Character->GetMesh();
	OBJECT_VOID_IF_FAIL(MeshComponent, Error, "MeshComponent is invalid");
	MeshComponent->GetMaterials()
		| ETContainer::Transform{[](let It) { return Cast<UMaterialInstanceDynamic>(It); }}
		| ETContainer::Filter{[](let It) { return IsValid(It); }}
		| ETContainer::ForEach{
			[this, NewState](let It)
			{
				if (NewState)
				{
					It->SetScalarParameterValue(ShieldReflectName, Reflect);
					It->SetScalarParameterValue(ShieldPowerName, Power);
					It->SetVectorParameterValue(ShieldColorName, Color);
				}

				It->SetScalarParameterValue(ShieldEnableName, NewState);
			}
		};
}

UCloud9CharacterShieldEffect::UCloud9CharacterShieldEffect()
{
	Duration = 5.0f;
	Color = FLinearColor::Red;
	Power = 10.0f;
	Reflect = 0.004f;
	ElapsedTime = 0.0f;
	IsEnabled = false;
}

bool UCloud9CharacterShieldEffect::IsExtinguished_Implementation() const { return ElapsedTime >= Duration; }

void UCloud9CharacterShieldEffect::OnApply_Implementation()
{
	Super::OnApply_Implementation();
	ToggleEffect(true);
}

void UCloud9CharacterShieldEffect::OnRemove_Implementation()
{
	Super::OnRemove_Implementation();
	ToggleEffect(false);
}

bool UCloud9CharacterShieldEffect::CanApply_Implementation() const
{
	// TODO: Check only one shield effect applied
	return true;
}

bool UCloud9CharacterShieldEffect::CanTick_Implementation() const { return true; }

void UCloud9CharacterShieldEffect::OnTick_Implementation(float DeltaSeconds) { ElapsedTime += DeltaSeconds; }

void UCloud9CharacterShieldEffect::OnSkeletalMeshChanged_Implementation(bool bReinitPose)
{
	ToggleVisual(IsEnabled);
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9CharacterEffectTrait.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9EffectsComponent.h"
#include "Cloud9/Character/Components/Cloud9SkeletalMeshComponent.h"

bool UCloud9CharacterEffectTrait::IsExtinguished_Implementation() const { return true; }

void UCloud9CharacterEffectTrait::OnApply_Implementation()
{
	let Character = GetCharacter();
	let MeshComponent = Cast<UCloud9SkeletalMeshComponent>(Character->GetMesh());
	CheckIsValid(MeshComponent, Error, "SkeletalMeshComponent is invalid");
	MeshComponent->OnSkeletalMeshChanged.AddDynamic(this, &UCloud9CharacterEffectTrait::OnSkeletalMeshChanged);
}

void UCloud9CharacterEffectTrait::OnRemove_Implementation()
{
	let Character = GetCharacter();
	let MeshComponent = Cast<UCloud9SkeletalMeshComponent>(Character->GetMesh());
	CheckIsValid(MeshComponent, Error, "SkeletalMeshComponent is invalid");
	MeshComponent->OnSkeletalMeshChanged.RemoveDynamic(this, &UCloud9CharacterEffectTrait::OnSkeletalMeshChanged);
}

bool UCloud9CharacterEffectTrait::CanApply_Implementation() const { return true; }

bool UCloud9CharacterEffectTrait::CanTick_Implementation() const { return false; }

bool UCloud9CharacterEffectTrait::CanDamaged_Implementation() const { return false; }

void UCloud9CharacterEffectTrait::OnTick_Implementation(float DeltaSeconds) {}

void UCloud9CharacterEffectTrait::OnApplyDamage_Implementation(float Damage) {}

void UCloud9CharacterEffectTrait::OnSkeletalMeshChanged_Implementation(bool bReinitPose) {}

ACloud9Character* UCloud9CharacterEffectTrait::GetCharacter() const
{
	let MyOuter = GetOuter();
	CheckIsValid(MyOuter, Fatal, "Effect outer parent object wasn't specified", nullptr);

	let Container = Cast<UCloud9EffectsComponent>(MyOuter);
	CheckIsValid(Container, Fatal, "Outer parent object is invalid", nullptr);

	let Character = Container->GetOwner<ACloud9Character>();
	CheckIsValid(Character, Fatal, "Owner is invalid", nullptr);

	return Character;
}

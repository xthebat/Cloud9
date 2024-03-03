// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9PhysicalMaterial.h"

constexpr float DecalRotationPitchOrRoll = -90.0f;
constexpr float DecalRotationYaw = 90.0f;

UCloud9PhysicalMaterial::UCloud9PhysicalMaterial()
{
	FirearmDecalSize = {8.0f, 8.0, 8.0f};
	FirearmDecalRotationMin = 0.0f;
	FirearmDecalRotationMax = 180.0f;
}

UMaterialInterface* UCloud9PhysicalMaterial::GetRandomFirearmDecal() const { return GetRandomItem(FirearmDecals); }

UNiagaraSystem* UCloud9PhysicalMaterial::GetRandomFirearmSquib() const { return GetRandomItem(FirearmEffects); }

FVector UCloud9PhysicalMaterial::GetFirearmDecalSize() const { return FirearmDecalSize; }

FRotator UCloud9PhysicalMaterial::GetFirearmDecalRotation(FVector Normal) const
{
	let RandomRotation = FMath::RandRange(FirearmDecalRotationMin, FirearmDecalRotationMax);
	let BaseRotation = Normal.Rotation();
	let CorrectiveRotation = FMath::IsNearlyEqual(Normal.Z, 1.0f, 0.1f)
		                         ? FRotator{DecalRotationPitchOrRoll, DecalRotationYaw, RandomRotation}
		                         : FRotator{RandomRotation, DecalRotationYaw, DecalRotationPitchOrRoll};
	return BaseRotation + CorrectiveRotation;
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9PhysicalMaterial.h"

UCloud9PhysicalMaterial::UCloud9PhysicalMaterial()
{
	FirearmDecalSize = {8.0f, 8.0, 8.0f};
	FirearmDecalScaleMin = 1.0f;
	FirearmDecalScaleMax = 1.0f;
	FirearmDecalRotationMin = 0.0f;
	FirearmDecalRotationMax = 180.0f;

	FirearmBackgroundDecalSize = {8.0f, 64.0, 64.0f};
	FirearmBackgroundDecalOffsetMin = -50.0f;
	FirearmBackgroundDecalOffsetMax = 50.0f;
	FirearmBackgroundDecalRotationMin = 0.0f;
	FirearmBackgroundDecalRotationMax = 180.0f;
	FirearmBackgroundDecalProbability = 0.5f;
	FirearmBackgroundDecalMaxDistance = 200.0f;
	FirearmHitSoundVolume = 1.0f;

	GrenadeDecalSize = {8.0f, 8.0f, 8.0f};
	MeleeDecalSize = {8.0f, 8.0f, 8.0f};
}

bool IsVertical(FVector Normal)
{
	return FMath::IsNearlyEqual(Normal.Z, 1.0f, 0.1f);
}

UMaterialInterface* UCloud9PhysicalMaterial::GetRandomFirearmDecal() const { return GetRandomItem(FirearmDecals); }

FVector UCloud9PhysicalMaterial::GetFirearmDecalSize() const
{
	let RandomScale = FMath::RandRange(FirearmDecalScaleMin, FirearmDecalScaleMax);
	return FirearmDecalSize * RandomScale;
}

FRotator UCloud9PhysicalMaterial::GetFirearmDecalRotation(FVector Normal) const
{
	let RandomRotation = FMath::RandRange(FirearmDecalRotationMin, FirearmDecalRotationMax);
	return Normal.Rotation() + FRotator{0.0f, 0.0f, RandomRotation};
}

USoundBase* UCloud9PhysicalMaterial::GetRandomFirearmHitSound() const { return GetRandomItem(FirearmHitSounds); }

USoundBase* UCloud9PhysicalMaterial::GetFirearmAltHitSound(const TSet<USoundBase*> Sounds) const
{
	let Sound = GetRandomItem(Sounds);
	OBJECT_RETURN_IF_FAIL(IsValid(Sound), nullptr, Warning, "Can't get alternative sound, fallback to base sound");
	return Sound;
}

USoundBase* UCloud9PhysicalMaterial::GetFirearmAlt1HitSound() const
{
	return GetFirearmAltHitSound(FirearmHitAlt1Sounds);
}

USoundBase* UCloud9PhysicalMaterial::GetFirearmAlt2HitSound() const
{
	return GetFirearmAltHitSound(FirearmHitAlt2Sounds);
}

USoundBase* UCloud9PhysicalMaterial::GetFirearmAlt3HitSound() const
{
	return GetFirearmAltHitSound(FirearmHitAlt3Sounds);
}

float UCloud9PhysicalMaterial::GetFirearmHitSoundVolume() const { return FirearmHitSoundVolume; }

UMaterialInterface* UCloud9PhysicalMaterial::GetRandomBackgroundDecal() const
{
	return GetRandomItem(FirearmBackgroundDecals);
}

FVector UCloud9PhysicalMaterial::GetBackgroundDecalSize() const
{
	return FirearmBackgroundDecalSize;
}

FVector UCloud9PhysicalMaterial::GetBackgroundDecalLocation(FVector Location, FVector Normal) const
{
	let OffsetH = FMath::RandRange(FirearmBackgroundDecalOffsetMin, FirearmBackgroundDecalOffsetMax);
	let OffsetV = FMath::RandRange(FirearmBackgroundDecalOffsetMin, FirearmBackgroundDecalOffsetMax);
	return IsVertical(Normal)
		       ? FVector{Location.X + OffsetH, Location.Y + OffsetV, Location.Z}
		       : FVector{Location.X + OffsetH, Location.Y, Location.Z + OffsetV};
}

FRotator UCloud9PhysicalMaterial::GetBackgroundDecalRotation(FVector Normal) const
{
	let RandomRotation = FMath::RandRange(FirearmBackgroundDecalRotationMin, FirearmBackgroundDecalRotationMax);
	return Normal.Rotation() + FRotator{0.0f, 0.0f, RandomRotation};
}


float UCloud9PhysicalMaterial::GetBackgroundDecalMaxDistance() const
{
	return FirearmBackgroundDecalMaxDistance;
}

bool UCloud9PhysicalMaterial::TestBackgroundDecalProbability() const
{
	return FMath::RandRange(0.0f, 1.0f) < FirearmBackgroundDecalProbability;
}

UNiagaraSystem* UCloud9PhysicalMaterial::GetRandomFirearmSquib(bool IsAlternative) const
{
	return IsAlternative ? GetRandomItem(FirearmAltEffects) : GetRandomItem(FirearmEffects);
}

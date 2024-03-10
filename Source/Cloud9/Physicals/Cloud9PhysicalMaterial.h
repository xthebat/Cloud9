// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Cloud9PhysicalMaterial.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType, Blueprintable, CollapseCategories)
class CLOUD9_API UCloud9PhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UCloud9PhysicalMaterial();

	UMaterialInterface* GetRandomFirearmDecal() const;
	UNiagaraSystem* GetRandomFirearmSquib(bool IsAlternative) const;

	UMaterialInterface* GetRandomBackgroundDecal() const;
	FVector GetBackgroundDecalSize() const;
	FVector GetBackgroundDecalLocation(FVector Location, FVector Normal) const;
	FRotator GetBackgroundDecalRotation(FVector Normal) const;
	float GetBackgroundDecalMaxDistance() const;
	bool TestBackgroundDecalProbability() const;

	FVector GetFirearmDecalSize() const;
	FRotator GetFirearmDecalRotation(FVector Normal) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Decal")
	TSet<UMaterialInterface*> FirearmDecals;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Decal")
	FVector FirearmDecalSize;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Decal",
		meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmDecalRotationMin;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Decal",
		meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmDecalRotationMax;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Effect")
	TSet<UNiagaraSystem*> FirearmEffects;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Hit Effect")
	TSet<UNiagaraSystem*> FirearmAltEffects;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal")
	TSet<UMaterialInterface*> FirearmBackgroundDecals;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal")
	FVector FirearmBackgroundDecalSize;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="-100", UIMax="100", ClampMin="-100", ClampMax="100"))
	float FirearmBackgroundDecalOffsetMin;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="-100", UIMax="100", ClampMin="-100", ClampMax="100"))
	float FirearmBackgroundDecalOffsetMax;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmBackgroundDecalRotationMin;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmBackgroundDecalRotationMax;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="0", UIMax="1.0", ClampMin="0", ClampMax="1.0"))
	float FirearmBackgroundDecalProbability;

	UPROPERTY(EditDefaultsOnly, Category="Firearm Background Decal",
		meta=(UIMin="0", UIMax="1000", ClampMin="0", ClampMax="1000"))
	float FirearmBackgroundDecalMaxDistance;


	UPROPERTY(EditDefaultsOnly, Category="Grenade Hit Decal")
	TSet<UMaterialInterface*> GrenadeDecals;

	UPROPERTY(EditDefaultsOnly, Category="Grenade Hit Decal")
	FVector GrenadeDecalSize;

	UPROPERTY(EditDefaultsOnly, Category="Grenade Hit Effect")
	TSet<UNiagaraSystem*> GrenadeEffects;


	UPROPERTY(EditDefaultsOnly, Category="Melee Hit Decal")
	TSet<UMaterialInterface*> MeleeDecals;

	UPROPERTY(EditDefaultsOnly, Category="Melee Hit Decal")
	FVector MeleeDecalSize;

	UPROPERTY(EditDefaultsOnly, Category="Melee Hit Effect")
	TSet<UNiagaraSystem*> MeleeEffects;

private:
	template <typename ReturnType>
	static ReturnType* GetRandomItem(const TSet<ReturnType*> Collection)
	{
		return Collection
			| ETContainer::Random{}
			| ETOptional::Get{[] { return nullptr; }};
	}
};

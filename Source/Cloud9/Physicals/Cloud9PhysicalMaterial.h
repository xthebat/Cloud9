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
	UNiagaraSystem* GetRandomFirearmSquib() const;

	FVector GetFirearmDecalSize() const;
	FRotator GetFirearmDecalRotation(FVector Normal) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	TSet<UMaterialInterface*> FirearmDecals;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	FVector FirearmDecalSize;

	UPROPERTY(EditDefaultsOnly, Category=Decal, meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmDecalRotationMin;

	UPROPERTY(EditDefaultsOnly, Category=Decal, meta=(UIMin="-180", UIMax="180", ClampMin="-180", ClampMax="180"))
	float FirearmDecalRotationMax;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TSet<UNiagaraSystem*> FirearmEffects;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	TSet<UMaterialInterface*> GrenadeDecals;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	TSet<UNiagaraSystem*> GrenadeEffects;

	UPROPERTY(EditDefaultsOnly, Category=Decal)
	TSet<UMaterialInterface*> MeleeDecals;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
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

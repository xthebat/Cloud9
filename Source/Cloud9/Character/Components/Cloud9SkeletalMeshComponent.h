// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Cloud9SkeletalMeshComponent.generated.h"

UCLASS()
class CLOUD9_API UCloud9SkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose) override;

protected:
	void MakeDynamicMaterials(const TArray<UMaterialInterface*>& Materials);
};

// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Cloud9SkeletalMeshComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkeletalMeshChanged, bool, bReinitPose);

UCLASS()
class CLOUD9_API UCloud9SkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose) override;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnSkeletalMeshChanged OnSkeletalMeshChanged;

protected:
	void MakeDynamicMaterials(const TArray<UMaterialInterface*>& Materials);
};

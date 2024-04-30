// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9SkeletalMeshComponent.h"

#include "Cloud9/Tools/Extensions/TContainer.h"

void UCloud9SkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	MakeDynamicMaterials(GetMaterials());
}

void UCloud9SkeletalMeshComponent::SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose)
{
	Super::SetSkeletalMesh(NewMesh, bReinitPose);
	let Materials = NewMesh->GetMaterials()
		| ETContainer::Transform{[](let It) { return It.MaterialInterface; }}
		| ETContainer::ToArray{};
	MakeDynamicMaterials(Materials);
}

void UCloud9SkeletalMeshComponent::MakeDynamicMaterials(const TArray<UMaterialInterface*>& Materials)
{
	// Material to copy dynamic properties from
	var FoundMaterialToCopy = GetMaterials() | ETContainer::Find{
		[](let It) { return Cast<UMaterialInstanceDynamic>(It); }
	};

	var MaterialToCopy = FoundMaterialToCopy ? Cast<UMaterialInstanceDynamic>(*FoundMaterialToCopy) : nullptr;

	Materials
		| ETContainer::WithIndex{}
		| ETContainer::Filter{[](let It) { return not Cast<UMaterialInstanceDynamic>(It.Value); }}
		| ETContainer::ForEach{
			[this, MaterialToCopy](let It)
			{
				var DynamicMaterial = UMaterialInstanceDynamic::Create(It.Value, this);

				if (MaterialToCopy)
				{
					DynamicMaterial->CopyParameterOverrides(MaterialToCopy);
				}

				SetMaterial(It.Key, DynamicMaterial);
			}
		};
}

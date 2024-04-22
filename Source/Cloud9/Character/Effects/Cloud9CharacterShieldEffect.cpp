// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9CharacterShieldEffect.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/TContainer.h"


UCloud9CharacterShieldEffect::UCloud9CharacterShieldEffect() {}


void UCloud9CharacterShieldEffect::BeginPlay()
{
	Super::BeginPlay();
}

bool UCloud9CharacterShieldEffect::Apply_Implementation(ACloud9Character* Character)
{
	if (let Mesh = Character->GetMesh(); IsValid(Mesh))
	{
		let Materials = Mesh->GetMaterials();
		let Dynamic = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0));
	}

	return true;
}

bool UCloud9CharacterShieldEffect::Remove_Implementation(ACloud9Character* Character)
{
	return true;
}

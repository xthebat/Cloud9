#include "Cloud9WeaponKnife.h"

ACloud9WeaponKnife::ACloud9WeaponKnife()
{
	bCanBeDropped = false;
	WeaponType = EWeaponType::Knife;
}

void ACloud9WeaponKnife::BeginPlay()
{
	Super::BeginPlay();
}


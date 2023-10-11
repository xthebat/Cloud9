#include "Cloud9WeaponKnife.h"

ACloud9WeaponKnife::ACloud9WeaponKnife()
{
	WeaponClass = EWeaponClass::Melee;
	WeaponType = EWeaponType::Knife;
	bHasSecondaryAction = true;
	bCanBeDropped = false;
	bIsAutomatic = true;

	PrimaryActionCooldown = 0.4;
	SecondaryActionCooldown = 1.0;

	BaseDamage = 45.0;

	DeployTime = 1.0;
}

#include "Cloud9WeaponFirearm.h"


ACloud9WeaponFirearm::ACloud9WeaponFirearm()
{
	WeaponClass = EWeaponClass::Firearm;
	
	ReloadTime = 0.0;
	MagazineRoundsCount = 0;
	TotalRoundsCount = 0;
	
	if (IsValid(Mesh))
	{
		MuzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(MuzzleFlashComponentName);
		MuzzleFlash->SetupAttachment(Mesh);
	}
}

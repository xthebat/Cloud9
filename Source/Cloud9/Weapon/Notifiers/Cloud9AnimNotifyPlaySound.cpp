// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9AnimNotifyPlaySound.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

void UCloud9AnimNotifyPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = MeshComp->GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		return;
	}

	let Weapon = Character->GetInventory()->GetSelectedWeapon();

	if (not IsValid(Weapon))
	{
		return;
	}

	if (not IsValid(Weapon->WeaponDefinition))
	{
		return;
	}

	// TODO: MAKE IT USING MAP
	
	let Info = Weapon->WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();

	if (Index >= Info->Sounds.ReloadSounds.Num())
	{
		return;
	}

	Weapon->PlaySound(Info->Sounds.ReloadSounds[Index], Settings->Volume);
}

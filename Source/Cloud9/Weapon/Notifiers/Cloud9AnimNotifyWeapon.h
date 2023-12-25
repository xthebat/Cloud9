// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Template.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

#include "Cloud9AnimNotifyWeapon.generated.h"

UCLASS(Blueprintable)
class CLOUD9_API UCloud9AnimNotifyWeapon : public UAnimNotify
{
	GENERATED_BODY()

protected:
	template <typename WeaponClassType, derivedtype(WeaponClassType, ACloud9WeaponBase)>
	static WeaponClassType* GetSelectedWeapon(USkeletalMeshComponent* MeshComp)
	{
		let Character = MeshComp->GetOwner<ACloud9Character>();

		if (not IsValid(Character))
		{
			log(Error, "Character is invalid");
			return nullptr;
		}

		let Inventory = Character->GetInventory();

		if (not IsValid(Inventory))
		{
			log(Error, "Invenotry is invalid");
			return nullptr;
		}

		let SelectedWeapon = Inventory->GetSelectedWeapon();

		if (not IsValid(SelectedWeapon))
		{
			log(Error, "Selected weapon is invalid");
			return nullptr;
		}

		let& WeaponDefinition = SelectedWeapon->WeaponDefinition;

		if (not IsValid(WeaponDefinition))
		{
			log(Error, "Selected weapon is not defined");
			return nullptr;
		}

		return Cast<WeaponClassType>(SelectedWeapon);
	}
};

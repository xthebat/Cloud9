// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Template.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
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
		FunctionAssertOrReturn(IsValid(Character), nullptr, Error, "Character is invalid");

		let Inventory = Character->GetInventoryComponent();
		FunctionAssertOrReturn(IsValid(Inventory), nullptr, Error, "Inventory is invalid");

		let SelectedWeapon = Inventory->GetSelectedWeapon();
		FunctionAssertOrReturn(IsValid(SelectedWeapon), nullptr, Error, "Selected weapon is invalid");
		FunctionAssertOrReturn(SelectedWeapon->IsWeaponDefined(), nullptr, Error, "Selected weapon is not defined");

		return Cast<WeaponClassType>(SelectedWeapon);
	}
};

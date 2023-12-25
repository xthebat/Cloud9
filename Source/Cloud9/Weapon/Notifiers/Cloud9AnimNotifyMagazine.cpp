// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9AnimNotifyMagazine.h"
#include "Engine/StaticMeshActor.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"

UCloud9AnimNotifyMagazine::UCloud9AnimNotifyMagazine()
{
	Action = EMagazineAction::Nothing;
}

FString UCloud9AnimNotifyMagazine::GetNotifyName_Implementation() const
{
	return Action | EUEnum::GetValueName{} | EFName::ToString{};
}

void UCloud9AnimNotifyMagazine::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponFirearm>(MeshComp))
	{
		switch (Action)
		{
		case EMagazineAction::Nothing:
			break;
		case EMagazineAction::Detach:
			SelectedWeapon->UpdateMagazineAttachment(true);
			break;
		case EMagazineAction::Attach:
			SelectedWeapon->UpdateMagazineAttachment(false);
			break;
		case EMagazineAction::Drop:
			SelectedWeapon->DropMagazine();
			break;
		}
	}
}

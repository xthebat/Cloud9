// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "Cloud9WeaponMelee.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9AnimationComponent.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "Cloud9/Weapon/Tables/WeaponTableMelee.h"
#include "Cloud9/Weapon/Structures/WeaponDefinition.h"

FWeaponId ACloud9WeaponMelee::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

bool ACloud9WeaponMelee::CanBeDropped() const { return false; }

const FMeleeWeaponInfo* ACloud9WeaponMelee::GetWeaponInfo() const
{
	return WeaponDefinition.GetWeaponInfo<FMeleeWeaponInfo>();
}

bool ACloud9WeaponMelee::OnInitialize(const FWeaponConfig& WeaponConfig)
{
	if (Super::OnInitialize(WeaponConfig))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FMeleeWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(WeaponConfig.GetSkinName());
		AssertOrReturn(MySkinInfo.Material, false, Error, "Skin material is invalid");

		return InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo.Material);
	}

	return false;
}

void ACloud9WeaponMelee::OnWeaponAddedToInventory()
{
	Super::OnWeaponAddedToInventory();
	ChangeMeshCollisionState(WeaponMesh, false);
}

void ACloud9WeaponMelee::OnWeaponRemovedFromInventory()
{
	Super::OnWeaponRemovedFromInventory();
	ChangeMeshCollisionState(WeaponMesh, true);
}

void ACloud9WeaponMelee::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();
	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	let Character = GetOwner<ACloud9Character>();
	let AnimComponent = Character->GetAnimationComponent();

	WEAPON_ANIM_COMPONENT_GUARD();

	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FMeleeWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	let HasSecondaryAction = PoseMontages->bHasSecondaryAction;

	if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->DeployMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::Deploy); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryStart))
	{
		WeaponState.ClearAction(EWeaponAction::PrimaryStart);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryLoop))
	{
		ExecuteAction(
			EWeaponAction::PrimaryLoop,
			WeaponInfo->SlashCycleTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->PrimaryActionMontage); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryEnd))
	{
		WeaponState.ClearAction(EWeaponAction::PrimaryEnd);
	}
	else if (HasSecondaryAction and WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		ExecuteAction(
			EWeaponAction::Secondary,
			WeaponInfo->StabCycleTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->SecondaryActionMontage); }
		);

		// no auto stab
		WeaponState.ClearAction(EWeaponAction::Secondary);
	}
}

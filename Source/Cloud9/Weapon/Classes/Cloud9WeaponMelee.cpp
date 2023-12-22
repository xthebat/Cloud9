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
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Weapon/Enums/MeleeActions.h"
#include "Cloud9/Weapon/Tables/WeaponTableMelee.h"
#include "Cloud9/Weapon/Structures/WeaponDefinition.h"

const FName ACloud9WeaponMelee::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ACloud9WeaponMelee::ACloud9WeaponMelee()
{
	if (WeaponMesh = CreateMeshComponent(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		log(Error, "Failed to create WeaponMeshComponent");
		return;
	}
}

FWeaponId ACloud9WeaponMelee::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

const UEnum* ACloud9WeaponMelee::GetWeaponActions() const { return StaticEnum<EMeleeAction>(); }

bool ACloud9WeaponMelee::CanBeDropped() const { return false; }

const UStaticMeshSocket* ACloud9WeaponMelee::GetSocketByName(FName SocketName) const
{
	return WeaponMesh->GetSocketByName(SocketName);
}

const UStaticMeshComponent* ACloud9WeaponMelee::GetWeaponMesh() const { return WeaponMesh; }

bool ACloud9WeaponMelee::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	if (Super::OnInitialize(NewWeaponId, NewWeaponSkin))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FMeleeWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(NewWeaponSkin);
		return InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo);
	}

	return false;
}

void ACloud9WeaponMelee::DeInitialize()
{
	Super::DeInitialize();
	WeaponMesh->SetStaticMesh(nullptr);
}

void ACloud9WeaponMelee::OnWeaponAddedToInventory()
{
	ChangeMeshCollisionState(WeaponMesh, false);
}

void ACloud9WeaponMelee::OnWeaponRemovedFromInventory() { ChangeMeshCollisionState(WeaponMesh, true); }

void ACloud9WeaponMelee::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();
	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = GetOwner<ACloud9Character>();
	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FMeleeWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	if (bIsPrimaryActionActive)
	{
		ExecuteAction(
			EMeleeAction::Slash,
			WeaponInfo->SlashCycleTime, [&]
			{
				return PlayAnimMontage(PoseMontages->PrimaryActionMontage) and
					PlayRandomSound(WeaponInfo->Sounds.SlashSounds, Settings->Volume);
			}
		);
	}
	else if (bIsSecondaryActionActive)
	{
		ExecuteAction(
			EMeleeAction::Slash,
			WeaponInfo->StabCycleTime, [&]
			{
				return PlayAnimMontage(PoseMontages->SecondaryActionMontage) and
					PlayRandomSound(WeaponInfo->Sounds.StabSounds, Settings->Volume);
			}
		);

		// no auto stab
		bIsSecondaryActionActive = false;
	}
}

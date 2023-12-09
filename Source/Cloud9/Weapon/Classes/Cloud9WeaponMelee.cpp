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

#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Tools/Extensions/Range.h"
#include "Cloud9/Tools/Extensions/TOptional.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Weapon/Enums/MeleeActions.h"
#include "Cloud9/Weapon/Tables/WeaponTableMelee.h"
#include "Cloud9/Weapon/Structures/WeaponInstance.h"

const FName ACloud9WeaponMelee::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ACloud9WeaponMelee::ACloud9WeaponMelee()
{
	if (WeaponMesh = CreateMeshComponent(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		log(Error, "Failed to create WeaponMeshComponent");
		return;
	}
}

EWeaponClass ACloud9WeaponMelee::GetWeaponClass() const { return EWeaponClass::Melee; }

const UEnum* ACloud9WeaponMelee::GetWeaponActions() const { return StaticEnum<EMeleeAction>(); }

bool ACloud9WeaponMelee::CanBeDropped() const { return false; }

void ACloud9WeaponMelee::OnConstruction(const FTransform& Transform)
{
	using namespace ETOptional;
	using namespace EFWeaponInfo;
	Super::OnConstruction(Transform);
	WEAPON_IS_INITIALIZED_GUARD();
	let MyWeaponInfo = WeaponInstance->GetWeaponInfo<FMeleeWeaponInfo>();
	let SkinInfo = MyWeaponInfo
		| GetSkinByName(Skin)
		| Get([&]
			{
				log(Error, "[Weapon='%s'] Skin '%s' not found", *GetName(), *Skin.ToString());
				return MyWeaponInfo
					| GetSkinByName()
					| Get();
			}
		);

	InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, SkinInfo);
}

void ACloud9WeaponMelee::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_INITIALIZED_GUARD();

	static let Settings = UCloud9DeveloperSettings::Get();

	// Check if any other action in progress on this weapon
	if (IsActionInProgress())
	{
		return;
	}

	let WeaponInfo = WeaponInstance->GetWeaponInfo<FMeleeWeaponInfo>();
	let Character = GetOwner<ACloud9Character>();
	let AnimInstance = Character->GetMesh()->GetAnimInstance();
	let PoseMontages = WeaponInstance->GetPoseMontages(Character->bIsCrouched);

	if (bIsPrimaryActionActive)
	{
		ExecuteAction(EMeleeAction::Slash, WeaponInfo->SlashCycleTime, [&]
		{
			if (not AnimInstance->Montage_Play(PoseMontages->PrimaryActionMontage))
			{
				log(Error, "[Weapon='%s'] Can't play montage", *GetName())
				return false;
			}

			if (let FireSound = WeaponInfo->Sounds.SlashSounds | ERange::Random())
			{
				*FireSound | EUSoundBase::Play(GetActorLocation(), Settings->Volume);
			}

			return true;
		});
	}
	else if (bIsSecondaryActionActive)
	{
		ExecuteAction(EMeleeAction::Slash, WeaponInfo->StabCycleTime, [&]
		{
			if (not AnimInstance->Montage_Play(PoseMontages->SecondaryActionMontage))
			{
				log(Error, "[Weapon='%s'] Can't play montage", *GetName())
				return false;
			}

			if (let FireSound = WeaponInfo->Sounds.StabSounds | ERange::Random())
			{
				*FireSound | EUSoundBase::Play(GetActorLocation(), Settings->Volume);
			}

			return true;
		});

		// no auto stab
		bIsSecondaryActionActive = false;
	}
}

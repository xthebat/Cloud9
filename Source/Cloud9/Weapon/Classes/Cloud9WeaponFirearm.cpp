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

#include "Cloud9WeaponFirearm.h"

#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/Range.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Enums/Cloud9FirearmActions.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableFirearm.h"
#include "Kismet/GameplayStatics.h"

const FName ACloud9WeaponFirearm::WeaponMeshComponentName = TEXT("WeaponMeshComponent");
const FName ACloud9WeaponFirearm::MagazineMeshComponentName = TEXT("MagazineMeshComponent");
const FName ACloud9WeaponFirearm::MuzzleFlashComponentName = TEXT("MuzzleFlashComponent");

const FName ACloud9WeaponFirearm::MagazineSocketName = TEXT("MagazineSocket");
const FName ACloud9WeaponFirearm::MuzzleFlashSocketName = TEXT("MuzzleFlashSocket");

ACloud9WeaponFirearm::ACloud9WeaponFirearm()
{
	Class = EWeaponClass::Firearm;
	Actions = StaticEnum<EFirearmAction>();

	if (WeaponMesh = CreateMesh(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		TRACE(Error, "Failed to create WeaponMeshComponent");
		return;
	}

	if (MagazineMesh = CreateMesh(MagazineMeshComponentName, MagazineSocketName); not IsValid(MagazineMesh))
	{
		TRACE(Error, "Failed to create MagazineMeshComponent");
		return;
	}

	if (MuzzleFlash = CreateEffect(MuzzleFlashComponentName, MuzzleFlashSocketName); not IsValid(MuzzleFlash))
	{
		TRACE(Error, "Failed to create MuzzleFlashComponent");
		return;
	}
}

void ACloud9WeaponFirearm::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	let MyWeaponInfo = GetWeaponInfo<FFirearmWeaponInfo>();
	let Skin = MyWeaponInfo | EFWeaponInfo::GetSkinByName<FFirearmWeaponSkin>();
	WeaponMesh->SetStaticMesh(Skin->Weapon);
	MagazineMesh->SetStaticMesh(Skin->Magazine);
	MuzzleFlash->SetAsset(MyWeaponInfo->Effects.MuzzleFlash);
}

void ACloud9WeaponFirearm::PrimaryAction(bool bIsReleased)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	if (not bIsReleased)
	{
		let WeaponInfo = GetWeaponInfo<FFirearmWeaponInfo>();
		ExecuteAction(EFirearmAction::Fire, WeaponInfo->CycleTime, [=]
		{
			let Character = GetOwner<ACloud9Character>();
			let Montage = Montages->GetPoseMontages(Character->bIsCrouched)->PrimaryActionMontage;
			let AnimInstance = Character->GetMesh()->GetAnimInstance();

			MuzzleFlash->Activate(true);

			if (not AnimInstance->Montage_Play(Montage))
			{
				TRACE(Error, "Can't play montage for '%s'", *Info->Label.ToString())
				return false;
			}

			if (let FireSound = WeaponInfo->Sounds.FireSounds | ERange::Random())
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), *FireSound, GetActorLocation(), Settings->Volume);
			}

			return true;
		});
	}
	else { }
}

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
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/TOptional.h"
#include "Cloud9/Weapon/Enums/FirearmActions.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"

const FName ACloud9WeaponFirearm::WeaponMeshComponentName = TEXT("WeaponMeshComponent");
const FName ACloud9WeaponFirearm::MagazineMeshComponentName = TEXT("MagazineMeshComponent");
const FName ACloud9WeaponFirearm::MuzzleFlashComponentName = TEXT("MuzzleFlashComponent");

const FName ACloud9WeaponFirearm::MagazineSocketName = TEXT("MagazineSocket");
const FName ACloud9WeaponFirearm::MuzzleFlashSocketName = TEXT("MuzzleFlashSocket");

ACloud9WeaponFirearm::ACloud9WeaponFirearm()
{
	if (WeaponMesh = CreateMeshComponent(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		log(Error, "Failed to create WeaponMeshComponent");
		return;
	}

	if (MagazineMesh = CreateMeshComponent(MagazineMeshComponentName, MagazineSocketName); not IsValid(MagazineMesh))
	{
		log(Error, "Failed to create MagazineMeshComponent");
		return;
	}

	if (MuzzleFlash = CreateEffectComponent(MuzzleFlashComponentName, MuzzleFlashSocketName); not IsValid(MuzzleFlash))
	{
		log(Error, "Failed to create MuzzleFlashComponent");
		return;
	}
}

EWeaponClass ACloud9WeaponFirearm::GetWeaponClass() const { return EWeaponClass::Firearm; }

const UEnum* ACloud9WeaponFirearm::GetWeaponActions() const { return StaticEnum<EFirearmAction>(); }

void ACloud9WeaponFirearm::OnConstruction(const FTransform& Transform)
{
	using namespace ETOptional;
	using namespace EFWeaponInfo;
	Super::OnConstruction(Transform);
	WEAPON_IS_INITIALIZED_GUARD();
	let MyWeaponInfo = WeaponInstance->GetWeaponInfo<FFirearmWeaponInfo>();
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
	InitializeMeshComponent(MagazineMesh, MyWeaponInfo->MagazineModel, SkinInfo);
	InitializeEffectComponent(MuzzleFlash, MyWeaponInfo->Effects.MuzzleFlash);
}

void ACloud9WeaponFirearm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_INITIALIZED_GUARD();

	static let Settings = UCloud9DeveloperSettings::Get();

	if (IsActionInProgress())
	{
		return;
	}

	let WeaponInfo = WeaponInstance->GetWeaponInfo<FFirearmWeaponInfo>();
	let Character = GetOwner<ACloud9Character>();
	let AnimInstance = Character->GetMesh()->GetAnimInstance();
	let PoseMontages = WeaponInstance->GetPoseMontages(Character->bIsCrouched);

	if (bIsPrimaryActionActive)
	{
		ExecuteAction(EFirearmAction::Fire, WeaponInfo->CycleTime, [&]
		{
			if (not AnimInstance->Montage_Play(PoseMontages->PrimaryActionMontage))
			{
				log(Error, "[Weapon='%s'] Can't play montage for ", *GetName());
				return false;
			}

			MuzzleFlash->Activate(true);

			if (let FireSound = WeaponInfo->Sounds.FireSounds | ERange::Random())
			{
				*FireSound | EUSoundBase::Play(GetActorLocation(), Settings->Volume);
			}

			let Controller = Character->GetCloud9Controller();

			if (not IsValid(Controller))
			{
				log(Error, "Can't hit object because player controller isn't valid")
				return false;
			}

			FHitResult CursorHit;
			if (not Controller->GetHitResultUnderCursor(ECC_Visibility, true, CursorHit))
			{
				log(Error, "Cursor not hit anything")
				return true;
			}

			let StartLocation = MuzzleFlash->GetComponentLocation();
			let EndLocation = CursorHit.Location;

			FHitResult LineHit;
			if (not GetWorld()->LineTraceSingleByChannel(LineHit, StartLocation, EndLocation, ECC_Visibility))
			{
				log(Error, "LineTraceSingleByChannel not hit anything")
				return true;
			}

			let Target = LineHit.Component;

			log(Error, "Target = %s Owner = %s", *Target->GetName(), *Target->GetOwner()->GetName());

			if (Target->IsSimulatingPhysics() and Target->Mobility == EComponentMobility::Movable)
			{
				var Direction = LineHit.Location - StartLocation;
				log(Error, "Direction = %s", *Direction.ToString())
				Direction.Normalize();
				Target->AddImpulse(1000.0 * Direction, NAME_None, true);
			}

			return true;
		});

		if (not WeaponInfo->bIsFullAuto)
		{
			bIsPrimaryActionActive = false;
		}
	}
	else {}
}

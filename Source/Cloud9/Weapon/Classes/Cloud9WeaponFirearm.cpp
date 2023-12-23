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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/FVector.h"

#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"

const FName ACloud9WeaponFirearm::WeaponMeshComponentName = TEXT("WeaponMeshComponent");
const FName ACloud9WeaponFirearm::MagazineMeshComponentName = TEXT("MagazineMeshComponent");
const FName ACloud9WeaponFirearm::SilencerMeshComponentName = TEXT("SilencerMeshComponent");
const FName ACloud9WeaponFirearm::MuzzleFlashComponentName = TEXT("MuzzleFlashComponent");

const FName ACloud9WeaponFirearm::MagazineSocketName = TEXT("MagazineSocket");
const FName ACloud9WeaponFirearm::SilencerSocketName = TEXT("SilencerSocket");
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

	if (SilencerMesh = CreateMeshComponent(SilencerMeshComponentName, SilencerSocketName); not IsValid(SilencerMesh))
	{
		log(Error, "Failed to create SilencerMeshComponent");
		return;
	}
}

FWeaponId ACloud9WeaponFirearm::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

const UStaticMeshSocket* ACloud9WeaponFirearm::GetSocketByName(FName SocketName) const
{
	return WeaponMesh->GetSocketByName(SocketName);
}

const UStaticMeshComponent* ACloud9WeaponFirearm::GetWeaponMesh() const { return WeaponMesh; }

bool ACloud9WeaponFirearm::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	if (Super::OnInitialize(NewWeaponId, NewWeaponSkin))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(NewWeaponSkin);

		if (not InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo))
		{
			log(Error, "[Weapon='%s'] Can't initilaize WeaponMesh", *GetName());
			return false;
		}

		if (not InitializeMeshComponent(MagazineMesh, MyWeaponInfo->MagazineModel, MySkinInfo))
		{
			log(Error, "[Weapon='%s'] Can't initilaize MagazineMesh", *GetName());
			return false;
		}

		if (not InitializeEffectComponent(MuzzleFlash, MyWeaponInfo->Effects.MuzzleFlash))
		{
			log(Error, "[Weapon='%s'] Can't initilaize MuzzleFlash", *GetName());
			return false;
		}

		if (MyWeaponInfo->SilencerModel)
		{
			return InitializeMeshComponent(SilencerMesh, MyWeaponInfo->SilencerModel, MySkinInfo);
		}

		return true;
	}

	return false;
}

void ACloud9WeaponFirearm::DeInitialize()
{
	Super::DeInitialize();
	WeaponMesh->SetStaticMesh(nullptr);
	MagazineMesh->SetStaticMesh(nullptr);
	MuzzleFlash->SetAsset(nullptr);
}

void ACloud9WeaponFirearm::OnWeaponAddedToInventory()
{
	ChangeMeshCollisionState(WeaponMesh, false);
	ChangeMeshCollisionState(MagazineMesh, false);
	ChangeMeshCollisionState(SilencerMesh, false);
}

void ACloud9WeaponFirearm::OnWeaponRemovedFromInventory()
{
	ChangeMeshCollisionState(WeaponMesh, true);
	ChangeMeshCollisionState(MagazineMesh, true);
	ChangeMeshCollisionState(SilencerMesh, true);
}

void ACloud9WeaponFirearm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();
	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = GetOwner<ACloud9Character>();
	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);
	let CommonData = WeaponDefinition.GetCommonData();

	if (bIsDeploying)
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&]
			{
				PlayAnimMontage(PoseMontages->DeployMontage);
				WeaponInfo->Sounds.DeploySound | EUSoundBase::Play{GetActorLocation(), Settings->Volume};
				return true;
			},
			[this] { bIsDeploying = false; }
		);
	}
	else if (bIsPrimaryActionActive)
	{
		ExecuteAction(
			EWeaponAction::Primary,
			WeaponInfo->CycleTime, [&]
			{
				if (PlayAnimMontage(PoseMontages->PrimaryActionMontage) and
					PlayRandomSound(WeaponInfo->Sounds.FireSounds, Settings->Volume))
				{
					MuzzleFlash->Activate(true);
					return Fire(WeaponInfo, CommonData->ImpulseMultiplier);
				}

				return false;
			},
			[] {}
		);

		if (not WeaponInfo->bIsFullAuto)
		{
			bIsPrimaryActionActive = false;
		}
	}
	else {}
}

bool ACloud9WeaponFirearm::Fire(const FFirearmWeaponInfo* WeaponInfo, float ImpulseMultiplier) const
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "Character is invalid")
		return false;
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

	log(Verbose, "Target = %s Owner = %s", *Target->GetName(), *Target->GetOwner()->GetName());

	if (Target->IsSimulatingPhysics() and Target->Mobility == EComponentMobility::Movable)
	{
		let Direction = (LineHit.Location - StartLocation) | EFVector::Normalize{};
		Target->AddImpulse(WeaponInfo->Damage * ImpulseMultiplier * Direction, NAME_None, true);
	}

	return true;
}

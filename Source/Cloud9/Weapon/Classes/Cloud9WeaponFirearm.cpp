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
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/FVector.h"

#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"
#include "Engine/StaticMeshActor.h"

FWeaponId ACloud9WeaponFirearm::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

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

	let LoopedReload = PoseMontages->ReloadLoopMontage != nullptr;

	if (WeaponState.IsActionActive(EWeaponAction::ReloadStart))
	{
		ExecuteAction(
			EWeaponAction::ReloadStart,
			WeaponInfo->ReloadTime,
			[&] { return PlayAnimMontage(PoseMontages->ReloadMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::ReloadStart); }
		);

		if (not LoopedReload)
		{
			WeaponState.ClearAction(EWeaponAction::ReloadLoop);
		}
	}
	else if (LoopedReload and WeaponState.IsActionActive(EWeaponAction::ReloadLoop))
	{
		ExecuteAction(
			EWeaponAction::ReloadLoop,
			WeaponInfo->ReloadLoopTime,
			[&] { return PlayAnimMontage(PoseMontages->ReloadLoopMontage); },
			[] {}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&]
			{
				if (PlayAnimMontage(PoseMontages->DeployMontage))
				{
					PlaySound(WeaponInfo->Sounds.DeploySound, Settings->Volume);
					return true;
				}

				return false;
			},
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::Deploy);
			}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Primary))
	{
		ExecuteAction(
			EWeaponAction::Primary,
			WeaponInfo->CycleTime, [&]
			{
				if (not PlayAnimMontage(PoseMontages->PrimaryActionMontage))
				{
					log(Error, "[Weapon='%s'] No montage for primary action specified", *GetName());
					return false;
				}

				if (not PlayRandomSound(WeaponInfo->Sounds.FireSounds, Settings->Volume))
				{
					log(Warning, "[Weapon='%s'] No any sound for primary action specified", *GetName());
				}

				MuzzleFlash->Activate(true);

				if (not Fire(WeaponInfo, CommonData->ImpulseMultiplier))
				{
					log(Error, "[Weapon='%s'] Weapon fire failure", *GetName());
					return false;
				}

				return true;
			},
			[] {}
		);

		if (not WeaponInfo->bIsFullAuto)
		{
			WeaponState.ClearAction(EWeaponAction::Primary);
		}
	}
	else {}
}

const FFirearmWeaponInfo* ACloud9WeaponFirearm::GetWeaponInfo() const
{
	return WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
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

	EjectCase();

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

bool ACloud9WeaponFirearm::UpdateMagazineAttachment(bool IsReload)
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Weapon owner is invalid", *GetName());
		return false;
	}

	UMeshComponent* Mesh;
	FName SocketName;
	bool IsDetached;

	if (IsReload)
	{
		let CharacterMesh = Character->GetMesh();

		if (not IsValid(CharacterMesh))
		{
			log(Error, "[Weapon='%s'] Character mesh is invalid", *GetName());
			return false;
		}

		SocketName = GetWeaponType() == EWeaponType::Pistol
			             ? UWeaponSlot::ReloadPistolSocket()
			             : UWeaponSlot::ReloadWeaponSocket();

		if (SocketName.IsNone())
		{
			log(Error, "[Weapon='%s'] Can't get socket name='%s'", *GetName(), *SocketName.ToString());
			return false;
		}

		if (not CharacterMesh->GetSocketByName(SocketName))
		{
			log(Error, "[Weapon='%s'] Socket not found in character mesh", *GetName());
			return false;
		}

		Mesh = CharacterMesh;
		IsDetached = true;
	}
	else
	{
		let Inventory = Character->GetInventory();

		if (not IsValid(Inventory))
		{
			log(Error, "[Weapon='%s'] Inventory is invalid", *GetName());
			return false;
		}

		let SelectedWeapon = Inventory->GetSelectedWeapon();

		if (not IsValid(SelectedWeapon))
		{
			log(Error, "[Weapon='%s'] Selected weapon is invalid", *GetName());
			return false;
		}

		Mesh = SelectedWeapon->GetWeaponMesh();

		SocketName = MagazineSocketName;
		IsDetached = false;
	}

	if (MagazineMesh->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SocketName))
	{
		log(
			Display,
			"[Weapon='%s'] Update magazine attachment to Mesh='%s' socket='%s'",
			*GetName(), *Mesh->GetName(), *SocketName.ToString());

		WeaponState.DetachMagazine(IsDetached);
		return true;
	}

	log(Error, "[Weapon='%s'] Can't change magazine attachement", *GetName());

	return false;
}

void ACloud9WeaponFirearm::DropMagazine() const
{
	let World = GetWorld();
	let Magazine = World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(),
		MagazineMesh->GetComponentTransform());

	Magazine->SetMobility(EComponentMobility::Movable);

	let Mesh = Magazine->GetStaticMeshComponent();
	Mesh->SetStaticMesh(MagazineMesh->GetStaticMesh());
	Mesh->SetMaterial(0, MagazineMesh->GetMaterial(0));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Magazine | EAActor::DestroyAfter{WeaponDefinition.GetCommonData()->MagazineLifetime};
}

void ACloud9WeaponFirearm::EjectCase() const
{
	let World = GetWorld();

	if (not GetWeaponMesh()->GetSocketByName(CaseEjectSocketName))
	{
		log(Error, "[Weapon='%s'] Socket case ejector not found", *GetName());
		return;
	}

	let Transform = GetWeaponMesh()->GetSocketTransform(CaseEjectSocketName);

	let CaseModel = GetWeaponInfo()->CaseModel;

	if (not IsValid(CaseModel))
	{
		log(Error, "[Weapon='%s'] Case model is invalid", *GetName());
		return;
	}

	let Case = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform);

	let CommonData = WeaponDefinition.GetCommonData();

	Case->SetMobility(EComponentMobility::Movable);
	Case->SetActorScale3D({CommonData->CaseScale, CommonData->CaseScale, CommonData->CaseScale});

	let Mesh = Case->GetStaticMeshComponent();
	Mesh->SetStaticMesh(CaseModel);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	let RightVector = GetWeaponMesh()->GetRightVector();
	let ForwardVector = GetWeaponMesh()->GetForwardVector();
	let Direction = RightVector.RotateAngleAxis(CommonData->CaseEjectAngle, ForwardVector);

	Mesh->AddImpulse(CommonData->CaseImpulse * Direction, NAME_None, true);

	Case | EAActor::DestroyAfter{CommonData->CaseLifetime};
}

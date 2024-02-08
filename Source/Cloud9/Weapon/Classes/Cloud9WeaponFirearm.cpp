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
#include "Engine/StaticMeshActor.h"
#include "NiagaraFunctionLibrary.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"

const FName ACloud9WeaponFirearm::TracerProbabilityParameterName = TEXT("Probability");
const FName ACloud9WeaponFirearm::TracerDirectionParameterName = TEXT("Direction");

FWeaponId ACloud9WeaponFirearm::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

bool ACloud9WeaponFirearm::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	if (Super::OnInitialize(NewWeaponId, NewWeaponSkin))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(NewWeaponSkin);

		if (MySkinInfo.Material == nullptr)
		{
			log(Error, "[Weapon='%s'] Skin material is invalid", *GetName());
			return false;
		}

		if (not InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo.Material))
		{
			log(Error, "[Weapon='%s'] Can't initilaize WeaponMesh", *GetName());
			return false;
		}

		if (not InitializeMeshComponent(
			MagazineMesh,
			MyWeaponInfo->MagazineModel,
			MyWeaponInfo->bIsMagazinePainted ? MySkinInfo.Material : nullptr))
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
			if (not InitializeMeshComponent(SilencerMesh, MyWeaponInfo->SilencerModel, MySkinInfo.Material))
			{
				log(Error, "[Weapon='%s'] Can't initilaize SilencerMesh", *GetName());
				return false;
			}
		}

		CurrentAmmo = MyWeaponInfo->MagazineSize;
		MagazineSize = MyWeaponInfo->MagazineSize;
		AmmoInReserve = MyWeaponInfo->MaxAmmoInReserve;

		log(Verbose, "[Weapon='%s'] CurrentAmmo=%d MagazineSize=%d AmmoInReserve=%d",
		    *GetName(), CurrentAmmo, MagazineSize, AmmoInReserve);

		return true;
	}

	return false;
}

void ACloud9WeaponFirearm::OnWeaponAddedToInventory()
{
	Super::OnWeaponAddedToInventory();
	ChangeMeshCollisionState(WeaponMesh, false);
	ChangeMeshCollisionState(MagazineMesh, false);
	ChangeMeshCollisionState(SilencerMesh, false);
}

void ACloud9WeaponFirearm::OnWeaponRemovedFromInventory()
{
	Super::OnWeaponRemovedFromInventory();
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

	let Character = GetOwner<ACloud9Character>();
	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);
	let CommonData = WeaponDefinition.GetCommonData();

	let HasSecondaryAction = PoseMontages->bHasSecondaryAction;
	let HasLoopedReload = PoseMontages->bHasReloadLoop;

	if (WeaponState.IsActionActive(EWeaponAction::ReloadStart))
	{
		ExecuteAction(
			EWeaponAction::ReloadStart,
			WeaponInfo->ReloadTime,
			[&]
			{
				return UpdateReloadAmmo(WeaponInfo->Type == EWeaponType::Shotgun)
					and PlayAnimMontage(PoseMontages->ReloadMontage);
			},
			[this, HasLoopedReload]
			{
				WeaponState.ClearAction(EWeaponAction::ReloadStart);

				if (not HasLoopedReload)
				{
					WeaponState.ClearAction(EWeaponAction::ReloadLoop, EWeaponAction::ReloadEnd);
				}
			}
		);
	}
	else if (HasLoopedReload and WeaponState.IsActionActive(EWeaponAction::ReloadLoop, EWeaponAction::ReloadEnd))
	{
		if (WeaponState.IsActionActive(EWeaponAction::ReloadLoop))
		{
			ExecuteAction(
				EWeaponAction::ReloadLoop,
				WeaponInfo->ReloadLoopTime,
				[&]
				{
					return UpdateReloadAmmo(WeaponInfo->Type == EWeaponType::Shotgun)
						and PlayAnimMontage(PoseMontages->ReloadLoopMontage);
				}
			);
		}
		else if (WeaponState.IsActionActive(EWeaponAction::ReloadEnd))
		{
			ExecuteAction(
				EWeaponAction::ReloadLoop,
				WeaponInfo->ReloadEndTime,
				[&] { return PlayAnimMontage(PoseMontages->ReloadEndMontage); }
			);

			WeaponState.ClearAction(EWeaponAction::ReloadEnd);
		}
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&] { return PlayAnimMontage(PoseMontages->DeployMontage); },
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
			EWeaponAction::PrimaryStart,
			WeaponInfo->CycleTime,
			[&]
			{
				let Status = Fire(WeaponInfo, CommonData->Firearm);

				if (Status == EFirearmFireStatus::Error)
				{
					log(Error, "[Weapon='%s'] Weapon fire failure", *GetName());
					return false;
				}

				if (Status == EFirearmFireStatus::Success)
				{
					if (not PlayAnimMontage(PoseMontages->PrimaryActionMontage))
					{
						log(Error, "[Weapon='%s'] No montage for primary action specified", *GetName());
						return false;
					}

					// TODO: May be move to notifier?
					MuzzleFlash->Activate(true);
				}

				return true;
			}
		);

		if (not WeaponInfo->bIsFullAuto)
		{
			WeaponState.ClearAction(EWeaponAction::PrimaryLoop);
		}
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryEnd))
	{
		WeaponState.ClearAction(EWeaponAction::PrimaryEnd);
	}
	else if (HasSecondaryAction and WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		// TODO: Implement secondary action for firearm	
	}
}

const FFirearmWeaponInfo* ACloud9WeaponFirearm::GetWeaponInfo() const
{
	return WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
}

int ACloud9WeaponFirearm::GetCurrentAmmo() const { return CurrentAmmo; }

int ACloud9WeaponFirearm::GetAmmoInReserve() const { return AmmoInReserve; }

bool ACloud9WeaponFirearm::AddAmmoInReserve(int Count)
{
	if (let NewAmmoInReserve = FMath::Min(AmmoInReserve + Count, GetWeaponInfo()->MaxAmmoInReserve);
		NewAmmoInReserve != AmmoInReserve)
	{
		AmmoInReserve = NewAmmoInReserve;
		return true;
	}

	return false;
}

EFirearmFireStatus ACloud9WeaponFirearm::Fire(
	const FFirearmWeaponInfo* WeaponInfo,
	const FFirearmCommonData& FirearmCommonData)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "Character is invalid")
		return EFirearmFireStatus::Error;
	}

	let Controller = Character->GetCloud9Controller();

	if (not IsValid(Controller))
	{
		log(Error, "Can't hit object because player controller isn't valid")
		return EFirearmFireStatus::Error;
	}

	if (CurrentAmmo == 0)
	{
		let EmptyAmmoSound = WeaponInfo->Type == EWeaponType::Pistol
			                     ? FirearmCommonData.EmptyAmmoPistol
			                     : FirearmCommonData.EmptyAmmoRifle;

		if (IsValid(EmptyAmmoSound))
		{
			UCloud9SoundPlayer::PlaySingleSound(EmptyAmmoSound, GetActorLocation(), Settings->Volume);
		}

		return EFirearmFireStatus::OutOfAmmo;
	}

	CurrentAmmo -= 1;

	EjectCase();

	FHitResult CursorHit;
	if (not Controller->GetHitResultUnderCursor(ECC_Visibility, true, CursorHit))
	{
		log(Error, "Cursor not hit anything")
		return EFirearmFireStatus::Success;
	}

	var CollisionParams = FCollisionQueryParams::DefaultQueryParam;

	if (Settings->bIsDrawHitScan)
	{
		const FName TraceTag("HitScanTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		CollisionParams.TraceTag = TraceTag;
		CollisionParams.bTraceComplex = true;
	}

	let StartLocation = MuzzleFlash->GetComponentLocation();
	// GetHitResultUnderCursor can return coordinates slightly upper then surface
	// Prolong line in shoot direction
	let EndLocation = FMath::Lerp(StartLocation, FVector{CursorHit.Location}, FirearmCommonData.LineTraceAlpha);

	FHitResult LineHit;
	let IsHit = GetWorld()->LineTraceSingleByChannel(
		LineHit,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		CollisionParams);

	if (Settings->bIsPrintHitScanInfo)
	{
		FString TargetName = IsHit ? *LineHit.Component->GetName() : TEXT("???");
		FString OwnerName = IsHit and LineHit.Component->GetOwner() != nullptr
			                    ? *LineHit.Component->GetOwner()->GetName()
			                    : TEXT("???");

		log(Display,
		    "Target='%s' Owner='%s' Start={%s} End={%s} TraceEnd={%s} Hit={%s} ",
		    *TargetName,
		    *OwnerName,
		    *StartLocation.ToString(),
		    *EndLocation.ToString(),
		    *LineHit.TraceEnd.ToString(),
		    *LineHit.Location.ToString()
		);
	}

	if (IsHit)
	{
		let Target = LineHit.Component;

		let Direction = LineHit.Location - StartLocation | EFVector::Normalize{};

		if (Target->IsSimulatingPhysics() and Target->Mobility == EComponentMobility::Movable)
		{
			let Velocity = WeaponInfo->Damage * FirearmCommonData.ImpulseMultiplier * Direction;
			Target->AddImpulse(Velocity, NAME_None, true);
		}

		if (IsValid(FirearmCommonData.Tracer))
		{
			let Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FirearmCommonData.Tracer,
				StartLocation);
			Tracer->SetVectorParameter(TracerDirectionParameterName, Direction);
			Tracer->SetFloatParameter(TracerProbabilityParameterName, WeaponInfo->TracerProbability);
			Tracer->SetAutoDestroy(true);
		}

		if (IsValid(FirearmCommonData.Squib))
		{
			let Squib = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FirearmCommonData.Squib,
				LineHit.Location,
				CursorHit.Normal.Rotation());
			Squib->SetAutoDestroy(true);
		}

		return EFirearmFireStatus::Success;
	}

	return EFirearmFireStatus::Success;
}

bool ACloud9WeaponFirearm::UpdateReloadAmmo(bool IsShotgun)
{
	if (AmmoInReserve == 0)
	{
		log(Display, "[Weapon='%s'] No ammo in reserve", *GetName());
		return false;
	}

	if (CurrentAmmo == MagazineSize)
	{
		log(Display, "[Weapon='%s'] Can't load more ammo", *GetName());
		return false;
	}

	let RequiredAmmo = IsShotgun ? 1 : FMath::Min(MagazineSize - CurrentAmmo, AmmoInReserve);

	AmmoInReserve -= RequiredAmmo;
	CurrentAmmo += RequiredAmmo;
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

		SocketName = UWeaponSlot::ReloadWeaponSocket(GetWeaponType());

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
		log(Verbose,
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

	Magazine | EAActor::DestroyAfter{WeaponDefinition.GetCommonData()->Firearm.MagazineLifetime};
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
	Case->SetActorRotation({0.0f, FMath::RandRange(0.0f, CommonData->Case.MaxEjectRotation), 0.0f});
	Case->SetActorScale3D({CommonData->Case.Scale, CommonData->Case.Scale, CommonData->Case.Scale});

	let Mesh = Case->GetStaticMeshComponent();
	Mesh->SetStaticMesh(CaseModel);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	let RightVector = GetWeaponMesh()->GetRightVector();
	let ForwardVector = GetWeaponMesh()->GetForwardVector();
	let ImpulseDirection = RightVector.RotateAngleAxis(CommonData->Case.EjectAngle, ForwardVector);

	Mesh->AddImpulse(CommonData->Case.EjectImpulse * ImpulseDirection, NAME_None, true);

	Case | EAActor::DestroyAfter{CommonData->Case.Lifetime};
}

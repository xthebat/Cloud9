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

#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/DecalComponent.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Tools/Extensions/APlayerController.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9AnimationComponent.h"
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
#include "Cloud9/Character/Damages/FirearmDamageType.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Physicals/Cloud9PhysicalMaterial.h"
#include "Cloud9/Tools/Math.h"
#include "Cloud9/Tools/Structures.h"
#include "Cloud9/Tools/Extensions/TArray.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

const FName ACloud9WeaponFirearm::TracerProbabilityParameterName = TEXT("Probability");
const FName ACloud9WeaponFirearm::TracerDirectionParameterName = TEXT("Direction");


TErrorValue<EFirearmFireStatus, FCursorHitScanInfo> FCursorHitScanInfo::Create(
	const ACloud9WeaponFirearm* Firearm,
	const FFirearmCommonData& FirearmCommonData)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = Firearm->GetOwner<ACloud9Character>();
	RETURN_IF_FAIL(IsValid(Character), EFirearmFireStatus::Error, Error, "Character is invalid");

	let Controller = Character->GetCloud9Controller();
	RETURN_IF_FAIL(
		IsValid(Controller), EFirearmFireStatus::Error,
		Error, "Can't hit object because player controller isn't valid");

	var Result = FCursorHitScanInfo();

	// When we do line hit scan ignore our character in any case (Enabled or not SelfAimOption)
	Result.ActorsToIgnore.Add(Character);
	Result.StartLocation = Firearm->GetShootLocationActor()->GetComponentLocation();

	if (not Settings->IsSelfAimEnabled)
	{
		TOptional<FHitResult> CursorHit = Controller | EAPlayerController::GetHitUnderCursor{
			TRACE_CHANNEL,
			true,
			Result.ActorsToIgnore
		};

		RETURN_IF_FAIL(
			CursorHit, EFirearmFireStatus::NoCursorHit,
			Error, "Cursor wasn't hit anything");

		Result.EndLocation = CursorHit->Location;
		Result.Alpha = FirearmCommonData.LineTraceAlpha;
	}
	else
	{
		TOptional<FHitResult> CursorHit = Controller | EAPlayerController::GetHitUnderCursor{
			TRACE_CHANNEL,
			true
		};

		RETURN_IF_FAIL(
			CursorHit, EFirearmFireStatus::NoCursorHit,
			Error, "Cursor wasn't hit anything");

		// Check if character use Somali shooting (when cursor on Character)
		if (CursorHit->Actor == Character)
		{
			Result.EndLocation = Result.StartLocation + Firearm->GetShootLocationActor()->GetForwardVector();
			Result.Alpha = FirearmCommonData.UnknownTraceAlpha;
		}
		else
		{
			Result.EndLocation = CursorHit->Location;
			Result.Alpha = FirearmCommonData.LineTraceAlpha;
		}
	}

	return Result;
}

FWeaponId ACloud9WeaponFirearm::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

bool ACloud9WeaponFirearm::OnInitialize(const FWeaponConfig& WeaponConfig)
{
	if (Super::OnInitialize(WeaponConfig))
	{
		let MyWeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(WeaponConfig.GetSkinName());
		OBJECT_RETURN_IF_FAIL(MySkinInfo.Material, false, Error, "Skin material is invalid");

		OBJECT_RETURN_IF_FAIL(
			InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo.Material), false,
			Error, "Can't initilaize WeaponMesh"
		);

		OBJECT_RETURN_IF_FAIL(
			InitializeMeshComponent(
				MagazineMesh, MyWeaponInfo->MagazineModel,
				MyWeaponInfo->bIsMagazinePainted ? MySkinInfo.Material : nullptr), false,
			Error, "Can't initilaize MagazineMesh"
		);

		OBJECT_RETURN_IF_FAIL(
			InitializeEffectComponent(MuzzleFlash, MyWeaponInfo->Effects.MuzzleFlash), false,
			Error, "Can't initilaize MuzzleFlash"
		);

		if (MyWeaponInfo->SilencerModel)
		{
			OBJECT_RETURN_IF_FAIL(
				InitializeMeshComponent(SilencerMesh, MyWeaponInfo->SilencerModel, MySkinInfo.Material), false,
				Error, "Can't initilaize SilencerMesh"
			);
		}

		MaxMagazineSize = MyWeaponInfo->MagazineSize;
		MaxAmmoInReserve = MyWeaponInfo->MaxAmmoInReserve;

		CurrentAmmo = WeaponConfig.GetAmmoInMagazine(MaxMagazineSize);
		AmmoInReserve = WeaponConfig.GetAmmoInReserve(MaxAmmoInReserve);

		AccuracyPenalty = 0.0f;
		LastShotDelta = 0.0f;
		RecoilPattern = 0.0f;

		OnAmmoInMagazineChanged.Broadcast(CurrentAmmo);
		OnAmmoInReserveChanged.Broadcast(AmmoInReserve);

		OBJECT_VERBOSE(
			"CurrentAmmo=%d MaxMagazineSize=%d AmmoInReserve=%d MaxAmmoInReserve=%d",
			CurrentAmmo, MaxMagazineSize, AmmoInReserve, MaxAmmoInReserve);

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

	OBJECT_VOID_IF_FAIL(IsWeaponDefined(), Error, "Weapon not defined");
	OBJECT_VOID_IF_FAIL(not IsWeaponDisarmed(), Verbose, "AnimComponent isn't valid");

	UpdateAccuracyPenalty(DeltaSeconds);

	OBJECT_VOID_IF_FAIL(not IsActionInProgress(), Verbose, "Action already in progress during Tick");

	let Character = GetOwner<ACloud9Character>(); // suppose a weapon has owner cus we pass bond guard
	let AnimComponent = Character->GetAnimationComponent();
	OBJECT_VOID_IF_FAIL(IsValid(AnimComponent), Error, "AnimComponent isn't valid");

	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();
	let BasePoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);
	let OtherPoseMontages = WeaponDefinition.GetPoseMontages(not Character->bIsCrouched);
	let CommonData = WeaponDefinition.GetCommonData();

	let HasSecondaryAction = BasePoseMontages->bHasSecondaryAction;
	let HasLoopedReload = BasePoseMontages->bHasReloadLoop;

	if (WeaponState.IsActionActive(EWeaponAction::ReloadStart))
	{
		ExecuteAction(
			EWeaponAction::ReloadStart,
			WeaponInfo->ReloadTime,
			[&]
			{
				// Stop current montage action to if change pose 
				AnimComponent->StopAllMontages(0.0f);

				return UpdateReloadAmmo(WeaponInfo->Type == EWeaponType::Shotgun)
					and AnimComponent->PlayMontage(
						BasePoseMontages->ReloadMontage,
						OtherPoseMontages->ReloadMontage
					);
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
			// Assume only shotguns has looped reload
			if (not CanReload())
			{
				WeaponState.ClearAction(EWeaponAction::ReloadLoop);
				WeaponState.ActivateAction(EWeaponAction::ReloadEnd);
			}
			else
			{
				ExecuteAction(
					EWeaponAction::ReloadLoop,
					WeaponInfo->ReloadLoopTime,
					[&]
					{
						return UpdateReloadAmmo(true) and
							AnimComponent->PlayMontage(
								BasePoseMontages->ReloadLoopMontage,
								OtherPoseMontages->ReloadLoopMontage
							);
					}
				);
			}
		}
		else if (WeaponState.IsActionActive(EWeaponAction::ReloadEnd))
		{
			ExecuteAction(
				EWeaponAction::ReloadLoop,
				WeaponInfo->ReloadEndTime,
				[&]
				{
					return AnimComponent->PlayMontage(
						BasePoseMontages->ReloadEndMontage,
						OtherPoseMontages->ReloadEndMontage
					);
				}
			);

			WeaponState.ClearAction(EWeaponAction::ReloadEnd);
		}
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&]
			{
				// Stop current montage action to if change pose 
				AnimComponent->StopAllMontages(0.0f);

				return AnimComponent->PlayMontage(
					BasePoseMontages->DeployMontage,
					OtherPoseMontages->DeployMontage);
			},
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
				let Status = PrimaryAttack(WeaponInfo, CommonData->Firearm);

				if (Status == EFirearmFireStatus::OutOfAmmo)
				{
					return true;
				}

				OBJECT_RETURN_IF_FAIL(
					Status != EFirearmFireStatus::Error, false,
					Error, "Weapon fire failure status=%d", Status);

				OBJECT_RETURN_IF_FAIL(
					AnimComponent->PlayMontage(
						BasePoseMontages->PrimaryActionMontage,
						OtherPoseMontages->PrimaryActionMontage
					), false,
					Error, "No montage for primary action specified"
				);

				// TODO: May be move to notifier?
				MuzzleFlash->Activate(true);

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
		OnAmmoInReserveChanged.Broadcast(AmmoInReserve);
		return true;
	}

	return false;
}

EFirearmFireStatus ACloud9WeaponFirearm::PrimaryAttack(
	const FFirearmWeaponInfo* WeaponInfo,
	const FFirearmCommonData& FirearmCommonData)
{
	static let Settings = UCloud9DeveloperSettings::Get();

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

	if (var [Error, HitScanInfo] = FCursorHitScanInfo::Create(this, FirearmCommonData);
		Error == EFirearmFireStatus::Success)
	{
		let EndLocations = RecalculateByShotInaccuracy(*HitScanInfo);

		let IsOk = EndLocations | ETContainer::AllByPredicate{
			[&](let EndLocation)
			{
				HitScanInfo->EndLocation = EndLocation;
				// 'PEW PEW' HAPPENS HERE
				return GunFire(WeaponInfo, FirearmCommonData, *HitScanInfo) == EFirearmFireStatus::Success;
			}
		};

		OBJECT_RETURN_IF_FAIL(IsOk, EFirearmFireStatus::Error, Error, "Gun fire error...");
	}

	AccuracyPenalty += WeaponInfo->GetInaccuracyFire();
	RecoilPattern += 1.0f;

	if (not Settings->IsCheatsEnabled or not Settings->IsInfiniteAmmo)
	{
		CurrentAmmo -= 1;
		OnAmmoInMagazineChanged.Broadcast(CurrentAmmo);
	}

	EjectCase();

	return EFirearmFireStatus::Success;
}

EFirearmFireStatus ACloud9WeaponFirearm::GunFire(
	const FFirearmWeaponInfo* WeaponInfo,
	const FFirearmCommonData& FirearmCommonData,
	const FCursorHitScanInfo& HitScanInfo)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), EFirearmFireStatus::Error, Error, "Character is invalid");

	// GetHitResultUnderCursor can return coordinates slightly upper then surface
	// Prolong line in shoot direction
	let EndLocation = HitScanInfo.ExtendedEndLocation();

	var CollisionParams = FCollisionQueryParams::DefaultQueryParam;

	CollisionParams.bReturnPhysicalMaterial = true;

	if (Settings->IsDrawHitScan)
	{
#ifndef UE_BUILD_SHIPPING
		const FName TraceTag("HitScanTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		CollisionParams.TraceTag = TraceTag;
		CollisionParams.bTraceComplex = true;
		CollisionParams.AddIgnoredActors(HitScanInfo.ActorsToIgnore);
#endif
	}

	FHitResult LineHit;
	let IsHit = GetWorld()->LineTraceSingleByChannel(
		LineHit,
		HitScanInfo.StartLocation,
		EndLocation,
		TRACE_CHANNEL,
		CollisionParams);

	if (Settings->IsPrintHitScanInfo)
	{
		FString TargetName = IsHit ? *LineHit.Component->GetName() : TEXT("???");
		FString OwnerName = IsHit and LineHit.Component->GetOwner() != nullptr
			                    ? *LineHit.Component->GetOwner()->GetName()
			                    : TEXT("???");
		FString PhysicalMaterial = LineHit.PhysMaterial.IsValid() ? *LineHit.PhysMaterial->GetName() : TEXT("???");

		OBJECT_DISPLAY(
			"Target='%s' Owner='%s' Material='%s' Start={%s} End={%s} TraceEnd={%s} Hit={%s} Normal={%s}",
			*TargetName,
			*OwnerName,
			*PhysicalMaterial,
			*HitScanInfo.StartLocation.ToString(),
			*EndLocation.ToString(),
			*LineHit.TraceEnd.ToString(),
			*LineHit.Location.ToString(),
			*LineHit.Normal.ToString()
		);
	}

	if (IsHit)
	{
		if (Cast<UWidgetComponent>(LineHit.Component))
		{
			let Component = Character->GetWidgetInteractionComponent();
			Component->SetCustomHitResult(LineHit);
			Component->PressPointerKey(EKeys::LeftMouseButton);
			Component->ReleasePointerKey(EKeys::LeftMouseButton);
			return EFirearmFireStatus::Success;
		}

		let Direction = LineHit.Location - HitScanInfo.StartLocation | EFVector::Normalize{};

		if (IsValid(FirearmCommonData.Tracer))
		{
			let Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FirearmCommonData.Tracer,
				HitScanInfo.StartLocation);
			Tracer->SetVectorParameter(TracerDirectionParameterName, Direction);
			Tracer->SetFloatParameter(TracerProbabilityParameterName, WeaponInfo->TracerProbability);
			Tracer->SetAutoDestroy(true);
		}

		let DamagedActor = Cast<AActor>(LineHit.Actor);
		OBJECT_RETURN_IF_FAIL(
			DamagedActor, EFirearmFireStatus::Success,
			Warning, "Line trace got hit but target actor is invalid");

		let Damage = UGameplayStatics::ApplyPointDamage(
			DamagedActor,
			WeaponInfo->Damage,
			Direction,
			LineHit,
			GetInstigatorController(),
			this,
			UFirearmDamageType::StaticClass()
		);

		let HealthComponent = DamagedActor->FindComponentByClass<UCloud9HealthComponent>();

		// If an actor has the health component (then can be damaged) or we shot in some stuff
		let IsNeedProcessPhysicalMaterial = Damage > 0.0f or not HealthComponent;

		if (let Target = LineHit.Component;
			Target.IsValid() and IsNeedProcessPhysicalMaterial)
		{
			if (Target->IsSimulatingPhysics() and Target->Mobility == EComponentMobility::Movable)
			{
				let Impulse = FMath::Clamp(
					Damage * FirearmCommonData.ImpulseMultiplier,
					FirearmCommonData.MinAppliedImpulse,
					FirearmCommonData.MaxAppliedImpulse);
				OBJECT_VERBOSE("[%s] Damage=%f Impulse=%f", *GetName(), Damage, Impulse);
				Target->AddImpulseAtLocation(Direction * Impulse, LineHit.Location, LineHit.BoneName);
			}

			if (let PhysicalMaterial = Cast<UCloud9PhysicalMaterial>(LineHit.PhysMaterial); IsValid(PhysicalMaterial))
			{
				if (IsValid(HealthComponent) and HealthComponent->IsArmored())
				{
					if (let FirearmSquib = PhysicalMaterial->GetRandomFirearmSquib(true); IsValid(FirearmSquib))
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(
							GetWorld(),
							FirearmSquib,
							LineHit.Location,
							LineHit.Normal.Rotation(),
							FVector::OneVector,
							true);
					}
				}

				if (let FirearmSquib = PhysicalMaterial->GetRandomFirearmSquib(false); IsValid(FirearmSquib))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						FirearmSquib,
						LineHit.Location,
						LineHit.Normal.Rotation(),
						FVector::OneVector,
						true);
				}

				if (let HitDecal = PhysicalMaterial->GetRandomFirearmDecal(); IsValid(HitDecal))
				{
					var Decal = UGameplayStatics::SpawnDecalAttached(
						HitDecal,
						PhysicalMaterial->GetFirearmDecalSize(),
						LineHit.GetComponent(),
						NAME_None,
						LineHit.Location,
						PhysicalMaterial->GetFirearmDecalRotation(-LineHit.ImpactNormal),
						EAttachLocation::KeepWorldPosition,
						Settings->DecalLifeSpan
					);

					Decal->SetFadeScreenSize(Settings->DecalFadeScreenSize);
				}

				let HitSound = GetHitSound(DamagedActor, PhysicalMaterial, HealthComponent, LineHit.BoneName);

				if (IsValid(HitSound))
				{
					UCloud9SoundPlayer::PlaySingleSound(
						HitSound,
						LineHit.Location,
						Settings->Volume * PhysicalMaterial->GetFirearmHitSoundVolume());
				}

				if (let BackgroundDecal = PhysicalMaterial->GetRandomBackgroundDecal(); IsValid(BackgroundDecal))
				{
					if (PhysicalMaterial->TestBackgroundDecalProbability())
					{
						var BackgroundCollisionParams = FCollisionQueryParams::DefaultQueryParam;
						BackgroundCollisionParams.AddIgnoredActor(DamagedActor);

						let StartBackgroundLocation = LineHit.Location;
						let EndBackgroundLocation = FMath::Lerp(
							FVector{StartBackgroundLocation},
							StartBackgroundLocation + Direction,
							PhysicalMaterial->GetBackgroundDecalMaxDistance());

						FHitResult BackgroundHit;
						let IsBackgroundHit = GetWorld()->LineTraceSingleByChannel(
							BackgroundHit,
							StartBackgroundLocation,
							EndBackgroundLocation,
							TRACE_CHANNEL,
							BackgroundCollisionParams);

						if (IsBackgroundHit)
						{
							var Decal = UGameplayStatics::SpawnDecalAttached(
								BackgroundDecal,
								PhysicalMaterial->GetBackgroundDecalSize(),
								BackgroundHit.GetComponent(),
								NAME_None,
								PhysicalMaterial->GetBackgroundDecalLocation(
									BackgroundHit.Location, BackgroundHit.Normal),
								PhysicalMaterial->GetBackgroundDecalRotation(BackgroundHit.ImpactNormal),
								EAttachLocation::KeepWorldPosition,
								Settings->DecalLifeSpan
							);

							Decal->SetFadeScreenSize(Settings->DecalFadeScreenSize);
						}
					}
				}
			}
		}
	}

	return EFirearmFireStatus::Success;
}

USoundBase* ACloud9WeaponFirearm::GetHitSound(
	AActor* DamagedActor,
	const UCloud9PhysicalMaterial* DamagedPhysicalMaterial,
	const UCloud9HealthComponent* DamagedHealthComponent,
	FName Bone)
{
	USoundBase* HitSound;

	if (let DamagedCharacter = Cast<ACloud9Character>(DamagedActor); IsValid(DamagedCharacter))
	{
		if (DamagedCharacter->IsHeadBone(Bone))
		{
			if (DamagedHealthComponent->HasHelmet())
			{
				HitSound = DamagedPhysicalMaterial->GetFirearmAlt3HitSound();
			}
			else
			{
				HitSound = DamagedPhysicalMaterial->GetFirearmAlt2HitSound();
			}
		}
		else
		{
			if (DamagedHealthComponent->GetArmor() > 0.0f)
			{
				HitSound = DamagedPhysicalMaterial->GetFirearmAlt1HitSound();
			}
			else
			{
				HitSound = DamagedPhysicalMaterial->GetRandomFirearmHitSound();
			}
		}
	}
	else
	{
		HitSound = DamagedPhysicalMaterial->GetRandomFirearmHitSound();
	}

	return HitSound;
}

bool ACloud9WeaponFirearm::HasAmmoInReserve() const { return AmmoInReserve != 0; }

bool ACloud9WeaponFirearm::IsFullyLoaded() const { return CurrentAmmo == MaxMagazineSize; }

bool ACloud9WeaponFirearm::CanReload() const { return HasAmmoInReserve() and not IsFullyLoaded(); }

bool ACloud9WeaponFirearm::UpdateReloadAmmo(bool IsShotgun)
{
	if (CanReload())
	{
		let RequiredAmmo = IsShotgun ? 1 : FMath::Min(MaxMagazineSize - CurrentAmmo, AmmoInReserve);
		AmmoInReserve -= RequiredAmmo;
		CurrentAmmo += RequiredAmmo;
		OnAmmoInReserveChanged.Broadcast(AmmoInReserve);
		OnAmmoInMagazineChanged.Broadcast(CurrentAmmo);
		return true;
	}

	return false;
}

// ======================================== begin cstrike15_src ====================================================

float ACloud9WeaponFirearm::GetInaccuracy() const
{
	static let Settings = UCloud9DeveloperSettings::Get();

	if (Settings->IsNoInaccuracy)
	{
		return 0.0f;
	}

	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		return 0.0f;
	}

	let WeaponInfo = GetWeaponInfo();

	float Inaccuracy = AccuracyPenalty;

	let Velocity = Character->GetVelocity();

	let MaxSpeed = WeaponInfo->MaxPlayerSpeed;

	var MovementInaccuracyScale = Math::RemapValueClamped(
		Velocity.Size2D(),
		MaxSpeed * Character->GetCloud9CharacterMovement()->SpeedDuckModifier,
		MaxSpeed * 0.95f, // max out at 95% of run speed to avoid jitter near max speed
		0.0f,
		1.0f);

	if (MovementInaccuracyScale > 0.0f)
	{
		// power curve only applies at speeds greater than walk
		if (not Character->bIsSneaking)
		{
			MovementInaccuracyScale = FMath::Pow(
				MovementInaccuracyScale,
				MovementCurve01Exponent
			);
		}

		Inaccuracy += MovementInaccuracyScale * WeaponInfo->GetInaccuracyMove();
	}

	// If we are in the air/on ladder, add inaccuracy based on vertical speed (maximum accuracy at apex of jump)
	if (not Character->GetMovementComponent()->IsMovingOnGround())
	{
		let VerticalSpeed = FMath::Abs(Velocity.Z);

		let InaccuracyJumpInitial = WeaponInfo->GetInaccuracyJumpInitial() * Settings->WeaponAirSpreadScale;

		// Use sqrt here to make the curve more "sudden" around the accurate point at the apex of the jump
		let SqrtMaxJumpSpeed = FMath::Sqrt(Settings->JumpImpulse);
		let SqrtVerticalSpeed = FMath::Sqrt(VerticalSpeed);

		var AirSpeedInaccuracy = Math::RemapValue(
			SqrtVerticalSpeed,

			SqrtMaxJumpSpeed * 0.25f,
			// Anything less than 6.25% of maximum speed has no additional accuracy penalty for z-motion (6.25% = .25 * .25)
			SqrtMaxJumpSpeed, // Penalty at max jump speed

			0.0f, // No movement-related penalty when close to stopped
			InaccuracyJumpInitial
		); // Movement-penalty at the start of jump

		// Clamp to min/max values.  (Don't use RemapValClamped because it makes clamping to > kJumpMovePenalty hard)
		if (AirSpeedInaccuracy < 0.0f)
		{
			AirSpeedInaccuracy = 0.0f;
		}
		else if (AirSpeedInaccuracy > MaxFallingPenalty * InaccuracyJumpInitial)
		{
			AirSpeedInaccuracy = MaxFallingPenalty * InaccuracyJumpInitial;
		}

		// Apply air velocity inaccuracy penalty
		// (There is an additional penalty for being in the air at all applied in UpdateAccuracyPenalty())
		Inaccuracy += AirSpeedInaccuracy;
	}

	return FMath::Min(Inaccuracy, 1.0f);
}

TArray<FVector> ACloud9WeaponFirearm::RecalculateByShotInaccuracy(
	const FCursorHitScanInfo& HitScanInfo,
	float Seed) const
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let WeaponInfo = GetWeaponInfo();

	// RandomSeed(Seed); // init random system with this seed

	let Direction = HitScanInfo.Direction();

	// Accuracy curve density adjustment FOR R8 REVOLVER SECONDARY FIRE, NEGEV WILD BEAST
	var RadiusCurveDensity = FMath::FRand();

	/*R8 REVOLVER SECONDARY FIRE*/
	if (GetWeaponId<EFirearm>() == EFirearm::Revolver and WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		RadiusCurveDensity = 1.0f - RadiusCurveDensity * RadiusCurveDensity;
	}

	// Negev currently isn't implemented but left this check if will be added
	if (GetWeaponId<EFirearm>() == EFirearm::Negev and RecoilPattern < 3) /* NEGEV WILD BEAST */
	{
		for (int j = 3; j > RecoilPattern; --j)
		{
			RadiusCurveDensity *= RadiusCurveDensity;
		}
		RadiusCurveDensity = 1.0f - RadiusCurveDensity;
	}

	if (Settings->WeaponDebugMaxInaccuracy)
	{
		RadiusCurveDensity = 1.0f;
	}

	// Get accuracy displacement
	var Theta0 = FMath::FRandRange(0.0f, 2.0f * PI);

	if (Settings->WeaponDebugInaccuracyOnlyUp)
	{
		Theta0 = PI * 0.5f;
	}

	let Inaccuracy = GetInaccuracy();
	let Radius0 = RadiusCurveDensity * Inaccuracy;
	let OffsetX0 = Radius0 * FMath::Cos(Theta0);
	let OffsetY0 = Radius0 * FMath::Sin(Theta0);

	CRASH_IF_FAIL(
		WeaponInfo->BulletsPerShot >= 1 and
		WeaponInfo->BulletsPerShot <= MaxBullets,
		"Invalid bullets per shot"
	);

	FVector Forward, Right, Up;
	let RotMatrix = FRotationMatrix(Direction.Rotation());
	RotMatrix.GetScaledAxes(Forward, Right, Up);

	if (Settings->DrawShotDirectionAxis)
	{
		DrawDebugLine(
			GetWorld(),
			HitScanInfo.StartLocation,
			HitScanInfo.StartLocation + Forward * 10.0f,
			FColor::Red,
			false,
			5.0
		);

		DrawDebugLine(
			GetWorld(),
			HitScanInfo.StartLocation,
			HitScanInfo.StartLocation + Right * 10.0f,
			FColor::Green,
			false,
			5.0
		);

		DrawDebugLine(
			GetWorld(),
			HitScanInfo.StartLocation,
			HitScanInfo.StartLocation + Up * 10.0f,
			FColor::Blue,
			false,
			5.0
		);
	}

	let DirectionLength = Direction.Size();

	// the RNG can be desynchronized by FireBullet(), so pre-generate all spread offsets
	return ETArray::ArrayOf(
		WeaponInfo->BulletsPerShot,
		[&](let Bullet)
		{
			// Spread curve density adjustment for R8 REVOLVER SECONDARY FIRE, NEGEV WILD BEAST
			var SpreadCurveDensity = FMath::FRand();

			if (GetWeaponId<EFirearm>() == EFirearm::Revolver and WeaponState.IsActionActive(
				EWeaponAction::Secondary))
			{
				SpreadCurveDensity = 1.0f - SpreadCurveDensity * SpreadCurveDensity;
			}

			if (GetWeaponId<EFirearm>() == EFirearm::Negev and RecoilPattern < 3) /*NEGEV WILD BEAST*/
			{
				for (int j = 3; j > RecoilPattern; --j)
				{
					SpreadCurveDensity *= SpreadCurveDensity;
				}

				SpreadCurveDensity = 1.0f - SpreadCurveDensity;
			}

			if (Settings->WeaponDebugMaxInaccuracy)
			{
				SpreadCurveDensity = 1.0f;
			}

			var Theta1 = FMath::FRandRange(0.0f, 2.0f * PI);

			if (Settings->WeaponDebugInaccuracyOnlyUp)
			{
				Theta1 = PI * 0.5f;
			}

			let Radius1 = SpreadCurveDensity * WeaponInfo->GetSpread();
			let OffsetX1 = OffsetX0 + Radius1 * FMath::Cos(Theta1);
			let OffsetY1 = OffsetY0 + Radius1 * FMath::Sin(Theta1);

			let InaccuracyDirection = Forward + Up * OffsetX1 + Right * OffsetY1 | EFVector::Normalize{};
			return InaccuracyDirection * DirectionLength + HitScanInfo.StartLocation;
		}
	);
}

void ACloud9WeaponFirearm::UpdateAccuracyPenalty(float DeltaSeconds)
{
	const float ExponentBase = FMath::Loge(10.0f);

	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = GetOwner<ACloud9Character>();
	OBJECT_VOID_IF_FAIL(IsValid(Character), Error, "Weapon owner is invalid");

	let WeaponInfo = GetWeaponInfo();

	float NewPenalty = 0.0f;

	if (Character->GetCloud9CharacterMovement()->IsOnLadder())
	{
		NewPenalty += 2.0f * WeaponInfo->GetInaccuracyLadder();
	}
	else if (Character->GetMovementComponent()->IsFlying())
	{
		NewPenalty += WeaponInfo->GetInaccuracyStand();
		NewPenalty += WeaponInfo->GetInaccuracyJump() * Settings->WeaponAirSpreadScale;
	}
	else if (Character->GetMovementComponent()->IsCrouching())
	{
		NewPenalty += WeaponInfo->GetInaccuracyCrouch();
	}
	else
	{
		NewPenalty += WeaponInfo->GetInaccuracyStand();
	}

	if (WeaponState.IsReloading())
	{
		NewPenalty += WeaponInfo->GetInaccuracyReload();
	}

	if (NewPenalty > AccuracyPenalty)
	{
		AccuracyPenalty = NewPenalty;
	}
	else
	{
		let RecoveryTime = GetRecoveryTime();
		let DecayFactor = ExponentBase / RecoveryTime;
		let PenaltyAlpha = FMath::Exp(DeltaSeconds * -DecayFactor);
		AccuracyPenalty = FMath::Lerp(NewPenalty, AccuracyPenalty, PenaltyAlpha);
	}

	LastShotDelta += DeltaSeconds;

	// *** Original condition with tickrate ***
	// Decay the recoil index if a little more than cycle time has elapsed since the last shot. In other words,
	// don't decay if we're firing full-auto.
	// if (gpGlobals->curtime > m_fLastShotTime + TICK_INTERVAL * Cloud9WeaponConsts::WeaponRecoilDecayThreshold)
	// *** Check only by elapsed time in frame and weapon cycle time *** 
	if (LastShotDelta > WeaponInfo->CycleTime * WeaponRecoilDecayThreshold)
	{
		LastShotDelta = 0.0f;
		let DecayFactor = ExponentBase * Settings->WeaponRecoilDecayCoefficient;
		let RecoilAlpha = FMath::Exp(DeltaSeconds * -DecayFactor);
		RecoilPattern = FMath::Lerp(0.0f, RecoilPattern, RecoilAlpha);
	}
}

float ACloud9WeaponFirearm::GetRecoveryTime() const
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), -1.0f, Error, "Weapon owner is invalid");

	let WeaponInfo = GetWeaponInfo();

	if (Character->GetCloud9CharacterMovement()->IsOnLadder())
	{
		// *** IDK why RecoveryTime with movement type == LADDER is RecoveryTimeStand ***
		return WeaponInfo->GetRecoveryTimeStand();
	}

	if (Character->GetMovementComponent()->IsFlying()) // in air
	{
		// enforce a large recovery speed penalty (400%) for players in the air; this helps to provide
		// comparable in-air accuracy to the old weapon model
		return WeaponInfo->GetRecoveryTimeCrouch() * 4.0f;
	}

	if (Character->GetMovementComponent()->IsCrouching())
	{
		float RecoveryTime = WeaponInfo->GetRecoveryTimeCrouch();
		float RecoveryTimeFinal = WeaponInfo->GetRecoveryTimeCrouchFinal();

		// uninitialized final recovery values are set to -1.0 from the weapon_base prefab in schema
		if (RecoveryTimeFinal != -1.0f)
		{
			const int RecoilIndex = RecoilPattern;

			RecoveryTime = Math::RemapValueClamped(
				RecoilIndex,
				WeaponInfo->GetRecoveryTransitionStartBullet(),
				WeaponInfo->GetRecoveryTransitionEndBullet(),
				RecoveryTime,
				RecoveryTimeFinal);
		}

		return RecoveryTime;
	}

	float RecoveryTime = WeaponInfo->GetRecoveryTimeStand();
	float RecoveryTimeFinal = WeaponInfo->GetRecoveryTimeStandFinal();

	// uninitialized final recovery values are set to -1.0 from the weapon_base prefab in schema
	if (RecoveryTimeFinal != -1.0f)
	{
		const int RecoilIndex = RecoilPattern;

		RecoveryTime = Math::RemapValueClamped(
			RecoilIndex,
			WeaponInfo->GetRecoveryTransitionStartBullet(),
			WeaponInfo->GetRecoveryTransitionEndBullet(),
			RecoveryTime,
			RecoveryTimeFinal);
	}

	return RecoveryTime;
}

// ======================================== end cstrike15_src ======================================================


bool ACloud9WeaponFirearm::UpdateMagazineAttachment(bool IsReload)
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), false, Error, "Weapon owner is invalid");

	UMeshComponent* Mesh;
	FName SocketName;
	bool IsDetached;

	if (IsReload)
	{
		let CharacterMesh = Character->GetMesh();
		OBJECT_RETURN_IF_FAIL(IsValid(CharacterMesh), false, Error, "Character mesh is invalid");

		SocketName = UWeaponSlot::ReloadWeaponSocket(GetWeaponType());
		OBJECT_RETURN_IF_FAIL(
			not SocketName.IsNone(), false,
			Error, "Can't get socket name='%s'", *SocketName.ToString());
		OBJECT_RETURN_IF_FAIL(
			CharacterMesh->GetSocketByName(SocketName), false,
			Error, "Socket not found in character mesh");

		Mesh = CharacterMesh;
		IsDetached = true;
	}
	else
	{
		let Inventory = Character->GetInventoryComponent();
		OBJECT_RETURN_IF_FAIL(IsValid(Inventory), false, Error, "Inventory is invalid");

		let SelectedWeapon = Inventory->GetSelectedWeapon();
		OBJECT_RETURN_IF_FAIL(IsValid(SelectedWeapon), false, Error, "Selected weapon is invalid");

		Mesh = SelectedWeapon->GetWeaponMesh();

		SocketName = MagazineSocketName;
		IsDetached = false;
	}

	// Attach in any case either to character (if reloading in progress) or to weapon (if reloading finished)
	// WARN: Also need to correct socket (size and rotation) in skeletal mesh if added other weapons 
	OBJECT_RETURN_IF_FAIL(
		MagazineMesh->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SocketName), false,
		Error, "Can't change magazine attachement");

	OBJECT_VERBOSE("Update magazine attachment to Mesh='%s' Socket='%s'", *Mesh->GetName(), *SocketName.ToString());

	WeaponState.DetachMagazine(IsDetached);
	return true;
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
	OBJECT_VOID_IF_FAIL(GetWeaponMesh()->GetSocketByName(CaseEjectSocketName), Error, "Socket case ejector not found");

	let Transform = GetWeaponMesh()->GetSocketTransform(CaseEjectSocketName);

	let CaseModel = GetWeaponInfo()->CaseModel;
	OBJECT_VOID_IF_FAIL(IsValid(CaseModel), Error, "Case model is invalid");

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

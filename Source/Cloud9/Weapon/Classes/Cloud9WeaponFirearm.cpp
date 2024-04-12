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
#include "Cloud9/Cloud9Consts.h"
#include "Components/WidgetInteractionComponent.h"

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
#include "Cloud9/Tools/Structures.h"
#include "Cloud9/Tools/Extensions/TArray.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "Cloud9/Weapon/Tables/WeaponTableFirearm.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

const FName ACloud9WeaponFirearm::TracerProbabilityParameterName = TEXT("Probability");
const FName ACloud9WeaponFirearm::TracerDirectionParameterName = TEXT("Direction");


TErrorValue<EFirearmFireStatus, FCursorHitScanInfo> FCursorHitScanInfo::Create(
	const ACloud9WeaponFirearm* Firearm,
	const FFirearmWeaponInfo* WeaponInfo,
	const FFirearmCommonData& FirearmCommonData)
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Character = Firearm->GetOwner<ACloud9Character>();

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

		if (not CursorHit)
		{
			log(Error, "Cursor wasn't hit anything")
			return EFirearmFireStatus::NoCursorHit;
		}

		Result.EndLocation = CursorHit->Location;
		Result.Alpha = FirearmCommonData.LineTraceAlpha;
	}
	else
	{
		TOptional<FHitResult> CursorHit = Controller | EAPlayerController::GetHitUnderCursor{
			TRACE_CHANNEL,
			true
		};

		if (not CursorHit)
		{
			log(Error, "Cursor wasn't anything")
			return EFirearmFireStatus::NoCursorHit;
		}

		// Check if use Somali shooting (when cursor on Character)
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

		MaxMagazineSize = MyWeaponInfo->MagazineSize;
		MaxAmmoInReserve = MyWeaponInfo->MaxAmmoInReserve;

		CurrentAmmo = WeaponConfig.GetAmmoInMagazine(MaxMagazineSize);
		AmmoInReserve = WeaponConfig.GetAmmoInReserve(MaxAmmoInReserve);

		AccuracyPenalty = 0.0f;
		LastShotDelta = 0.0f;
		RecoilPattern = 0.0f;

		OnAmmoInMagazineChanged.Broadcast(CurrentAmmo);
		OnAmmoInReserveChanged.Broadcast(AmmoInReserve);

		log(Verbose, "[Weapon='%s'] CurrentAmmo=%d MaxMagazineSize=%d AmmoInReserve=%d MaxAmmoInReserve=%d",
		    *GetName(), CurrentAmmo, MaxMagazineSize, AmmoInReserve, MaxAmmoInReserve);

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

	UpdateAccuracyPenalty(DeltaSeconds);

	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	let Character = GetOwner<ACloud9Character>(); // suppose weapon has owner cus we pass bond guard
	let AnimComponent = Character->GetAnimationComponent();

	WEAPON_ANIM_COMPONENT_GUARD();

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
					and AnimComponent->PlayMontage(PoseMontages->ReloadMontage);
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
						and AnimComponent->PlayMontage(PoseMontages->ReloadLoopMontage);
				}
			);
		}
		else if (WeaponState.IsActionActive(EWeaponAction::ReloadEnd))
		{
			ExecuteAction(
				EWeaponAction::ReloadLoop,
				WeaponInfo->ReloadEndTime,
				[&] { return AnimComponent->PlayMontage(PoseMontages->ReloadEndMontage); }
			);

			WeaponState.ClearAction(EWeaponAction::ReloadEnd);
		}
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Deploy))
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
			EWeaponAction::PrimaryStart,
			WeaponInfo->CycleTime,
			[&]
			{
				if (let Status = PrimaryAttack(WeaponInfo, CommonData->Firearm);
					Status > EFirearmFireStatus::PlayAnimation)
				{
					log(Error, "[Weapon='%s'] Weapon fire failure status = %d", *GetName(), Status);
					return false;
				}

				if (not AnimComponent->PlayMontage(PoseMontages->PrimaryActionMontage))
				{
					log(Error, "[Weapon='%s'] No montage for primary action specified", *GetName());
					return false;
				}

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

	if (var [Error, HitScanInfo] = FCursorHitScanInfo::Create(this, WeaponInfo, FirearmCommonData);
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

		if (not IsOk)
		{
			log(Error, "[Weapon='%s'] Gun fire error...", *GetName());
			return EFirearmFireStatus::Error;
		}
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

	if (not IsValid(Character))
	{
		log(Error, "Character is invalid")
		return EFirearmFireStatus::Error;
	}

	// GetHitResultUnderCursor can return coordinates slightly upper then surface
	// Prolong line in shoot direction
	let EndLocation = HitScanInfo.ExtendedEndLocation();

	var CollisionParams = FCollisionQueryParams::DefaultQueryParam;

	CollisionParams.bReturnPhysicalMaterial = true;

	if (Settings->IsDrawHitScan)
	{
		const FName TraceTag("HitScanTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		CollisionParams.TraceTag = TraceTag;
		CollisionParams.bTraceComplex = true;
		CollisionParams.AddIgnoredActors(HitScanInfo.ActorsToIgnore);
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

		log(Display,
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

		if (not IsValid(DamagedActor))
		{
			log(Warning, "[Weapon='%s'] Line trace got hit but target actor is invalid", *GetName());
			return EFirearmFireStatus::Success;
		}

		let Damage = UGameplayStatics::ApplyPointDamage(
			DamagedActor,
			WeaponInfo->Damage,
			Direction,
			LineHit,
			GetInstigatorController(),
			this,
			UFirearmDamageType::StaticClass()
		);

		if (let Target = LineHit.Component; Target.IsValid())
		{
			if (Target->IsSimulatingPhysics() and Target->Mobility == EComponentMobility::Movable)
			{
				let Impulse = FMath::Clamp(
					Damage * FirearmCommonData.ImpulseMultiplier,
					FirearmCommonData.MinAppliedImpulse,
					FirearmCommonData.MaxAppliedImpulse);
				log(Verbose, "[Weapon='%s'] Damage=%f Impulse=%f", *GetName(), Damage, Impulse);
				Target->AddImpulseAtLocation(Direction * Impulse, LineHit.Location, LineHit.BoneName);
			}

			if (let PhysicalMaterial = Cast<UCloud9PhysicalMaterial>(LineHit.PhysMaterial); IsValid(PhysicalMaterial))
			{
				if (let HealthComponent = DamagedActor->FindComponentByClass<UCloud9HealthComponent>();
					IsValid(HealthComponent) and HealthComponent->IsArmored())
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
					GetWorld() | EUWorld::SpawnDecal{
						.Material = HitDecal,
						.DecalSize = PhysicalMaterial->GetFirearmDecalSize(),
						.Location = LineHit.Location,
						.Rotator = PhysicalMaterial->GetFirearmDecalRotation(LineHit.Normal),
						.Owner = DamagedActor,
						.Instigator = Character
					};
				}

				if (let HitSound = PhysicalMaterial->GetRandomFirearmHitSound(); IsValid(HitSound))
				{
					HitSound | EUSoundBase::PlaySoundAtLocation{
						.Location = LineHit.Location,
						.VolumeMultiplier = Settings->Volume * PhysicalMaterial->GetFirearmHitSoundVolume()
					};
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
							GetWorld() | EUWorld::SpawnDecal{
								.Material = BackgroundDecal,
								.DecalSize = PhysicalMaterial->GetBackgroundDecalSize(),
								.Location = PhysicalMaterial->GetBackgroundDecalLocation(
									BackgroundHit.Location, BackgroundHit.Normal),
								.Rotator = PhysicalMaterial->GetBackgroundDecalRotation(BackgroundHit.Normal),
								.Owner = LineHit.Actor.Get(),
								.Instigator = Character
							};
						}
					}
				}
			}
		}
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

	if (CurrentAmmo == MaxMagazineSize)
	{
		log(Display, "[Weapon='%s'] Can't load more ammo", *GetName());
		return false;
	}

	let RequiredAmmo = IsShotgun ? 1 : FMath::Min(MaxMagazineSize - CurrentAmmo, AmmoInReserve);

	AmmoInReserve -= RequiredAmmo;
	CurrentAmmo += RequiredAmmo;
	OnAmmoInReserveChanged.Broadcast(AmmoInReserve);
	OnAmmoInMagazineChanged.Broadcast(CurrentAmmo);
	return true;
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

	let MaxSpeed = WeaponInfo->GetMaxSpeed();

	var MovementInaccuracyScale = UCloud9ToolsLibrary::RemapValueClamped(
		Velocity.Size2D(),
		MaxSpeed * Cloud9Player::SpeedDuckModifier,
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
				Cloud9WeaponConsts::MovementCurve01Exponent
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

		var AirSpeedInaccuracy = UCloud9ToolsLibrary::RemapValue(
			SqrtVerticalSpeed,

			SqrtMaxJumpSpeed * 0.25f,
			// Anything less than 6.25% of maximum speed has no additional accuracy penalty for z-motion (6.25% = .25 * .25)
			SqrtMaxJumpSpeed, // Penalty at max jump speed

			0.0f, // No movement-related penalty when close to stopped
			InaccuracyJumpInitial
		); // Movement-penalty at start of jump

		// Clamp to min/max values.  (Don't use RemapValClamped because it makes clamping to > kJumpMovePenalty hard)
		if (AirSpeedInaccuracy < 0.0f)
		{
			AirSpeedInaccuracy = 0.0f;
		}
		else if (AirSpeedInaccuracy > Cloud9WeaponConsts::MaxFallingPenalty * InaccuracyJumpInitial)
		{
			AirSpeedInaccuracy = Cloud9WeaponConsts::MaxFallingPenalty * InaccuracyJumpInitial;
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

	// Negev currently not implemented but left this check if will be added
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

	assert(
		WeaponInfo->BulletsPerShot >= 1 and
		WeaponInfo->BulletsPerShot <= Cloud9WeaponConsts::MaxBullets
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

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Weapon owner is invalid", *GetName());
		return;
	}

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
	if (LastShotDelta > WeaponInfo->CycleTime * Cloud9WeaponConsts::WeaponRecoilDecayThreshold)
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

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Weapon owner is invalid", *GetName());
		return -1.0f;
	}

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

			RecoveryTime = UCloud9ToolsLibrary::RemapValueClamped(
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

		RecoveryTime = UCloud9ToolsLibrary::RemapValueClamped(
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
		let Inventory = Character->GetInventoryComponent();

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

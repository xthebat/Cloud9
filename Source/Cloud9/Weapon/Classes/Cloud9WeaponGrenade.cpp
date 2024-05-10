﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9WeaponGrenade.h"

#include "DrawDebugHelpers.h"
#include "PhysicsEngine/RadialForceComponent.h"

#include "Cloud9/Tools/Extensions/APlayerController.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9AnimationComponent.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
#include "Cloud9/Character/Damages/GrenadeDamageType.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"
#include "Cloud9/Weapon/Tables/WeaponTableGrenade.h"
#include "Kismet/GameplayStatics.h"

const FName ACloud9WeaponGrenade::ExplosionComponentName = TEXT("ExplosionComponent");
const FName ACloud9WeaponGrenade::DetonationEffectComponentName = TEXT("DetonationEffectComponent");
const FName ACloud9WeaponGrenade::ActiveEffectComponentName = TEXT("ActiveEffectComponent");

ACloud9WeaponGrenade::ACloud9WeaponGrenade()
{
	Explosion = CreateDetonateComponent(ExplosionComponentName);
	AssertOrVoid(IsValid(Explosion), Error, "Failed to create URadialForceComponent");

	DetonationEffect = CreateEffectComponent(DetonationEffectComponentName);
	AssertOrVoid(IsValid(Explosion), Error, "Failed to create DetonationEffect");

	ActiveEffect = CreateEffectComponent(ActiveEffectComponentName);
	AssertOrVoid(IsValid(Explosion), Error, "Failed to create ActiveEffect");
}

FWeaponId ACloud9WeaponGrenade::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

const FGrenadeWeaponInfo* ACloud9WeaponGrenade::GetWeaponInfo() const
{
	return WeaponDefinition.GetWeaponInfo<FGrenadeWeaponInfo>();
}

bool ACloud9WeaponGrenade::OnInitialize(const FWeaponConfig& WeaponConfig)
{
	if (Super::OnInitialize(WeaponConfig))
	{
		let MyWeaponInfo = GetWeaponInfo();
		let MySkinInfo = MyWeaponInfo | EFWeaponInfo::GetSkinByNameOrThrow(WeaponConfig.GetSkinName());
		AssertOrReturn(MySkinInfo.Material, false, Error, "Skin material is invalid");

		let& [OnDetonationEffect, OnDetonationScale, OnActiveEffect, OnActiveScale] = MyWeaponInfo->Effects;

		if (OnDetonationEffect != nullptr)
		{
			InitializeEffectComponent(DetonationEffect, OnDetonationEffect, OnDetonationScale);
		}

		if (OnActiveEffect != nullptr)
		{
			InitializeEffectComponent(ActiveEffect, OnActiveEffect, OnActiveScale);
		}

		let CommonData = WeaponDefinition.GetCommonData();

		Explosion->Radius = GetWeaponInfo()->Radius;
		Explosion->ImpulseStrength = GetWeaponInfo()->Damage * CommonData->Grenade.ImpulseMultiplier;
		Explosion->bIgnoreOwningActor = true;
		Explosion->bImpulseVelChange = true;

		return InitializeMeshComponent(WeaponMesh, MyWeaponInfo->WeaponModel, MySkinInfo.Material);
	}

	return false;
}

void ACloud9WeaponGrenade::BeginPlay()
{
	Super::BeginPlay();
	DetonationEffect->Deactivate();
	ActiveEffect->Deactivate();
}

void ACloud9WeaponGrenade::OnWeaponAddedToInventory()
{
	Super::OnWeaponAddedToInventory();
	ChangeMeshCollisionState(WeaponMesh, false);
}

void ACloud9WeaponGrenade::OnWeaponRemovedFromInventory()
{
	Super::OnWeaponRemovedFromInventory();
	ChangeMeshCollisionState(WeaponMesh, true);
}

void ACloud9WeaponGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();

	if (WeaponState.IsGrenadeActionFinished())
	{
		OnGrenadeActionFinished();
		return;
	}

	if (WeaponState.IsGrenadeActionLoop())
	{
		OnGrenadeActionLoop();
		return;
	}

	if (WeaponState.IsGrenadeThrown())
	{
		OnGrenadeThrown();
		return;
	}

	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	let Character = GetOwner<ACloud9Character>();
	let AnimComponent = Character->GetAnimationComponent();

	WEAPON_ANIM_COMPONENT_GUARD();

	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			GetWeaponInfo()->DeployTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->DeployMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::Deploy); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryStart))
	{
		ExecuteAction(
			EWeaponAction::PrimaryStart,
			GetWeaponInfo()->PinpullTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->PinpullPrimaryActionMontage); },
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::PrimaryStart);
				WeaponState.ActivateAction(EWeaponAction::PrimaryLoop);
			}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryLoop))
	{
		// Play hold frame of montage
		AnimComponent->PlayMontage(
			PoseMontages->PinpullPrimaryActionMontage,
			PoseMontages->PinpullPrimaryActionHoldTiming);

		if (WeaponState.IsActionActive(EWeaponAction::PrimaryEnd))
		{
			WeaponState.ClearAction(EWeaponAction::PrimaryLoop);
		}
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		// TODO: Implement near throw (or cancel throw) for grenade
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Third))
	{
		// TODO: Implement middle throw (or cancel throw) for grenade
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryEnd))
	{
		ExecuteAction(
			EWeaponAction::PrimaryEnd,
			GetWeaponInfo()->ThrowTime,
			[&] { return AnimComponent->PlayMontage(PoseMontages->PrimaryActionMontage); },
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::PrimaryEnd);

				if (Throw())
				{
					WeaponState.GrenadeThrown();
				}
			}
		);
	}
}

bool ACloud9WeaponGrenade::OnGrenadeActionFinished()
{
	if (IsValid(this))
	{
		Destroy();
		SetActorTickEnabled(false);
		return true;
	}

	return false;
}

bool ACloud9WeaponGrenade::OnGrenadeActionLoop()
{
	static let Settings = UCloud9DeveloperSettings::Get();

	if (ActiveTimerHandle.IsValid())
	{
		return false;
	}

	// TODO: look like active effect not required (can be replaced by niagara effect)
	if (IsValid(ActiveEffect))
	{
		ActiveEffect->Activate();
	}

	if (let LoopSound = GetWeaponInfo()->Sounds.LoopSound; IsValid(LoopSound))
	{
		UCloud9SoundPlayer::PlaySingleSound(LoopSound, GetActorLocation(), Settings->Volume);
	}

	if (GetWeaponInfo()->bIsDestroyedOnDetonation)
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}

	// Moved out from timer because switching level can lead to crash with timer complex code
	if (IsValid(DetonationEffect))
	{
		DetonationEffect->Activate();
	}

	let Location = GetActorLocation();

	let IgnoredActors = TArray<AActor*>{};

	let InstigatorController = GetInstigatorController();

	// TODO: Handle other types of nades if required
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		GetWeaponInfo()->Damage,
		GetActorLocation(),
		Explosion->Radius,
		UGrenadeDamageType::StaticClass(),
		IgnoredActors,
		this,
		InstigatorController,
		false,
		TRACE_CHANNEL);
	Explosion->FireImpulse();

	if (let ExplodeSounds = GetWeaponInfo()->Sounds.ExplodeSounds; ExplodeSounds.Num() > 0)
	{
		UCloud9SoundPlayer::PlayRandomSound(ExplodeSounds, Location, Settings->Volume);
	}

	if (Settings->IsDrawExplosionSpheres)
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 0.5f,
			DebugSphereSegments,
			FColor::Red,
			false,
			DebugSphereLifeTime,
			0,
			DebugSphereThickness
		);

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 0.75f,
			DebugSphereSegments,
			FColor::Yellow,
			false,
			DebugSphereLifeTime,
			0,
			DebugSphereThickness
		);

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 1.0f,
			DebugSphereSegments,
			FColor::Green,
			false,
			DebugSphereLifeTime,
			0,
			DebugSphereThickness
		);
	}

	// Activate action finished timer
	ActiveTimerHandle = GetWorld()
		| EUWorld::AsyncAfter{
			[this] { WeaponState.GrenadeActionFinished(); },
			GetWeaponInfo()->ActionTime
		};

	return true;
}

bool ACloud9WeaponGrenade::OnGrenadeThrown()
{
	if (DetonationTimerHandle.IsValid())
	{
		return false;
	}

	let IsOnGround = GetVelocity().IsZero();
	let CanDetonateInAir = GetWeaponInfo()->bCanDetonateInAir;

	if (not CanDetonateInAir and not IsOnGround)
	{
		return false;
	}

	DetonationTimerHandle = GetWorld()
		| EUWorld::AsyncAfter{
			[this] { WeaponState.GrenadeActionLoop(); },
			GetWeaponInfo()->TimeToDetonate
		};

	return true;
}

bool ACloud9WeaponGrenade::Throw() const
{
	let Character = GetOwner<ACloud9Character>();
	AssertOrReturn(IsValid(Character), false, Error, "Character is invalid");

	let Controller = Character->GetCloud9Controller();
	AssertOrReturn(IsValid(Controller), false, Error, "Controller is invalid");

	let Inventory = Character->GetInventoryComponent();
	AssertOrReturn(IsValid(Inventory), false, Error, "Inventory is invalid");

	let ActorsToIgnore = TArray<AActor*>{Character};
	let CursorHit = Controller | EAPlayerController::GetHitUnderCursor{
		TRACE_CHANNEL,
		true,
		ActorsToIgnore
	};

	AssertOrReturn(CursorHit, false, Error, "Cursor not hit anything");

	static let Settings = UCloud9DeveloperSettings::Get();
	FWeaponConfig GrenadeConfig;
	let IsInfiniteAmmo = Settings->IsCheatsEnabled and Settings->IsInfiniteAmmo;

	if (IsInfiniteAmmo)
	{
		GrenadeConfig = FWeaponConfig::FromWeapon(this);
	}

	let CommonData = WeaponDefinition.GetCommonData();
	Inventory->DropWeapon(
		GetWeaponSlot(),
		CursorHit->Location,
		CommonData->Grenade.MaxThrowAngle,
		CommonData->Grenade.MaxThrowImpulse);

	if (IsInfiniteAmmo)
	{
		Inventory->AddWeapon(GrenadeConfig);
		// need to force select for valid grenade state
		Inventory->SelectWeapon(GrenadeConfig.GetWeaponSlot(), true, true);
	}

	return true;
}

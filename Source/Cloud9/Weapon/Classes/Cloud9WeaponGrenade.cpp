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

#include "Cloud9WeaponGrenade.h"

#include "DrawDebugHelpers.h"
#include "PhysicsEngine/RadialForceComponent.h"

#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Character/Cloud9Character.h"
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
	if (Explosion = CreateDetonateComponent(ExplosionComponentName); not IsValid(Explosion))
	{
		log(Error, "Failed to create URadialForceComponent");
		return;
	}

	if (DetonationEffect = CreateEffectComponent(DetonationEffectComponentName); not IsValid(DetonationEffect))
	{
		log(Error, "Failed to create DetonationEffect");
		return;
	}

	if (ActiveEffect = CreateEffectComponent(ActiveEffectComponentName); not IsValid(ActiveEffect))
	{
		log(Error, "Failed to create ActiveEffect");
		return;
	}
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

		if (MySkinInfo.Material == nullptr)
		{
			log(Error, "[Weapon='%s'] Skin material is invalid", *GetName());
			return false;
		}

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
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			GetWeaponInfo()->DeployTime,
			[&] { return PlayAnimMontage(PoseMontages->DeployMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::Deploy); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryStart))
	{
		log(Display, "EWeaponAction::Primary");

		ExecuteAction(
			EWeaponAction::PrimaryStart,
			GetWeaponInfo()->PinpullTime,
			[&] { return PlayAnimMontage(PoseMontages->PinpullPrimaryActionMontage); },
			[this]
			{
				WeaponState.ClearAction(EWeaponAction::PrimaryStart);
				WeaponState.ActivateAction(EWeaponAction::PrimaryLoop);
			}
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::PrimaryLoop))
	{
		log(Display, "EWeaponAction::PrimaryLoop");

		// Play hold frame of montage
		PlayAnimMontage(
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
		log(Display, "EWeaponAction::PrimaryEnd");

		ExecuteAction(
			EWeaponAction::PrimaryEnd,
			GetWeaponInfo()->ThrowTime,
			[&] { return PlayAnimMontage(PoseMontages->PrimaryActionMontage); },
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
		log(Display, "OnGrenadeActionFinished()")
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

	// TODO: Handle other types of nades if required
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		GetWeaponInfo()->Damage,
		GetActorLocation(),
		Explosion->Radius,
		UDamageType::StaticClass(),
		IgnoredActors,
		this,
		nullptr,
		false,
		ECC_Visibility);
	Explosion->FireImpulse();

	if (let ExplodeSounds = GetWeaponInfo()->Sounds.ExplodeSounds; ExplodeSounds.Num() > 0)
	{
		UCloud9SoundPlayer::PlayRandomSound(ExplodeSounds, Location, Settings->Volume);
	}

	if (Settings->bIsDrawExplosionSpheres)
	{
		constexpr int SphereSegments = 32;
		constexpr int SphereLifeTime = 4.0f;
		constexpr int SphereThickness = 1.0f;

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 0.5f,
			SphereSegments,
			FColor::Red,
			false,
			SphereLifeTime,
			0,
			SphereThickness
		);

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 0.75f,
			SphereSegments,
			FColor::Yellow,
			false,
			SphereLifeTime,
			0,
			SphereThickness
		);

		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),
			Explosion->Radius * 1.0f,
			SphereSegments,
			FColor::Green,
			false,
			SphereLifeTime,
			0,
			SphereThickness
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

	log(Display, "OnGrenadeThrown()")

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

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Character is invalid", *GetName());
		return false;
	}

	let Controller = Character->GetCloud9Controller();

	if (not IsValid(Controller))
	{
		log(Error, "[Weapon='%s'] Controller is invalid", *GetName());
		return false;
	}

	let Inventory = Character->GetInventory();

	if (not IsValid(Inventory))
	{
		log(Error, "[Weapon='%s'] Inventory is invalid", *GetName());
		return false;
	}

	FHitResult CursorHit;
	if (not Controller->GetHitResultUnderCursor(ECC_Visibility, true, CursorHit))
	{
		log(Error, "[Weapon='%s'] Cursor not hit anything", *GetName());
		return false;
	}

	let CommonData = WeaponDefinition.GetCommonData();
	Inventory->DropWeapon(
		GetWeaponSlot(),
		CursorHit.Location,
		CommonData->Grenade.MaxThrowAngle,
		CommonData->Grenade.MaxThrowImpulse);

	return true;
}

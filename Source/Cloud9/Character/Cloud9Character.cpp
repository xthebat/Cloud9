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

#include "Cloud9Character.h"

#include "Cloud9AIController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Materials/Material.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"

#include "Cloud9/Tools/Extensions/AActor.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Contollers//Cloud9PlayerController.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Character/Effects/Cloud9CharacterEffectTrait.h"
#include "Cloud9/Modes/Cloud9GameMode.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Tools/Extensions/ACharacter.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"
#include "Components/Cloud9InventoryComponent.h"
#include "Components/Cloud9CharacterMovement.h"
#include "Components/Cloud9SpringArmComponent.h"
#include "Components/Cloud9HealthComponent.h"
#include "Components/Cloud9AnimationComponent.h"
#include "Components/Cloud9EffectsComponent.h"
#include "Components/Cloud9SkeletalMeshComponent.h"
#include "GameFramework/PlayerState.h"

const FName ACloud9Character::SpringArmComponentName = TEXT("CameraBoom");
const FName ACloud9Character::CameraComponentName = TEXT("TopDownCamera");
const FName ACloud9Character::DecalComponentName = TEXT("CursorToWorld");
const FName ACloud9Character::InventoryComponentName = TEXT("InventoryComponent");
const FName ACloud9Character::EffectsComponentName = TEXT("EffectsComponent");
const FName ACloud9Character::HealthComponentName = TEXT("HealthComponent");
const FName ACloud9Character::AnimationComponentName = TEXT("AnimationComponent");
const FName ACloud9Character::WidgetInteractionComponentName = TEXT("WidgetInteractionComponent");

ACloud9Character::ACloud9Character(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
	.SetDefaultSubobjectClass<UCloud9CharacterMovement>(CharacterMovementComponentName)
	.SetDefaultSubobjectClass<UCloud9SkeletalMeshComponent>(MeshComponentName))
{
	AIControllerClass = ACloud9AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bNeedInitialize = true;

	DestroyAfterTime = DefaultDestroyAfterTime;

	let MyCapsuleComponent = GetCapsuleComponent();
	MyCapsuleComponent->InitCapsuleSize(CapsuleRadius, CapsuleHeight / 2.0f);
	MyCapsuleComponent->CanCharacterStepUpOn = CanStepUpOn;

	let Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;
	Movement->JumpZVelocity = JumpZVelocity;

	CameraBoom = CreateDefaultSubobject<UCloud9SpringArmComponent>(SpringArmComponentName);
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level
	CameraBoom->SetWorldRotation({CameraBoomYaw, 0.0f, 0.0f});

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	USkeletalMeshComponent* MyMesh = GetMesh();
	MyMesh->SetRelativeLocation({0.0f, 0.0f, -MyCapsuleComponent->GetScaledCapsuleHalfHeight()});
	MyMesh->SetRelativeRotation({0.0f, MeshRotationYaw, 0.0f});

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>(DecalComponentName);
	CursorToWorld->SetupAttachment(RootComponent);
	CursorSize = {16.0f, 32.0f, 32.0f};
	CursorToWorld->SetRelativeLocation({
			0.0f,
			0.0f,
			-MyCapsuleComponent->GetScaledCapsuleHalfHeight()
		}
	);
	CursorToWorld->SetRelativeRotation({CrosshairRotationPitch, 0.0, 0.0f});

	InventoryComponent = CreateDefaultSubobject<UCloud9InventoryComponent>(InventoryComponentName);
	HealthComponent = CreateDefaultSubobject<UCloud9HealthComponent>(HealthComponentName);
	EffectsComponent = CreateDefaultSubobject<UCloud9EffectsComponent>(EffectsComponentName);
	AnimationComponent = CreateDefaultSubobject<UCloud9AnimationComponent>(AnimationComponentName);
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(WidgetInteractionComponentName);
	WidgetInteractionComponent->SetupAttachment(RootComponent);
	WidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::Custom;
	WidgetInteractionComponent->bShowDebug = false;

	HealthComponent->OnCharacterDie.AddDynamic(this, &ACloud9Character::OnCharacterDie);

	// Activate ticking to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

UCloud9CharacterMovement* ACloud9Character::GetCloud9CharacterMovement() const
{
	let Movement = GetCharacterMovement();
	return IsValid(Movement) ? Cast<UCloud9CharacterMovement>(Movement) : nullptr;
}

ACloud9PlayerController* ACloud9Character::GetCloud9Controller() const
{
	return IsValid(Controller) ? Cast<ACloud9PlayerController>(Controller) : nullptr;
}

bool ACloud9Character::CanSneak() const
{
	if (let Movement = GetCloud9CharacterMovement(); IsValid(Movement))
	{
		return not Movement->IsCrouching();
	}

	return false;
}

void ACloud9Character::Sneak() const
{
	if (let Movement = GetCloud9CharacterMovement(); IsValid(Movement))
	{
		Movement->Sneak();
	}
}

void ACloud9Character::UnSneak() const
{
	if (let Movement = GetCloud9CharacterMovement(); IsValid(Movement))
	{
		Movement->UnSneak();
	}
}

void ACloud9Character::SetViewDirection(const TOptional<FHitResult>& HitResult)
{
	if (HitResult)
	{
		if (IsValid(CursorToWorld))
		{
			let ImpactNormal = HitResult->ImpactNormal;
			let ImpactRotation = ImpactNormal.Rotation();

			if (HitResult->GetActor() != this)
			{
				CursorToWorld->SetWorldLocation(HitResult->Location);
				CursorToWorld->SetWorldRotation(ImpactRotation);
				SetCursorIsHidden(false);
			}
			else
			{
				SetCursorIsHidden(true);
			}
		}

		static let Settings = UCloud9DeveloperSettings::Get();

		let StartLocation = GetMesh()->GetBoneLocation(CameraTargetBoneName, EBoneSpaces::WorldSpace);

		if (Settings->IsDrawHitCursorLine)
		{
			DrawDebugLine(
				GetWorld(),
				StartLocation,
				HitResult->Location,
				FColor::Green,
				false,
				0.0);
		}

		if (Settings->IsDrawDeprojectedCursorLine)
		{
			FVector WorldLocation;
			FVector WorldDirection;
			FVector2D MousePosition;

			GetCloud9Controller()->GetMousePosition(MousePosition.X, MousePosition.Y);
			GetCloud9Controller()->DeprojectScreenPositionToWorld(
				MousePosition.X,
				MousePosition.Y,
				WorldLocation,
				WorldDirection);

			DrawDebugLine(
				GetWorld(),
				StartLocation,
				WorldLocation,
				FColor::Red,
				false,
				0.0);
		}

		let TargetLocation = HitResult->Location;
		let LookRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		ViewVerticalRotation = LookRotation.Pitch;
		GetCloud9CharacterMovement()->Rotate({0.0f, LookRotation.Yaw, 0.0f});
	}
}

void ACloud9Character::SetDestroyAfter(float NewTime)
{
	DestroyAfterTime = NewTime;
}

UWidgetInteractionComponent* ACloud9Character::GetWidgetInteractionComponent() const
{
	return WidgetInteractionComponent;
}

void ACloud9Character::SetCameraRotationYaw(float Angle) const
{
	var Rotation = CameraBoom->GetRelativeRotation();
	Rotation.Yaw = Angle;
	CameraBoom->SetRelativeRotation(Rotation);
}

void ACloud9Character::AddCameraRotationYaw(float Angle) const
{
	CameraBoom->AddRelativeRotation({0.0f, Angle, 0.0f});
}

float ACloud9Character::GetCameraRotationRoll() const
{
	let Rotation = CameraBoom->GetRelativeRotation();
	return -Rotation.Pitch;
}

void ACloud9Character::SetCameraRotationRoll(float Angle) const
{
	var Rotation = CameraBoom->GetRelativeRotation();
	Rotation.Pitch = -Angle;
	CameraBoom->SetRelativeRotation(Rotation);
}

void ACloud9Character::SetCursorIsHidden(bool Hidden) const
{
	CursorToWorld->SetHiddenInGame(Hidden or not IsValid(CursorDecal));
}

float ACloud9Character::GetCameraZoomHeight() const
{
	return CameraBoom->TargetArmLength;
}

void ACloud9Character::SetCameraZoomHeight(float Value) const
{
	CameraBoom->TargetArmLength = Value;
}

bool ACloud9Character::GetNeedInitialize() const
{
	return bNeedInitialize;
}

UCloud9InventoryComponent* ACloud9Character::GetInventoryComponent() const { return InventoryComponent; }

UCloud9HealthComponent* ACloud9Character::GetHealthComponent() const { return HealthComponent; }

UCloud9AnimationComponent* ACloud9Character::GetAnimationComponent() const { return AnimationComponent; }

// ReSharper disable once CppMemberFunctionMayBeConst
UCloud9CharacterEffectTrait* ACloud9Character::AddCharacterEffect(TSubclassOf<UCloud9CharacterEffectTrait> EffectClass)
{
	return IsValid(EffectClass) ? EffectsComponent->AddEffect(EffectClass) : nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool ACloud9Character::RemoveCharacterEffect(UCloud9CharacterEffectTrait* Effect)
{
	return EffectsComponent->RemoveEffect(Effect);
}

void ACloud9Character::AddScore() const
{
	if (let MyPlayerState = GetPlayerState())
	{
		let NewScore = MyPlayerState->GetScore() + 1;
		MyPlayerState->SetScore(NewScore);
		OnScoreChanged.Broadcast(NewScore);
	}
}

void ACloud9Character::UseObject()
{
	// TODO: Implement UseObject
}

void ACloud9Character::OnCharacterDie(AActor* Actor)
{
	if (let MyCapsuleComponent = GetCapsuleComponent(); IsValid(MyCapsuleComponent))
	{
		MyCapsuleComponent->DestroyComponent();
	}

	// TODO: Clean up timers tethered with character
	// TODO: Drop the most expensive weapon

	let MyMesh = GetMesh();
	MyMesh->SetMobility(EComponentMobility::Movable);
	MyMesh->SetSimulatePhysics(true);
	MyMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MyMesh->SetCollisionProfileName(COLLISION_PROFILE_RAGDOLL);
	this | EAActor::DestroyAfter{DestroyAfterTime};
}

float ACloud9Character::InternalTakePointDamage(
	float Damage,
	const FPointDamageEvent& PointDamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	if (GetHealthComponent()->IsInvulnerable())
	{
		return 0.0f;
	}

	static let Settings = UCloud9DeveloperSettings::Get();

	Damage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	// TODO: Scale will be used when implement grenade damage
	float Scale = 1.0f;
	float FlinchModLarge = 0.0f;
	float FlinchModSmall = 0.0f;

	if (let WeaponFirearm = Cast<ACloud9WeaponFirearm>(DamageCauser))
	{
		let BoneName = PointDamageEvent.HitInfo.BoneName;
		let WeaponInfo = WeaponFirearm->GetWeaponInfo();

		bool HitInArmor = false;

		if (HeadBoneNames.Contains(BoneName))
		{
			Damage *= WeaponInfo->HeadshotMultiplier;
			HitInArmor = HealthComponent->HasHelmet();
		}
		else if (UpperBodyBoneNames.Contains(BoneName))
		{
			Damage *= WeaponInfo->UpperBodyMultiplier;
			HitInArmor = HealthComponent->IsArmored();
		}
		else if (LowerBodyBoneNames.Contains(BoneName))
		{
			Damage *= WeaponInfo->LowerBodyMultiplier;
		}
		else if (LegsBoneNames.Contains(BoneName))
		{
			Damage *= WeaponInfo->LegMultiplier;
		}

		let Distance = FVector::DistSquared(DamageCauser->GetActorLocation(), GetActorLocation());
		let RangeCoefficient = FMath::Pow(
			WeaponInfo->RangeModifier,
			Distance * ACloud9WeaponFirearm::RangeExponentCoefficient);

		var DamageToHealth = Damage * RangeCoefficient;
		var DamageToArmor = 0.0f;

		if (HitInArmor)
		{
			DamageToHealth = Damage * WeaponInfo->ArmorPenetration * ACloud9WeaponFirearm::ArmorCoefficient;
			DamageToArmor = (Damage - DamageToHealth) * ACloud9WeaponFirearm::ArmorBonus;

			// Does this use more armor than we have?
			if (let ArmorValue = HealthComponent->GetArmor(); DamageToArmor > ArmorValue)
			{
				DamageToHealth = Damage - ArmorValue / ACloud9WeaponFirearm::ArmorBonus;
				DamageToArmor = ArmorValue;
			}

			HealthComponent->TakeArmorDamage(DamageToArmor < 0 ? 1.0f : DamageToArmor);
			Damage = DamageToHealth;
		}

		if (Settings->WeaponDebugDamageInfo)
		{
			log(Display,
			    "[Actor='%s'] Distance=%f RangeCoefficient=%f BoneName=%s Armored=%d DamageToHealth=%f DamageToArmor=%f",
			    *GetName(),
			    FMath::Sqrt(Distance),
			    RangeCoefficient,
			    *BoneName.ToString(),
			    HitInArmor,
			    DamageToHealth,
			    DamageToArmor);
		}

		FlinchModLarge = WeaponInfo->GetFlinchVelocityModifierLarge();
		FlinchModSmall = WeaponInfo->GetFlinchVelocityModifierSmall();
	}
	else if (let WeaponGrenade = Cast<ACloud9WeaponGrenade>(DamageCauser))
	{
		let WeaponInfo = WeaponGrenade->GetWeaponInfo();
		FlinchModLarge = WeaponInfo->GetFlinchVelocityModifierLarge();
		FlinchModSmall = WeaponInfo->GetFlinchVelocityModifierSmall();

		Scale = 1.0f - Damage / 40.0f;
		FlinchModLarge += Scale * 1.05;
		FlinchModLarge = FMath::Min(1.5f, FlinchModLarge);
	}

	if (Damage > 0.0f)
	{
		GetCloud9CharacterMovement()->UpdateFlinchVelocityModifier(FlinchModSmall, FlinchModLarge);
	}

	return Damage;
}

void ACloud9Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	let Rotator = Transform.Rotator();

	SetCameraRotationYaw(Rotator.Yaw);
	GetCloud9CharacterMovement()->Rotate({0.0f, Rotator.Yaw, 0.0f}, true);

	SetCursorIsHidden(true);

	if (IsValid(CursorDecal))
	{
		CursorToWorld->SetDecalMaterial(CursorDecal);
		CursorToWorld->DecalSize = CursorSize;
	}

	if (let MyMesh = GetMesh(); IsValid(MyMesh))
	{
		if (not CameraTargetBoneName.IsNone())
		{
			let HeadBoneLocation = MyMesh->GetBoneLocation(CameraTargetBoneName, EBoneSpaces::WorldSpace);
			log(Display, "Setup CameraBoom = %s", *HeadBoneLocation.ToString());
			CameraBoom->SetWorldLocation(HeadBoneLocation);
		}

		MyMesh->bCastDynamicShadow = true;
		MyMesh->bAffectDynamicIndirectLighting = true;
		MyMesh->SetCollisionProfileName(COLLISION_PROFILE_HITBOX);

#ifdef USE_PHYSICAL_ASSET_HITBOX
		// TODO: Make same hit boxes for all character's type - currently disabled
		let PhysicsAsset = MyMesh->GetPhysicsAsset();
		for (let BodySetup : PhysicsAsset->SkeletalBodySetups)
		{
			let AggGeom = BodySetup->AggGeom;
			for (let& Capsule : AggGeom.SphylElems)
			{
				let HitBox = NewObject<UCapsuleComponent>(
					MyMesh,
					UCapsuleComponent::StaticClass(),
					Capsule.GetName());
		
				HitBox->RegisterComponent();
				HitBox->AttachToComponent(MyMesh, FAttachmentTransformRules::KeepRelativeTransform, Capsule.GetName());
		
				HitBox->SetCapsuleSize(
					Capsule.GetScaledRadius(Transform.GetScale3D()),
					Capsule.GetScaledHalfLength(Transform.GetScale3D()));
				HitBox->SetRelativeTransform(Capsule.GetTransform());
				HitBox->SetCollisionProfileName(TRACE_HITBOX);
				HitBox->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		
				log(Error, "Hitbox registered = %s [%f]", *HitBox->GetName(), HitBox->GetUnscaledCapsuleRadius());
			}
		}
#endif
	}
}

void ACloud9Character::BeginPlay()
{
	Super::BeginPlay();

	if (not IsPlayerControlled())
	{
		CursorToWorld->Deactivate();
		TopDownCameraComponent->Deactivate();
		CameraBoom->Deactivate();
	}

	log(Display, "[Character='%s'] IsPlayer=%d", *GetName(), IsPlayerControlled());

	// Load all characters (even it's a bot) because LoadCharacter also handle GameMode initialization
	if (let GameMode = GetWorld() | EUWorld::GetGameMode<ACloud9GameMode>{})
	{
		GameMode->LoadCharacter(this);
	}
}

void ACloud9Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACloud9Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Auto select any available weapon if nothing selected
	if (not InventoryComponent->GetSelectedWeapon() and not InventoryComponent->IsEmpty())
	{
		InventoryComponent->SelectOtherAvailableWeapon(false);
	}
}

void ACloud9Character::OnLevelChanged() const
{
	if (let GameMode = GetWorld() | EUWorld::GetGameMode<ACloud9GameMode>{};
		IsValid(GameMode) and IsPlayerControlled())
	{
		GameMode->SaveCharacter(this);
	}
}

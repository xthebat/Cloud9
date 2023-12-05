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

#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Cloud9CharacterMovement.h"
#include "Components/Cloud9Inventory.h"
#include "Components/Cloud9SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

class UCloud9SpringArmComponent;
const FName ACloud9Character::SpringArmComponentName = TEXT("CameraBoom");
const FName ACloud9Character::CameraComponentName = TEXT("TopDownCamera");
const FName ACloud9Character::DecalComponentName = TEXT("CursorToWorld");
const FName ACloud9Character::InventoryComponentName = TEXT("Inventory");

ACloud9Character::ACloud9Character(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UCloud9CharacterMovement>(CharacterMovementComponentName))
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(32.f, 72.0f);

	let Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;
	Movement->JumpZVelocity = 320.0f;

	CameraBoom = CreateDefaultSubobject<UCloud9SpringArmComponent>(SpringArmComponentName);
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>(DecalComponentName);
	CursorToWorld->SetupAttachment(RootComponent);

	Inventory = CreateDefaultSubobject<UCloud9Inventory>(InventoryComponentName);


	// Activate ticking in order to update the cursor every frame.
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

void ACloud9Character::SetViewDirection(const FHitResult& HitResult, bool bIsHitValid) const
{
	if (IsValid(CursorToWorld))
	{
		let ImpactNormal = HitResult.ImpactNormal;
		let ImpactRotation = ImpactNormal.Rotation();

		CursorToWorld->SetWorldLocation(HitResult.Location);
		CursorToWorld->SetWorldRotation(ImpactRotation);
		SetCursorIsHidden(false);
	}

	let Settings = UCloud9DeveloperSettings::Get();

	if (Settings->bIsDrawHitCursorLine)
	{
		DrawDebugLine(
			GetWorld(),
			GetActorLocation(),
			HitResult.Location,
			FColor::Green,
			false,
			0.0);
	}

	if (Settings->bIsDrawDeprojectedCursorLine)
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
			GetActorLocation(),
			WorldLocation,
			FColor::Red,
			false,
			0.0);
	}

	if (bIsHitValid)
	{
		let TargetLocation = FVector{HitResult.Location.X, HitResult.Location.Y, 0.0f};
		let ActorLocation = GetActorLocation();
		let LookRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
		GetCloud9CharacterMovement()->Rotate({0.0f, LookRotation.Yaw, 0.0f});
	}
}

void ACloud9Character::SetCameraRotationYaw(float Angle) const
{
	var Rotation = CameraBoom->GetRelativeRotation();
	Rotation.Yaw = Angle;
	log(Display, "SetRelativeRotation Pitch: %s", *Rotation.ToString());
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
	log(Display, "SetRelativeRotation Yaw: %s", *Rotation.ToString());
	CameraBoom->SetRelativeRotation(Rotation);
}

void ACloud9Character::SetCursorIsHidden(bool Hidden) const
{
	CursorToWorld->SetHiddenInGame(Hidden || !IsValid(CursorDecal));
}

float ACloud9Character::GetCameraZoomHeight() const
{
	return CameraBoom->TargetArmLength;
}

void ACloud9Character::SetCameraZoomHeight(float Value) const
{
	CameraBoom->TargetArmLength = Value;
}

UCloud9Inventory* ACloud9Character::GetInventory() const { return Inventory; }

void ACloud9Character::UseObject()
{
	// TODO: Implement UseObject
}

void ACloud9Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	log(Display, "Contruction transform %s", *Transform.ToString());

	let Rotator = Transform.Rotator();

	SetCameraRotationYaw(Rotator.Yaw);
	GetCloud9CharacterMovement()->Rotate({0.0f, Rotator.Yaw, 0.0f}, true);

	SetCursorIsHidden(true);

	if (IsValid(CursorDecal))
	{
		log(Display, "Setup CursorDecal = %s", *CursorDecal->GetName());
		CursorToWorld->SetDecalMaterial(CursorDecal);
		CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	}

	if (let MyMesh = GetMesh(); IsValid(MyMesh) && not CameraTargetBoneName.IsNone())
	{
		let HeadBoneLocation = MyMesh->GetBoneLocation(CameraTargetBoneName, EBoneSpaces::WorldSpace);
		log(Display, "Setup CameraBoom = %s", *HeadBoneLocation.ToString());
		CameraBoom->SetWorldLocation(HeadBoneLocation);

		MyMesh->bCastDynamicShadow = true;
		MyMesh->bAffectDynamicIndirectLighting = true;
	}
}

void ACloud9Character::BeginPlay()
{
	Super::BeginPlay();

	// FTransform Transform = FTransform::Identity;
	// Transform.SetLocation({50, 329, -330});
	// GetGameInstance<UCloud9GameInstance>()->SpawnFirearmWeapon(EFirearm::Deagle);
}

void ACloud9Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

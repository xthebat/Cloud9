// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cloud9Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Cloud9CharacterMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

ACloud9Character::ACloud9Character(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UCloud9CharacterMovement>(CharacterMovementComponentName))
{
	

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	const auto Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true; // Rotate character to moving direction
	Movement->RotationRate = FRotator(0.f, 640.f, 0.f);
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;
	Movement->JumpZVelocity = 320.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

const UCloud9CharacterMovement* ACloud9Character::GetMyCharacterMovement() const
{
	const auto Movement = GetCharacterMovement();
	if (Movement == nullptr)
		return nullptr;

	return Cast<UCloud9CharacterMovement>(Movement);
}

const ACloud9PlayerController* ACloud9Character::GetMyPlayerController() const
{
	const auto MyController = GetController();
	if (MyController == nullptr)
		return nullptr;

	return Cast<ACloud9PlayerController>(MyController);
}

bool ACloud9Character::CanSneak() const
{
	return !GetMyCharacterMovement()->IsCrouching();
}

void ACloud9Character::Sneak() const
{
	if (const auto Movement = GetMyCharacterMovement())
		Movement->Sneak();
}

void ACloud9Character::UnSneak() const
{
	if (const auto Movement = GetMyCharacterMovement())
		Movement->UnSneak();
}

void ACloud9Character::SetViewDirection(const FHitResult& HitResult)
{
	if (CursorToWorld == nullptr)
		return;
	
	const auto Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location);
	
	CursorToWorld->SetWorldLocation(HitResult.Location);

	const auto ImpactNormal = HitResult.ImpactNormal;
	const auto ImpactRotation = ImpactNormal.Rotation();
	CursorToWorld->SetWorldRotation(ImpactRotation);

	SetActorRotation({0.0f, Rotation.Yaw, 0.0f});
}

void ACloud9Character::AddCameraRotationYaw(float Angle) const
{
	const FRotator Rotation = {0.0f, Angle, 0.0f};
	CameraBoom->AddRelativeRotation(Rotation);
}

void ACloud9Character::SetCameraRotationRoll(float Angle) const
{
	const FRotator Rotation = {-Angle, 0.0f, 0.0f};
	CameraBoom->SetRelativeRotation(Rotation);
}

void ACloud9Character::SetCursorIsHidden(bool Hidden) const
{
	CursorToWorld->bHiddenInGame = Hidden;
}

float ACloud9Character::GetCameraZoom() const
{
	return CameraBoom->TargetArmLength;
}

void ACloud9Character::SetCameraZoom(float Value) const
{
	CameraBoom->TargetArmLength = Value;
}

void ACloud9Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CursorDecal != nullptr)
	{
		CursorToWorld->SetDecalMaterial(CursorDecal);
		CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	}

	if (IsValid(GetMesh()))
	{
		if (!CameraTargetBoneName.IsNone())
		{
			const auto HeadBoneLocation = GetMesh()->GetBoneLocation(CameraTargetBoneName, EBoneSpaces::WorldSpace);
			CameraBoom->SetWorldLocation(HeadBoneLocation);		
		}

		const auto Box = UCloud9ToolsLibrary::GetAccurateReferencePoseBounds(GetMesh()->SkeletalMesh);

		UE_LOG(LogCloud9, Display, TEXT("Box = %s GetMesh()->Bounds = %s"), *Box.ToString(), *GetMesh()->Bounds.ToString());
		
		float Width = 0.0f, Height = 0.0f, Depth = 0.0f;
		UCloud9ToolsLibrary::GetWidthHeightDepth(GetMesh()->Bounds.GetBox(), Width, Height, Depth);
		// GetCapsuleComponent()->InitCapsuleSize(32.f, 72.0f);
		GetCapsuleComponent()->InitCapsuleSize(Width, Height);
	}
}

void ACloud9Character::SelectWeapon(int NewWeapon)
{
	PendingWeapon = NewWeapon;	
}

int ACloud9Character::GetSelectedWeapon()
{
	return SelectedWeapon;
}

int ACloud9Character::GetPendingWeapon()
{
	return PendingWeapon;
}

void ACloud9Character::OnWeaponChangeFinished()
{
	SelectedWeapon = PendingWeapon;
}

bool ACloud9Character::IsWeaponChanging()
{
	return SelectedWeapon != PendingWeapon;
}

void ACloud9Character::OnPoseUpdated()
{
	UE_LOG(LogCloud9, Display, TEXT("GetMesh()->Bounds = %s"), *GetMesh()->Bounds.ToString());
}

void ACloud9Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

#include "Cloud9Character.h"

#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Cloud9CharacterMovement.h"
#include "Components/Cloud9Inventory.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

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

	const auto Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;
	Movement->JumpZVelocity = 320.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(SpringArmComponentName);
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
	if (const auto MyCharacterMovement = GetCharacterMovement())
	{
		return Cast<UCloud9CharacterMovement>(MyCharacterMovement);
	}

	return nullptr;
}

ACloud9PlayerController* ACloud9Character::GetCloud9Controller() const
{
	if (const auto MyController = GetController())
	{
		return Cast<ACloud9PlayerController>(MyController);
	}

	return nullptr;
}

bool ACloud9Character::CanSneak() const { return !GetCloud9CharacterMovement()->IsCrouching(); }

void ACloud9Character::Sneak() const
{
	if (const auto Movement = GetCloud9CharacterMovement())
	{
		Movement->Sneak();
	}
}

void ACloud9Character::UnSneak() const
{
	if (const auto Movement = GetCloud9CharacterMovement())
	{
		Movement->UnSneak();
	}
}

void ACloud9Character::SetViewDirection(const FHitResult& HitResult, bool bIsHitValid) const
{
	if (IsValid(CursorToWorld))
	{
		const auto ImpactNormal = HitResult.ImpactNormal;
		const auto ImpactRotation = ImpactNormal.Rotation();

		CursorToWorld->SetWorldLocation(HitResult.Location);
		CursorToWorld->SetWorldRotation(ImpactRotation);
		SetCursorIsHidden(false);
	}

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
		/*LifeTime=*/0.0,
		/*DepthPriority=*/0,
		/*Thickness=*/0.f);

	DrawDebugLine(
		GetWorld(),
		GetActorLocation(),
		HitResult.Location,
		FColor::Green,
		false,
		/*LifeTime=*/0.0,
		/*DepthPriority=*/0,
		/*Thickness=*/0.f);

	// UE_LOG(LogCloud9, Display, TEXT("%s"), *HitResult.Location.ToString());

	if (bIsHitValid)
	{
		const auto TargetLocation = FVector{HitResult.Location.X, HitResult.Location.Y, 0.0f};
		const auto ActorLocation = GetActorLocation();
		const auto LookRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
		GetCloud9CharacterMovement()->Rotate({0.0f, LookRotation.Yaw, 0.0f});
	}
}

void ACloud9Character::SetCameraRotationYaw(float Angle) const
{
	auto Rotation = CameraBoom->GetRelativeRotation();
	Rotation.Yaw = Angle;
	UE_LOG(LogCloud9, Display, TEXT("SetRelativeRotation Pitch: %s"), *Rotation.ToString());
	CameraBoom->SetRelativeRotation(Rotation);
}

void ACloud9Character::AddCameraRotationYaw(float Angle) const
{
	const FRotator Rotation = {0.0f, Angle, 0.0f};
	CameraBoom->AddRelativeRotation(Rotation);
}

float ACloud9Character::GetCameraRotationRoll() const
{
	const auto Rotation = CameraBoom->GetRelativeRotation();
	return -Rotation.Pitch;
}

void ACloud9Character::SetCameraRotationRoll(float Angle) const
{
	auto Rotation = CameraBoom->GetRelativeRotation();
	Rotation.Pitch = -Angle;
	UE_LOG(LogCloud9, Display, TEXT("SetRelativeRotation Yaw: %s"), *Rotation.ToString());
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

void ACloud9Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UE_LOG(LogCloud9, Display, TEXT("Contruction transform %s"), *Transform.ToString());

	const auto Rotator = Transform.Rotator();

	SetCameraRotationYaw(Rotator.Yaw);
	GetCloud9CharacterMovement()->Rotate({0.0f, Rotator.Yaw, 0.0f}, true);

	SetCursorIsHidden(true);

	if (CursorDecal != nullptr)
	{
		UE_LOG(LogCloud9, Display, TEXT("Setup CursorDecal = %s"), *CursorDecal->GetName());
		CursorToWorld->SetDecalMaterial(CursorDecal);
		CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	}

	if (GetMesh() != nullptr && !CameraTargetBoneName.IsNone())
	{
		const auto HeadBoneLocation = GetMesh()->GetBoneLocation(CameraTargetBoneName, EBoneSpaces::WorldSpace);
		UE_LOG(LogCloud9, Display, TEXT("Setup CameraBoom = %s"), *HeadBoneLocation.ToString());
		CameraBoom->SetWorldLocation(HeadBoneLocation);
	}
}

void ACloud9Character::BeginPlay()
{
	Super::BeginPlay();
}

void ACloud9Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

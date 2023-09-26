#include "Cloud9WeaponBase.h"

#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

ACloud9WeaponBase::ACloud9WeaponBase()
{
	bCanBeDropped = true;
	WeaponType = EWeaponType::NoWeapon;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(CapsuleComponentName);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;
	
	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(MeshComponentName);
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Mesh->SetupAttachment(CapsuleComponent);
		Mesh->SetCollisionProfileName(MeshCollisionProfile);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}
}

void ACloud9WeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACloud9WeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (Mesh)
	{
		float Width = 0.0f, Height = 0.0f, Depth = 0.0f;
		UCloud9ToolsLibrary::GetWidthHeightDepth(Mesh->Bounds.GetBox(), Width, Height, Depth);
		CapsuleComponent->InitCapsuleSize(Depth, Width);		
	}
}

bool ACloud9WeaponBase::CanBeDropped() const { return bCanBeDropped; }

EWeaponType ACloud9WeaponBase::GetWeaponType() const { return WeaponType; }

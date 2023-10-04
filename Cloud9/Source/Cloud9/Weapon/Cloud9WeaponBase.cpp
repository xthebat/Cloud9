#include "Cloud9WeaponBase.h"

#include "Cloud9/Character/Cloud9Character.h"

const FName ACloud9WeaponBase::CapsuleComponentName = TEXT("CapsuleComponent");
const FName ACloud9WeaponBase::MeshComponentName = TEXT("MeshComponent");
const FName ACloud9WeaponBase::MeshCollisionProfile = TEXT("WeaponMesh");
const FName ACloud9WeaponBase::WeaponSocketName = TEXT("WeaponSocket");

ACloud9WeaponBase::ACloud9WeaponBase()
{
	bCanBeDropped = true;
	WeaponType = EWeaponType::NoWeapon;
	SocketTransform = FTransform::Identity;

	Mesh = CreateOptionalDefaultSubobject<UStaticMeshComponent>(MeshComponentName);
	
	if (Mesh)
	{
		RootComponent = Mesh;
		
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
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

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	if (const auto MyOwner = Cast<ACloud9Character>(GetOwner()))
	{
		const auto ParentMesh = MyOwner->GetMesh();
		AttachToComponent(ParentMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		SetActorRelativeTransform(SocketTransform);
	}
}

bool ACloud9WeaponBase::CanBeDropped() const { return bCanBeDropped; }

EWeaponType ACloud9WeaponBase::GetWeaponType() const { return WeaponType; }

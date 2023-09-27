#include "Cloud9WeaponBase.h"

ACloud9WeaponBase::ACloud9WeaponBase()
{
	bCanBeDropped = true;
	WeaponType = EWeaponType::NoWeapon;

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
}

bool ACloud9WeaponBase::CanBeDropped() const { return bCanBeDropped; }

EWeaponType ACloud9WeaponBase::GetWeaponType() const { return WeaponType; }

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

#include "Cloud9WeaponBase.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Character/Cloud9Character.h"

const FName ACloud9WeaponBase::CapsuleComponentName = TEXT("CapsuleComponent");
const FName ACloud9WeaponBase::MeshComponentName = TEXT("MeshComponent");
const FName ACloud9WeaponBase::MuzzleFlashComponentName = TEXT("MuzzleFlashComponent");
const FName ACloud9WeaponBase::MeshCollisionProfile = TEXT("WeaponMesh");
const FName ACloud9WeaponBase::EquippedWeaponSocketName = TEXT("EquippedWeaponSocket");
const FName ACloud9WeaponBase::HolsteredPrimaryWeaponSocketName = TEXT("HolsteredPrimaryWeaponSocket");
const FName ACloud9WeaponBase::HolsteredSecondaryWeaponSocketName = TEXT("HolsteredSecondaryWeaponSocket");
const FName ACloud9WeaponBase::HolsteredGrenadeWeaponSocketName = TEXT("HolsteredGrenadeWeaponSocket");

ACloud9WeaponBase::ACloud9WeaponBase()
{
	WeaponClass = EWeaponClass::NoClass;
	WeaponType = EWeaponType::NoWeapon;
	bHasSecondaryAction = false;
	bCanBeDropped = true;
	bIsAutomatic = false;
	
	PrimaryActionCooldown = 0.0;
	SecondaryActionCooldown = 0.0;
	BaseDamage = 0.0;
	DeployTime = 0.0;

	Mesh = CreateOptionalDefaultSubobject<UStaticMeshComponent>(MeshComponentName);

	if (IsValid(Mesh))
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
	if (let MyOwner = Cast<ACloud9Character>(GetOwner()))
	{
		let ParentMesh = MyOwner->GetMesh();
		AttachToComponent(ParentMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, EquippedWeaponSocketName);
	}
}

bool ACloud9WeaponBase::CanBeDropped() const { return bCanBeDropped; }

EWeaponType ACloud9WeaponBase::GetWeaponType() const { return WeaponType; }

EWeaponClass ACloud9WeaponBase::GetWeaponClass() const { return WeaponClass; }

bool ACloud9WeaponBase::HasSecondaryAction() const { return bHasSecondaryAction; }

bool ACloud9WeaponBase::IsAutomatic() const { return bIsAutomatic; }

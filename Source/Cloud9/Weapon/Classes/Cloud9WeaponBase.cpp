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

#include "Cloud9/Tools/Components/CooldownActionComponent.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9AssetManager.h"
#include "Cloud9/Weapon/Assets/WeaponDefinitionsAsset.h"
#include "Cloud9/Weapon/Enums/WeaponAction.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Extensions/EWeaponId.h"

const FName ACloud9WeaponBase::RootComponentName = TEXT("RootComponent");
const FName ACloud9WeaponBase::WeaponMeshCollisionProfile = TEXT("WeaponCollisionProfile");
const FString ACloud9WeaponBase::ActionComponentFormat = TEXT("{0}ActionComponent");

const FName ACloud9WeaponBase::WeaponMeshComponentName = TEXT("WeaponMeshComponent");
const FName ACloud9WeaponBase::MagazineMeshComponentName = TEXT("MagazineMeshComponent");
const FName ACloud9WeaponBase::SilencerMeshComponentName = TEXT("SilencerMeshComponent");
const FName ACloud9WeaponBase::MuzzleFlashComponentName = TEXT("MuzzleFlashComponent");

const FName ACloud9WeaponBase::MagazineSocketName = TEXT("MagazineSocket");
const FName ACloud9WeaponBase::SilencerSocketName = TEXT("SilencerSocket");
const FName ACloud9WeaponBase::MuzzleFlashSocketName = TEXT("MuzzleFlashSocket");
const FName ACloud9WeaponBase::CaseEjectSocketName = TEXT("CaseEjectSocket");

ACloud9WeaponBase::ACloud9WeaponBase()
{
	// Required for weapon with automatic fire
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkin = NAME_None;

	if (WeaponMesh = CreateMeshComponent(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		log(Error, "Failed to create WeaponMeshComponent");
		return;
	}

	if (MagazineMesh = CreateMeshComponent(MagazineMeshComponentName, MagazineSocketName); not IsValid(MagazineMesh))
	{
		log(Error, "Failed to create MagazineMeshComponent");
		return;
	}

	if (MuzzleFlash = CreateEffectComponent(MuzzleFlashComponentName, MuzzleFlashSocketName); not IsValid(MuzzleFlash))
	{
		log(Error, "Failed to create MuzzleFlashComponent");
		return;
	}

	if (SilencerMesh = CreateMeshComponent(SilencerMeshComponentName, SilencerSocketName); not IsValid(SilencerMesh))
	{
		log(Error, "Failed to create SilencerMeshComponent");
		return;
	}

	let Actions = StaticEnum<EWeaponAction>();

	ETContainer::Repeat(
		Actions->NumEnums(),
		[&](let Id)
		{
			let CooldownActionName = Actions | EUEnum::GetValueName{Id};
			let ComponentName = FString::Format(*ActionComponentFormat, {CooldownActionName.ToString()});
			let CooldownActionComponent = CreateCooldownAction(FName(ComponentName));
			Executors.Add(CooldownActionComponent);
		}
	);
}

UWeaponDefinitionsAsset* ACloud9WeaponBase::GetWeaponDefinitionsAsset()
{
	// Without static won't work in packaged game
	static UWeaponDefinitionsAsset* WeaponDefinitionsAsset;

	if (IsValid(WeaponDefinitionsAsset))
	{
		return WeaponDefinitionsAsset;
	}

	let Asset = UCloud9AssetManager::GetOrLoadAssetSync<UWeaponDefinitionsAsset>();

	if (not IsValid(Asset))
	{
		// Try reload later then ... this will haven in PIE
		// if some object required this asset placed on level
		log(Error, "WeaponDefinitionsAsset loading failure");
		return nullptr;
	}

	WeaponDefinitionsAsset = Asset;

	return WeaponDefinitionsAsset;
}

UCooldownActionComponent* ACloud9WeaponBase::CreateCooldownAction(FName ComponentName)
{
	let Component = CreateDefaultSubobject<UCooldownActionComponent>(ComponentName);

	if (not IsValid(Component))
	{
		log(Error, "Can't cooldown action '%s' for actor '%s'", *ComponentName.ToString(), *GetName());
		return nullptr;
	}

	return Component;
}

UStaticMeshComponent* ACloud9WeaponBase::CreateMeshComponent(FName ComponentName, FName SocketName)
{
	if (let Component = CreateDefaultSubobject<UStaticMeshComponent>(ComponentName); IsValid(Component))
	{
		Component->bOwnerNoSee = false;
		Component->AlwaysLoadOnClient = true;
		Component->AlwaysLoadOnServer = true;
		Component->bCastDynamicShadow = true;
		Component->bAffectDynamicIndirectLighting = true;
		Component->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Component->SetCollisionProfileName(WeaponMeshCollisionProfile);

		if (not SocketName.IsNone())
		{
			Component->SetupAttachment(RootComponent, SocketName);
		}
		else
		{
			RootComponent = Component;
		}

		return Component;
	}

	log(Error, "Can't create mesh '%s' for actor '%s'", *ComponentName.ToString(), *GetName());
	return nullptr;
}

UNiagaraComponent* ACloud9WeaponBase::CreateEffectComponent(FName ComponentName, FName SocketName)
{
	if (let Component = CreateDefaultSubobject<UNiagaraComponent>(ComponentName); IsValid(Component))
	{
		Component->SetupAttachment(RootComponent, SocketName);
		return Component;
	}

	log(Error, "Can't create VFX '%s' for actor '%s'", *ComponentName.ToString(), *GetName());
	return nullptr;
}

void ACloud9WeaponBase::InitializeName(const FWeaponId& NewWeaponId)
{
	Visit(
		[&]<typename WeaponIdType>(WeaponIdType Id)
		{
			if constexpr (not TIsSame<WeaponIdType, EBadWeapon>::Value)
			{
				using WeaponClassType = typename EFWeaponId::WeaponClass<WeaponIdType>::Type;
				var This = static_cast<WeaponClassType*>(this);
				This->WeaponId = Id;
			}
		},
		NewWeaponId
	);
}

bool ACloud9WeaponBase::InitializeMeshComponent(
	UStaticMeshComponent* Component,
	UStaticMesh* Mesh,
	UMaterialInstance* Material) const
{
	if (Mesh == nullptr)
	{
		log(Error, "[Weapon='%s'] Mesh is invalid", *GetName());
		return false;
	}

	Component->SetStaticMesh(Mesh);

	if (Material != nullptr)
	{
		Component->SetMaterial(0, Material);
	}

	return true;
}

bool ACloud9WeaponBase::InitializeEffectComponent(UNiagaraComponent* Component, UNiagaraSystem* Effect) const
{
	if (Effect == nullptr)
	{
		log(Error, "[Weapon='%s'] Effect is invalid", *GetName());
		return false;
	}

	Component->SetAsset(Effect);
	Component->Deactivate();
	return true;
}

UAnimInstance* ACloud9WeaponBase::GetAnimInstance() const
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s'] Character is invalid", *GetName());
		return nullptr;
	}

	let Mesh = Character->GetMesh();

	if (not IsValid(Mesh))
	{
		log(Error, "[Weapon='%s'] Mesh is invalid", *GetName());
		return nullptr;
	}

	return Mesh->GetAnimInstance();
}

bool ACloud9WeaponBase::PlayAnimMontage(UAnimMontage* Montage, float Rate, float StartTime) const
{
	if (not IsValid(Montage))
	{
		log(Error, "[Weapon='%s'] Montage is invalid", *GetName());
		return false;
	}

	let AnimInstance = GetAnimInstance();

	if (not IsValid(AnimInstance))
	{
		log(Error, "[Weapon='%s'] AnimInstance is invalid for montage '%s'", *GetName(), *Montage->GetName());
		return false;
	}

	if (not AnimInstance->Montage_Play(Montage, Rate, EMontagePlayReturnType::MontageLength, StartTime))
	{
		log(Error, "[Weapon='%s'] Can't play montage '%s'", *GetName(), *Montage->GetName());
		return false;
	}

	return true;
}

bool ACloud9WeaponBase::IsAnyMontagePlaying() const
{
	let AnimInstance = GetAnimInstance();

	if (not IsValid(AnimInstance))
	{
		log(Error, "[Weapon='%s'] AnimInstance is invalid", *GetName());
		return false;
	}

	return AnimInstance->IsAnyMontagePlaying();
}

#define SLOT_NAME *UWeaponSlot::ToString(NewSlot)
#define BOND_NAME *UWeaponBond::ToString(NewBond)

bool ACloud9WeaponBase::UpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponBond NewBond, bool Instant)
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Weapon owner is invalid", *GetName(), SLOT_NAME);
		return false;
	}

	let CharacterMesh = Character->GetMesh();

	if (not IsValid(CharacterMesh))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Character mesh is invalid", *GetName(), SLOT_NAME);
		return false;
	}

	let SocketName = NewBond == EWeaponBond::Armed
		                 ? UWeaponSlot::EquippedSocket()
		                 : UWeaponSlot::HolsteredSocket(NewSlot);

	if (SocketName.IsNone())
	{
		log(Error, "[Weapon='%s' Slot='%s'] Can't get socket name", *GetName(), SLOT_NAME);
		return false;
	}

	if (not CharacterMesh->GetSocketByName(SocketName))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Socket not found in character mesh", *GetName(), SLOT_NAME);
		return false;
	}

	log(
		Display,
		"[Weapon='%s' Slot='%s'] Update attachment to character '%s' into socket '%s'",
		*GetName(), SLOT_NAME, *Character->GetName(), *SocketName.ToString());

	AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

	WeaponState.OnUpdateWeaponAttachment(NewSlot, NewBond, Instant);

	SetActorTickEnabled(NewBond == EWeaponBond::Armed);

	return true;
}

bool ACloud9WeaponBase::AddToInventory(ACloud9Character* Character, EWeaponSlot NewSlot)
{
	if (let MyOwner = GetOwner<ACloud9Character>())
	{
		log(
			Warning,
			"[Weapon='%s' Slot='%s'] Weapon already in inventory of '%s'",
			*GetName(), SLOT_NAME, *MyOwner->GetName());
		return false;
	}

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Invalid character", *GetName(), SLOT_NAME);
		return false;
	}

	if (NewSlot == EWeaponSlot::NotSelected)
	{
		log(Error, "[Weapon='%s' Slot='%s'] Invalid slot", *GetName(), SLOT_NAME);
		return false;
	}

	let Inventory = Character->GetInventory();

	if (not IsValid(Inventory))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Character inventory is invalid", *GetName(), SLOT_NAME);
		return false;
	}

	if (Inventory->GetWeaponAt(NewSlot) != nullptr)
	{
		log(Error, "[Weapon='%s' Slot='%s'] Weapon slot already occupied", *GetName(), SLOT_NAME);
		return false;
	}

	SetOwner(Character);

	if (not UpdateWeaponAttachment(NewSlot, EWeaponBond::Holstered))
	{
		log(Error, "Failed to update attachment for weapon '%s' slot '%s'", *GetName(), SLOT_NAME);
		SetOwner(nullptr);
		return false;
	}

	OnWeaponAddedToInventory();

	return true;
}

bool ACloud9WeaponBase::RemoveFromInventory()
{
	if (let Character = GetOwner<ACloud9Character>(); not Character)
	{
		log(Error, "[Weapon='%s'] Weapon not in any inventory", *GetName());
		return false;
	}

	// TODO: Add velocity impulse when weapon dropped

	WeaponState.OnRemovedFromInventory();

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetOwner(nullptr);

	OnWeaponRemovedFromInventory();

	return true;
}

bool ACloud9WeaponBase::ChangeState(EWeaponBond NewBond, bool Instant, bool Force)
{
	if (let Character = GetOwner<ACloud9Character>(); not IsValid(Character))
	{
		log(Warning, "[Weapon='%s' Bond='%s'] Weapon not in any inventory", *GetName(), BOND_NAME);
		return false;
	}

	if (WeaponState.IsWeaponBond(NewBond))
	{
		log(Warning, "[Weapon='%s' Bond='%s'] Weapon will remain the same", *GetName(), BOND_NAME);
		return false;
	}

	if (not Force and IsAnyMontagePlaying())
	{
		log(Error, "[Weapon='%s' Bond='%s'] Montage is playing now", *GetName(), BOND_NAME);
		return false;
	}

	if (not Force and IsActionInProgress())
	{
		log(Error, "[Weapon='%s' Bond='%s'] Some action is in progress", *GetName(), BOND_NAME);
		return false;
	}

	return UpdateWeaponAttachment(WeaponState.GetWeaponSlot(), NewBond, Instant);
}

void ACloud9WeaponBase::PrimaryAction(bool bIsReleased)
{
	WeaponState.ActivateAction(EWeaponAction::Primary, bIsReleased);
}

void ACloud9WeaponBase::SecondaryAction(bool bIsReleased)
{
	WeaponState.ActivateAction(EWeaponAction::Secondary, bIsReleased);
}

void ACloud9WeaponBase::Reload(bool bIsReleased) {}

bool ACloud9WeaponBase::Initialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	InitializeName(NewWeaponId);
	WeaponSkin = NewWeaponSkin;

	if (not OnInitialize(NewWeaponId, NewWeaponSkin))
	{
		log(Error, "[Weapon='%s'] Weapon initialization failure", *GetName());
		Deinitialize();
		return false;
	}

	// Enable tick only when weapon armed
	SetActorTickEnabled(false);

	return true;
}

bool ACloud9WeaponBase::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	let Asset = GetWeaponDefinitionsAsset();

	if (not IsValid(Asset))
	{
		log(Error, "[Weapon='%s'] Can't get weapon definitions asset", *GetName());
		return false;
	}

	if (not Asset->GetWeaponDefinition(GetWeaponId(), WeaponDefinition))
	{
		log(Error, "[Weapon='%s'] Not initialized and Tick() will be disabled", *GetName());
		return false;
	}

	return true;
}

void ACloud9WeaponBase::Deinitialize()
{
	log(Warning, "[Weapon='%s'] Deinitialize weapon to initial state", *GetName());

	WeaponSkin = FWeaponSkin::Default;

	WeaponState.Reset();
	WeaponDefinition.Reset();
	Executors.Reset();

	WeaponMesh->SetStaticMesh(nullptr);
	MagazineMesh->SetStaticMesh(nullptr);
	SilencerMesh->SetStaticMesh(nullptr);
	MuzzleFlash->SetAsset(nullptr);

	SetActorTickEnabled(false);
}

void ACloud9WeaponBase::OnWeaponAddedToInventory() {}

void ACloud9WeaponBase::OnWeaponRemovedFromInventory() {}

void ACloud9WeaponBase::ChangeMeshCollisionState(UStaticMeshComponent* Mesh, bool bIsEnabled)
{
	let CollisionType = bIsEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	Mesh->SetSimulatePhysics(bIsEnabled);
	Mesh->SetCollisionEnabled(CollisionType);
}

FName ACloud9WeaponBase::GetWeaponName() const { return GetWeaponId() | EFWeaponId::ToName(); }

FWeaponId ACloud9WeaponBase::GetWeaponId() const
{
	static let UnknownWeaponId = ETVariant::Convert<FWeaponId>(EBadWeapon::NoClass);
	log(Fatal, "[Weapon='%s'] Not implmemented", *GetName())
	return UnknownWeaponId;
}

EWeaponType ACloud9WeaponBase::GetWeaponType() const
{
	assertf(IsValid(WeaponDefinition), "[Weapon='%ls'] Not initialized", *GetName());
	return WeaponDefinition.GetWeaponInfo()->Type;
}

EWeaponSlot ACloud9WeaponBase::GetWeaponSlot() const { return WeaponState.GetWeaponSlot(); }

const UStaticMeshSocket* ACloud9WeaponBase::GetSocketByName(FName SocketName) const
{
	return WeaponMesh->GetSocketByName(SocketName);
}

EWeaponClass ACloud9WeaponBase::GetWeaponClass() const { return EUEnum::From<EWeaponClass>(GetWeaponId()); }

bool ACloud9WeaponBase::CanBeDropped() const { return true; }

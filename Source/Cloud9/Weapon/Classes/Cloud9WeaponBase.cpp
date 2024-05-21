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

#include "PhysicsEngine/RadialForceComponent.h"
#include "Cloud9/Tools/Components/CooldownActionComponent.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9AnimationComponent.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
#include "Cloud9/Game/Cloud9AssetManager.h"
#include "Cloud9/Weapon/Assets/WeaponDefinitionsAsset.h"
#include "Cloud9/Weapon/Enums/WeaponAction.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Extensions/EWeaponId.h"

const FName ACloud9WeaponBase::RootComponentName = TEXT("RootComponent");
const FName ACloud9WeaponBase::WeaponMeshCollisionProfile = TEXT("WeaponCollisionProfile");
const FString ACloud9WeaponBase::ActionComponentFormat = TEXT("{0}ActionComponent");

const FName ACloud9WeaponBase::ExplosionEffectScaleName = TEXT("Scale");

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
	// Required for a weapon with automatic fire
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkin = NAME_None;

	WeaponMesh = CreateMeshComponent(WeaponMeshComponentName);
	OBJECT_VOID_IF_FAIL(IsValid(WeaponMesh), Error, "Failed to create WeaponMeshComponent");

	MagazineMesh = CreateMeshComponent(MagazineMeshComponentName, MagazineSocketName);
	OBJECT_VOID_IF_FAIL(IsValid(WeaponMesh), Error, "Failed to create MagazineMeshComponent");

	MuzzleFlash = CreateEffectComponent(MuzzleFlashComponentName, MuzzleFlashSocketName);
	OBJECT_VOID_IF_FAIL(IsValid(WeaponMesh), Error, "Failed to create MuzzleFlashComponent");

	SilencerMesh = CreateMeshComponent(SilencerMeshComponentName, SilencerSocketName);
	OBJECT_VOID_IF_FAIL(IsValid(WeaponMesh), Error, "Failed to create SilencerMeshComponent");

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

	// Try reload later then ... this will haven in PIE
	// if some object required this asset placed on level
	RETURN_IF_FAIL(IsValid(Asset), nullptr, Error, "WeaponDefinitionsAsset loading failure");

	WeaponDefinitionsAsset = Asset;

	return WeaponDefinitionsAsset;
}

UCooldownActionComponent* ACloud9WeaponBase::CreateCooldownAction(FName ComponentName)
{
	let Component = CreateDefaultSubobject<UCooldownActionComponent>(ComponentName);
	OBJECT_RETURN_IF_FAIL(IsValid(Component), nullptr, Error, "Can't create cooldown action '%s'",
	                      *ComponentName.ToString());
	return Component;
}

UStaticMeshComponent* ACloud9WeaponBase::CreateMeshComponent(FName ComponentName, FName SocketName)
{
	let Component = CreateDefaultSubobject<UStaticMeshComponent>(ComponentName);
	OBJECT_RETURN_IF_FAIL(IsValid(Component), nullptr, Error, "Can't create mesh '%s'", *ComponentName.ToString());

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

URadialForceComponent* ACloud9WeaponBase::CreateDetonateComponent(FName ComponentName)
{
	let Component = CreateDefaultSubobject<URadialForceComponent>(ComponentName);
	OBJECT_RETURN_IF_FAIL(IsValid(Component), nullptr, Error, "Can't create VFX '%s'", *ComponentName.ToString());
	Component->SetupAttachment(RootComponent);
	return Component;
}

UNiagaraComponent* ACloud9WeaponBase::CreateEffectComponent(FName ComponentName, FName SocketName)
{
	let Component = CreateDefaultSubobject<UNiagaraComponent>(ComponentName);
	OBJECT_RETURN_IF_FAIL(IsValid(Component), nullptr, Error, "Can't create VFX '%s'", *ComponentName.ToString());
	Component->SetupAttachment(RootComponent, SocketName);
	return Component;
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
	OBJECT_RETURN_IF_FAIL(Mesh, false, Error, "Mesh is invalid");

	Component->SetStaticMesh(Mesh);

	if (Material != nullptr)
	{
		Component->SetMaterial(0, Material);
	}

	return true;
}

bool ACloud9WeaponBase::InitializeEffectComponent(
	UNiagaraComponent* Component,
	UNiagaraSystem* Effect,
	float Scale) const
{
	OBJECT_RETURN_IF_FAIL(Effect, false, Error, "Effect is invalid");

	Component->SetAsset(Effect);
	Component->SetFloatParameter(ExplosionEffectScaleName, Scale);
	Component->Deactivate();

	return true;
}

#define SLOT_NAME *UWeaponSlot::ToString(NewSlot)
#define BOND_NAME *UWeaponBond::ToString(NewBond)

bool ACloud9WeaponBase::UpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponBond NewBond, bool Instant)
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), false, Error, "Weapon owner is invalid for slot '%s'", SLOT_NAME);

	let CharacterMesh = Character->GetMesh();
	OBJECT_RETURN_IF_FAIL(IsValid(CharacterMesh), false, Error, "Character mesh is invalid for slot '%s'", SLOT_NAME);

	let SocketName = NewBond == EWeaponBond::Armed
		                 ? UWeaponSlot::EquippedSocket()
		                 : UWeaponSlot::HolsteredSocket(NewSlot);

	OBJECT_RETURN_IF_FAIL(not SocketName.IsNone(), false, Error, "Can't get socket name for slot '%s'", SLOT_NAME);
	OBJECT_RETURN_IF_FAIL(
		CharacterMesh->GetSocketByName(SocketName), false,
		Error, "Socket not found in character mesh for '%s'", SLOT_NAME);

	OBJECT_VERBOSE(
		"Update attachment to character '%s' into socket '%s'",
		*Character->GetName(), *SocketName.ToString());

	AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

	WeaponState.OnUpdateWeaponAttachment(NewSlot, NewBond, Instant);

	SetActorTickEnabled(NewBond == EWeaponBond::Armed);

	return true;
}

const FWeaponCommonData* ACloud9WeaponBase::GetWeaponCommonData() const
{
	return WeaponDefinition.GetCommonData();
}

USceneComponent* ACloud9WeaponBase::GetShootLocationActor() const
{
	// TODO: Make it independent from animation
	return MuzzleFlash;
}

bool ACloud9WeaponBase::AddToInventory(ACloud9Character* Character, EWeaponSlot NewSlot)
{
	OBJECT_RETURN_IF_FAIL(
		not GetOwner<ACloud9Character>(), false,
		Error, "Weapon already in inventory of '%s'", *GetOwner()->GetName());
	OBJECT_RETURN_IF_FAIL(IsValid(Character), false, Error, "Invalid character for slot '%s'", SLOT_NAME);
	OBJECT_RETURN_IF_FAIL(NewSlot != EWeaponSlot::NotSelected, false, Error, "Invalid slot");

	let Inventory = Character->GetInventoryComponent();

	OBJECT_RETURN_IF_FAIL(IsValid(Inventory), false, Error, "Character inventory is invalid for slot '%s'",
	                      SLOT_NAME);
	OBJECT_RETURN_IF_FAIL(not Inventory->GetWeaponAt(NewSlot), false, Error, "Weapon slot '%s' already occupied",
	                      SLOT_NAME);

	SetOwner(Character);

	// Set new instigator for this weapon, so When a weapon thrown an owner will be changed
	// to nothing, but we need to know who do it (throw a weapon).
	// This is important for nades to get handle number of kills for player
	SetInstigator(Character);

	if (not UpdateWeaponAttachment(NewSlot, EWeaponBond::Holstered))
	{
		OBJECT_ERROR("Failed to update attachment for slot '%s'", SLOT_NAME);
		SetOwner(nullptr);
		return false;
	}

	OnWeaponAddedToInventory();

	return true;
}

bool ACloud9WeaponBase::RemoveFromInventory()
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), false, Error, "Weapon not in any inventory");

	// TODO: Add velocity impulse when weapon dropped

	WeaponState.OnRemovedFromInventory();

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetOwner(nullptr);

	OnWeaponRemovedFromInventory();

	return true;
}

bool ACloud9WeaponBase::ChangeState(EWeaponBond NewBond, bool Instant, bool Force)
{
	let Character = GetOwner<ACloud9Character>();
	OBJECT_RETURN_IF_FAIL(IsValid(Character), false, Error, "Weapon owner is invalid for bond '%s'", BOND_NAME);

	let AnimComponent = Character->GetAnimationComponent();
	OBJECT_RETURN_IF_FAIL(IsValid(AnimComponent), false, Error, "Weapon owner animation component is invalid '%s'",
	                      BOND_NAME);

	OBJECT_RETURN_IF_FAIL(
		Force or not AnimComponent->IsAnyMontagePlaying(), false,
		Verbose, "Montage is playing now");

	OBJECT_RETURN_IF_FAIL(
		Force or not IsActionInProgress(), false,
		Verbose, "Some action is in progress for bond '%s'", BOND_NAME);

	return UpdateWeaponAttachment(WeaponState.GetWeaponSlot(), NewBond, Instant);
}

void ACloud9WeaponBase::PrimaryAction(bool IsReleased)
{
	WeaponState.ActivateSequence(
		EWeaponAction::PrimaryStart,
		EWeaponAction::PrimaryLoop,
		EWeaponAction::PrimaryEnd,
		IsReleased);
}

void ACloud9WeaponBase::SecondaryAction(bool IsReleased)
{
	WeaponState.ActivateAction(EWeaponAction::Secondary, IsReleased);
}

bool ACloud9WeaponBase::CanReload() const { return false; }

void ACloud9WeaponBase::Reload(bool IsReleased)
{
	if (CanReload())
	{
		WeaponState.ActivateSequence(
			EWeaponAction::ReloadStart,
			EWeaponAction::ReloadLoop,
			EWeaponAction::ReloadEnd,
			IsReleased);
	}
}

bool ACloud9WeaponBase::Initialize(const FWeaponConfig& WeaponConfig)
{
	InitializeName(WeaponConfig.GetWeaponId());
	WeaponSkin = WeaponConfig.GetSkinName();

	if (not OnInitialize(WeaponConfig))
	{
		OBJECT_ERROR("Weapon initialization failure");
		Deinitialize();
		return false;
	}

	// Enable tick only when a weapon armed
	SetActorTickEnabled(false);

	return true;
}

bool ACloud9WeaponBase::OnInitialize(const FWeaponConfig& WeaponConfig)
{
	let Asset = GetWeaponDefinitionsAsset();

	OBJECT_RETURN_IF_FAIL(IsValid(Asset), false, Error, "Can't get weapon definitions asset");
	OBJECT_RETURN_IF_FAIL(
		Asset->GetWeaponDefinition(WeaponConfig.GetWeaponId(), WeaponDefinition), false,
		Error, "Not initialized and Tick() will be disabled");

	return true;
}

void ACloud9WeaponBase::Deinitialize()
{
	OBJECT_WARN("Deinitialize weapon to initial state");

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
	OBJECT_FATAL("Not implmemented");
	return UnknownWeaponId;
}

FName ACloud9WeaponBase::GetWeaponSkin() const { return WeaponSkin; }

EWeaponType ACloud9WeaponBase::GetWeaponType() const
{
	CRASH_IF_FAIL(IsValid(WeaponDefinition), "[Weapon='%ls'] Not initialized", *GetName());
	return WeaponDefinition.GetWeaponInfo()->Type;
}

EWeaponSlot ACloud9WeaponBase::GetWeaponSlot() const { return WeaponState.GetWeaponSlot(); }

FText ACloud9WeaponBase::GetWeaponLabel() const { return WeaponDefinition.GetWeaponInfo()->Label; }

UTexture2D* ACloud9WeaponBase::GetWeaponIcon() const { return WeaponDefinition.GetWeaponInfo()->Icon; }

const UStaticMeshSocket* ACloud9WeaponBase::GetSocketByName(FName SocketName) const
{
	return WeaponMesh->GetSocketByName(SocketName);
}

EWeaponClass ACloud9WeaponBase::GetWeaponClass() const { return EUEnum::From<EWeaponClass>(GetWeaponId()); }

bool ACloud9WeaponBase::CanBeDropped() const { return true; }

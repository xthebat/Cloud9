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
#include "Cloud9/Tools/Components/CooldownActionComponent.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableBase.h"

const FName ACloud9WeaponBase::RootComponentName = TEXT("RootComponent");
const FName ACloud9WeaponBase::WeaponMeshCollisionProfile = TEXT("WeaponCollisionProfile");
const FString ACloud9WeaponBase::ActionComponentFormat = TEXT("{0}ActionComponent");

ACloud9WeaponBase::ACloud9WeaponBase()
{
	// Required for weapon with automatic fire
	PrimaryActorTick.bCanEverTick = true;

	State = EWeaponState::Dropped;
	Slot = EWeaponSlot::NotSelected;
	bIsInitialized = false;

	Info = nullptr;
	Montages = nullptr;

	bIsPrimaryActionActive = false;
	bIsSecondaryActionActive = false;
}

void ACloud9WeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Insanity checks just in case
	static_assert(AnyActionId == 0);
	static_assert(AnyActionIndex == -1);

	let Class = GetWeaponClass();
	let Actions = GetWeaponActions();

	// Use check to be sure compiler not discard functions calls
	assert(Class != EWeaponClass::NoClass);
	assert(Actions != nullptr);

	assertf(bIsInitialized, "Weapon '%ls' wasn't initialized!", *GetName());
	assertf(Info != nullptr, "Weapon Info for '%ls' wasn't set!", *GetName());
	assertf(Montages != nullptr, "Weapon Montages '%ls' wasn't set!", *GetName());

	for (int Id = AnyActionId + 1; Id < Actions->NumEnums(); Id++)
	{
		let CooldownActionName = Actions | EUEnum::GetValueName(Id);
		let ComponentName = FString::Format(*ActionComponentFormat, {CooldownActionName.ToString()});
		let CooldownActionComponent = CreateCooldownAction(FName(ComponentName));
		Executors.Add(CooldownActionComponent);
	}
}

void ACloud9WeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* ACloud9WeaponBase::CreateMesh(FName ComponentName, FName SocketName)
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

		// TODO: Collision should be enabled by default and disabled when added to inventory
		// Disable by default Collision and Physics for added mesh
		Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Component->SetSimulatePhysics(false);

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

UNiagaraComponent* ACloud9WeaponBase::CreateEffect(FName ComponentName, FName SocketName)
{
	if (let Component = CreateDefaultSubobject<UNiagaraComponent>(ComponentName); IsValid(Component))
	{
		Component->SetupAttachment(RootComponent, SocketName);
		return Component;
	}

	log(Error, "Can't create VFX '%s' for actor '%s'", *ComponentName.ToString(), *GetName());
	return nullptr;
}

bool ACloud9WeaponBase::IsActionIndexValid(int Index) const
{
	if (Index > Executors.Num() or Index < 0)
	{
		log(
			Error,
			"[Weapon='%s'] Invalid action index '%d', should be > 0 and < %d",
			*GetName(), Index, Executors.Num());
		return false;
	}
	return true;
}

UCooldownActionComponent* ACloud9WeaponBase::CreateCooldownAction(FName ComponentName)
{
	let Component = NewObject<UCooldownActionComponent>(
		this, UCooldownActionComponent::StaticClass(), ComponentName);
	Component->RegisterComponent();

	// CreateDefaultSubobject - works only in constructor
	// let Component = CreateDefaultSubobject<UCooldownActionComponent>(ComponentName);

	if (not IsValid(Component))
	{
		log(Error, "Can't cooldown action '%s' for actor '%s'", *ComponentName.ToString(), *GetName());
		return nullptr;
	}

	return Component;
}

#define SLOT_NAME *UCloud9WeaponSlot::ToString(NewSlot)
#define STATE_NAME *UCloud9WeaponState::ToString(NewState)

bool ACloud9WeaponBase::UpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponState NewState)
{
	let Character = GetOwner<ACloud9Character>();

	if (not IsValid(Character))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Weapon isn't attach to any one", *GetName(), SLOT_NAME);
		return false;
	}

	let CharacterMesh = Character->GetMesh();

	if (not IsValid(CharacterMesh))
	{
		log(Error, "[Weapon='%s' Slot='%s'] Character mesh is invalid", *GetName(), SLOT_NAME);
		return false;
	}

	let SocketName = NewState == EWeaponState::Armed
		                 ? UCloud9WeaponSlot::EquippedSocket()
		                 : UCloud9WeaponSlot::HolsteredSocket(NewSlot);

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

	Slot = NewSlot;
	State = NewState;

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

	if (not UpdateWeaponAttachment(NewSlot, EWeaponState::Holstered))
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

	Slot = EWeaponSlot::NotSelected;
	State = EWeaponState::Dropped;

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetOwner(nullptr);

	OnWeaponRemovedFromInventory();

	return true;
}

bool ACloud9WeaponBase::ChangeState(EWeaponState NewState)
{
	if (let Character = GetOwner<ACloud9Character>(); not IsValid(Character))
	{
		log(Error, "[Weapon='%s' State='%s'] Weapon not in any inventory", *GetName(), STATE_NAME);
		return false;
	}

	if (NewState == State)
	{
		log(Warning, "[Weapon='%s' State='%s'] Weapon state will remain the same", *GetName(), STATE_NAME);
		return false;
	}

	UpdateWeaponAttachment(Slot, NewState);
	return true;
}

void ACloud9WeaponBase::PrimaryAction(bool bIsReleased)
{
	bIsPrimaryActionActive = ChangeActionFlag(bIsPrimaryActionActive, bIsReleased);
}

void ACloud9WeaponBase::SecondaryAction(bool bIsReleased)
{
	bIsSecondaryActionActive = ChangeActionFlag(bIsSecondaryActionActive, bIsReleased);
}

void ACloud9WeaponBase::Reload() {}

void ACloud9WeaponBase::OnWeaponAddedToInventory()
{
	// CapsuleComponent->SetSimulatePhysics(false);
	// CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACloud9WeaponBase::OnWeaponRemovedFromInventory()
{
	// CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// CapsuleComponent->SetSimulatePhysics(true);
}

bool ACloud9WeaponBase::ChangeActionFlag(bool Flag, bool bIsReleased)
{
	if (Flag and bIsReleased)
	{
		return false;
	}

	if (not Flag and not bIsReleased)
	{
		return true;
	}

	return Flag;
}

EWeaponType ACloud9WeaponBase::GetWeaponType() const { return GetWeaponInfo<FBaseWeaponInfo>()->Type; }

EWeaponClass ACloud9WeaponBase::GetWeaponClass() const
{
	log(Fatal, "[Weapon='%s'] Weapon class wasn't override", *GetName())
	return EWeaponClass::NoClass;
}

bool ACloud9WeaponBase::CanBeDropped() const { return true; }

const UEnum* ACloud9WeaponBase::GetWeaponActions() const
{
	log(Fatal, "[Weapon='%s'] Weapon actions wasn't override", *GetName());
	return nullptr;
}

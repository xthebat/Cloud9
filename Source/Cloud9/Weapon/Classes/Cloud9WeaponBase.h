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

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Tools/Components/CooldownActionComponent.h"
#include "Cloud9/Weapon/Assets/WeaponDefinitionsAsset.h"
#include "Cloud9/Weapon/Enums/WeaponAction.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Enums/WeaponBond.h"
#include "Cloud9/Weapon/Structures/WeaponDefinition.h"
#include "Cloud9/Weapon/Structures/WeaponState.h"
#include "PhysicsEngine/RadialForceComponent.h"

#include "Cloud9WeaponBase.generated.h"

struct FWeaponConfig;
class ACloud9Character;
class UCloud9InventoryComponent;

UCLASS()
class CLOUD9_API ACloud9WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	static const FName RootComponentName;
	static const FName WeaponMeshCollisionProfile;
	static const FString ActionComponentFormat;

	static const FName ExplosionEffectScaleName;

	static const FName WeaponMeshComponentName;
	static const FName MagazineMeshComponentName;
	static const FName SilencerMeshComponentName;
	static const FName MuzzleFlashComponentName;

	static const FName MagazineSocketName;
	static const FName SilencerSocketName;
	static const FName MuzzleFlashSocketName;
	static const FName CaseEjectSocketName;

public:
	static UWeaponDefinitionsAsset* GetWeaponDefinitionsAsset();

public:
	ACloud9WeaponBase();

	UFUNCTION(BlueprintCallable)
	EWeaponClass GetWeaponClass() const;

	/**
	 * Function returns weapon unique name as enum variant (identifier from weapon asset table)
	 */
	virtual FWeaponId GetWeaponId() const;

	template <typename WeaponIdType>
	WeaponIdType GetWeaponId() const { return GetWeaponId().Get<WeaponIdType>(); }

	UFUNCTION(BlueprintCallable)
	FName GetWeaponSkin() const;

	/**
	 * Function returns weapon unique name as string (identifier from weapon asset table)
	 */
	UFUNCTION(BlueprintCallable)
	FName GetWeaponName() const;

	UFUNCTION(BlueprintCallable)
	virtual bool CanBeDropped() const;

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable)
	EWeaponSlot GetWeaponSlot() const;

	UFUNCTION(BlueprintCallable)
	FText GetWeaponLabel() const;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetWeaponIcon() const;

	UFUNCTION(BlueprintCallable)
	const UStaticMeshSocket* GetSocketByName(FName SocketName) const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	/**
	 * Outer method to initialize weapon as concrete weapon type by it WeaponId and WeaponSkin.
	 * Called from FWeaponConfig to initialize method.
	 */
	bool Initialize(const FWeaponConfig& WeaponConfig);

	template <typename WeaponIdType>
	bool Initialize(WeaponIdType NewWeaponName, FName NewWeaponSkin)
	{
		let Variant = ETVariant::Convert<FWeaponId>(NewWeaponName);
		return Initialize(Variant, NewWeaponSkin);
	}

	FORCEINLINE bool IsWeaponDefined() const { return IsValid(WeaponDefinition); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDeploying() const { return WeaponState.IsActionActive(EWeaponAction::Deploy); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsReloading() const
	{
		return WeaponState.IsActionActive(
			EWeaponAction::ReloadStart,
			EWeaponAction::ReloadLoop,
			EWeaponAction::ReloadEnd);
	}

	UFUNCTION(BlueprintCallable)
	bool IsBoltCycled() const { return WeaponState.IsBoltCycled(); }

	bool AddToInventory(ACloud9Character* Character, EWeaponSlot NewSlot);
	bool RemoveFromInventory();

	bool ChangeState(EWeaponBond NewBond, bool Instant, bool Force = false);

	template <typename OnExecuteType, typename OnCompleteType>
	FORCEINLINE bool ExecuteAction(
		EWeaponAction Action,
		float CooldownTime,
		OnExecuteType OnExecute,
		OnCompleteType OnComplete)
	{
		let ActionIndex = Action | EUEnum::To<int>{};
		return Executors[ActionIndex]->Execute(CooldownTime, OnExecute, OnComplete);
	}

	template <typename OnExecuteType>
	FORCEINLINE bool ExecuteAction(EWeaponAction Action, float CooldownTime, OnExecuteType&& OnExecute)
	{
		let ActionIndex = Action | EUEnum::To<int>{};
		return Executors[ActionIndex]->Execute(CooldownTime, OnExecute);
	}

	FORCEINLINE bool IsActionInProgress(EWeaponAction Action) const
	{
		let ActionIndex = Action | EUEnum::To<int>{};
		return Executors[ActionIndex]->IsExecuting();
	}

	FORCEINLINE bool IsActionInProgress() const
	{
		return Executors | ETContainer::AnyByPredicate{[](let It) { return It->IsExecuting(); }};
	}

	FORCEINLINE bool IsWeaponArmed() const { return WeaponState.IsWeaponBond(EWeaponBond::Armed); }
	FORCEINLINE bool IsWeaponDisarmed() const { return not IsWeaponArmed(); }

	virtual void PrimaryAction(bool IsReleased);
	virtual void SecondaryAction(bool IsReleased);
	virtual void Reload(bool IsReleased);

protected:
	static void ChangeMeshCollisionState(UStaticMeshComponent* Mesh, bool bIsEnabled);

protected: // functions
	virtual bool OnInitialize(const FWeaponConfig& WeaponConfig);
	virtual void Deinitialize();

	/**
	 * Function called whenever weapon added to inventory and should
	 * process collision changes when weapon attached to character
	 */
	virtual void OnWeaponAddedToInventory();

	/**
	 * Function called whenever weapon removed from inventory and should
	 * process collision changes when weapon detached from character
	 */
	virtual void OnWeaponRemovedFromInventory();

	UStaticMeshComponent* CreateMeshComponent(FName ComponentName, FName SocketName = NAME_None);
	UNiagaraComponent* CreateEffectComponent(FName ComponentName, FName SocketName = NAME_None);
	URadialForceComponent* CreateDetonateComponent(FName ComponentName);

	void InitializeName(const FWeaponId& NewWeaponId);
	bool InitializeMeshComponent(UStaticMeshComponent* Component, UStaticMesh* Mesh, UMaterialInstance* Material) const;
	bool InitializeEffectComponent(UNiagaraComponent* Component, UNiagaraSystem* Effect, float Scale = 1.0f) const;

	bool UpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponBond NewBond, bool Instant = false);

	const FWeaponCommonData* GetWeaponCommonData() const;

#define WEAPON_IS_DEFINED_GUARD() \
	if (not IsWeaponDefined()) \
	{ \
		log(Error, "[Weapon='%s'] Not defined", *GetName()); \
		return; \
	}

#define WEAPON_ANIM_COMPONENT_GUARD() \
	if (not IsValid(AnimComponent)) \
	{ \
		log(Warning, "[Weapon='%s'] AnimComponent isn't valid", *GetName()); \
		return; \
	}

#define WEAPON_IS_ACTION_IN_PROGRESS_GUARD() \
	if (IsActionInProgress()) \
	{ \
		log(Verbose, "[Weapon='%s'] Action already in progress during Tick", *GetName()); \
		return; \
	}

#define WEAPON_IS_DISARMED_GUARD() \
	if (IsWeaponDisarmed()) \
	{ \
		return; \
	}

protected: // properties
	/**
	  * Current weapon skin name
	  */
	UPROPERTY(Category=Base, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess))
	FName WeaponSkin;

	/**
	 * Weapon mesh
	 */
	UPROPERTY(Category=Base, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UStaticMeshComponent* WeaponMesh;

	/**
	 * Magazine mesh
	 */
	UPROPERTY(Category=Firearm, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UStaticMeshComponent* MagazineMesh;

	/**
	 * Silencer mesh
	 */
	UPROPERTY(Category=Firearm, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UStaticMeshComponent* SilencerMesh;

	/**
	 * Muzzle flash effect to play when shoot
	 */
	UPROPERTY(Category=Firearm, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UNiagaraComponent* MuzzleFlash;

	/**
	 * Weapon cumulative data
	 */
	UPROPERTY(Category=Settings, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FWeaponDefinition WeaponDefinition;

	UPROPERTY(Category=State, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FWeaponState WeaponState;

private:
	UCooldownActionComponent* CreateCooldownAction(FName ComponentName);

	/**
	 * Cooldown action's executors
	 */
	UPROPERTY()
	TArray<UCooldownActionComponent*> Executors;
};

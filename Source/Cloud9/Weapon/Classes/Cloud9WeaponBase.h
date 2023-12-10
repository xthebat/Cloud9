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
#include "Cloud9/Cloud9.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Components/CooldownActionComponent.h"
#include "Cloud9/Weapon/Assets/WeaponDefinitionsAsset.h"
#include "Cloud9/Weapon/Enums/WeaponClass.h"
#include "Cloud9/Weapon/Enums/WeaponState.h"
#include "Cloud9/Weapon/Structures/WeaponDefinition.h"
#include "Cloud9WeaponBase.generated.h"

class UCloud9Inventory;

UCLASS()
class CLOUD9_API ACloud9WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	static const FName RootComponentName;
	static const FName WeaponMeshCollisionProfile;
	static const FString ActionComponentFormat;

	static constexpr int AnyActionId = 0;
	static constexpr int AnyActionIndex = AnyActionId - 1;

public:
	ACloud9WeaponBase();

	UFUNCTION(BlueprintCallable)
	virtual FName GetWeaponName() const;

	UFUNCTION(BlueprintCallable)
	virtual EWeaponClass GetWeaponClass() const;

	UFUNCTION(BlueprintCallable)
	virtual const UEnum* GetWeaponActions() const;

	UFUNCTION(BlueprintCallable)
	virtual bool CanBeDropped() const;

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;

	bool IsWeaponInitialized() const { return WeaponDefinition.IsSet(); }

	bool AddToInventory(ACloud9Character* Character, EWeaponSlot NewSlot);
	bool RemoveFromInventory();
	bool ChangeState(EWeaponState NewState);

	template <typename WeaponActionType, typename FunctionType>
	FORCEINLINE bool ExecuteAction(WeaponActionType WeaponAction, float Cooldown, FunctionType&& Function)
	{
		let Index = GetActionIndex(WeaponAction);

		if (Index == AnyActionIndex)
		{
			log(Error, "[Weapon='%s'] Action type can't be equal to AnyAction", *GetName());
			return false;
		}

		if (IsActionIndexValid(Index))
		{
			return not Executors[Index]->Execute(MoveTemp(Function), Cooldown);
		}

		return false;
	}

	template <typename WeaponActionType, typename FunctionType>
	FORCEINLINE bool ExecuteAction(WeaponActionType WeaponAction, FunctionType Function)
	{
		return ExecuteAction(WeaponAction, -1, Function);
	}

	template <typename WeaponActionType = int>
	FORCEINLINE bool IsActionInProgress(WeaponActionType WeaponAction = AnyActionId) const
	{
		let Index = GetActionIndex(WeaponAction);

		if (Index == AnyActionIndex)
		{
			let Executor = Executors.FindByPredicate([](let It) { return It->IsExecuting(); });
			return Executor != nullptr;
		}

		if (IsActionIndexValid(Index))
		{
			return not Executors[Index]->IsExecuting();
		}

		return false;
	}

	virtual void PrimaryAction(bool bIsReleased);
	virtual void SecondaryAction(bool bIsReleased);
	virtual void Reload();

protected: // functions
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

	static UWeaponDefinitionsAsset* GetWeaponDefinitionsAsset();

	static bool ChangeActionFlag(bool Flag, bool bIsReleased);

	UStaticMeshComponent* CreateMeshComponent(FName ComponentName, FName SocketName = NAME_None);
	UNiagaraComponent* CreateEffectComponent(FName ComponentName, FName SocketName);

	bool InitializeMeshComponent(
		UStaticMeshComponent* Component,
		UStaticMesh* Mesh,
		const FWeaponSkin& SkinInfo
	) const;

	bool InitializeEffectComponent(UNiagaraComponent* Component, UNiagaraSystem* Effect) const;

	bool PlayMontage(UAnimMontage* Montage) const;
	bool PlayRandomSound(const TArray<USoundBase*>& Sounds, float Volume) const;

	bool UpdateWeaponAttachment(
		EWeaponSlot NewSlot,
		EWeaponState NewState);

	virtual void OnConstruction(const FTransform& Transform) override;

#define WEAPON_IS_INITIALIZED_GUARD() \
	if (not IsWeaponInitialized()) \
	{ \
		log(Error, "[Weapon='%s'] Not initialized", *GetName()); \
		return; \
	}

#define WEAPON_IS_ACTION_IN_PROGRESS_GUARD() \
	if (IsActionInProgress()) \
	{ \
		log(Verbose, "[Weapon='%s'] Action already in progress during Tick", *GetName()); \
		return; \
	}

	bool IsActionIndexValid(int Index) const;

protected: // properties
	/**
	 * Current weapon skin name
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess))
	FName Skin;

	/**
	 * Weapon cumulative data
	 */
	TOptional<FWeaponDefinition> WeaponDefinition;

	/**
	 * Current weapon slot (main/pistol/knife/grenade)
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponSlot Slot;

	/**
	 * Current weapon state (armed/holstered/dropped)
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponState State;

	/**
	 * Is weapon primary action active
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsPrimaryActionActive;

	/**
	 * Is Weapon secondary action active
     */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsSecondaryActionActive;

private:
	template <typename WeaponActionType>
	static constexpr int GetActionIndex(WeaponActionType WeaponAction)
	{
		return static_cast<int>(WeaponAction) - 1;
	}


	UCooldownActionComponent* CreateCooldownAction(FName ComponentName);

	/**
	 * Cooldown action's executors
	 */
	UPROPERTY()
	TArray<UCooldownActionComponent*> Executors;
};

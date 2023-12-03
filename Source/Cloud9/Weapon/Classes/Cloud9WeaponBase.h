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
#include "Cloud9/Weapon/Enums/Cloud9WeaponClass.h"
#include "Cloud9/Weapon/Enums/Cloud9WeaponSlot.h"
#include "Cloud9/Weapon/Enums/Cloud9WeaponState.h"
#include "Cloud9/Weapon/Enums/Cloud9WeaponType.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableBase.h"
#include "Cloud9WeaponBase.generated.h"

class UCloud9Inventory;
struct FWeaponPosesMontages;

UCLASS()
class CLOUD9_API ACloud9WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	static const FName MeshCollisionProfile;
	static const FString ActionComponentFormat;
	static constexpr int AnyActionId = 0;
	static constexpr int AnyActionIndex = AnyActionId - 1;

public:
	ACloud9WeaponBase();

	UFUNCTION(BlueprintCallable)
	virtual EWeaponClass GetWeaponClass() const;

	UFUNCTION(BlueprintCallable)
	virtual const UEnum* GetWeaponActions() const;

	UFUNCTION(BlueprintCallable)
	virtual bool CanBeDropped() const;

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;

	template <typename TWeaponInfo>
	bool Initialize(const TWeaponInfo* NewWeaponInfo, const FWeaponPosesMontages* NewWeaponMontages)
	{
		assertf(not bIsInitialized, "Weapon '%ls' already initialized!", *Info->Label.ToString());
		bIsInitialized = true;
		Montages = NewWeaponMontages;
		Info = NewWeaponInfo;
		return true;
	}

	template <typename WeaponInfoType>
	const WeaponInfoType* GetWeaponInfo() const
	{
		// All checks are done in OnConstruction
		return static_cast<const WeaponInfoType*>(Info);
	}

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
	static bool ChangeActionFlag(bool Flag, bool bIsReleased);

	UStaticMeshComponent* CreateMesh(FName ComponentName, FName SocketName = NAME_None);
	UNiagaraComponent* CreateEffect(FName ComponentName, FName SocketName);

	bool UpdateWeaponAttachment(
		EWeaponSlot NewSlot,
		EWeaponState NewState);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

protected: // properties
	/**
	 * Flag sets to true on spawn through GameInstance spawners
	 * i.e. properties Montages and Info setups to valid values
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsInitialized;

	/**
	 * Base weapon info 
	 */
	const FBaseWeaponInfo* Info;

	/**
	 * Weapon montages
	 */
	const FWeaponPosesMontages* Montages;

	/**
	 * Current weapon slot (main/pistol/knife/grenade)
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	EWeaponSlot Slot;

	/**
	 * Current weapon state (armed/holstered/dropped)
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
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

	FORCEINLINE bool IsActionIndexValid(int Index) const;

	UCooldownActionComponent* CreateCooldownAction(FName ComponentName);

	/**
	 * Cooldown action's executors
	 */
	UPROPERTY()
	TArray<UCooldownActionComponent*> Executors;
};

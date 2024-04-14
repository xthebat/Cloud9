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

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Enums/GrenadeNames.h"

#include "Cloud9WeaponGrenade.generated.h"

class URadialForceComponent;

UCLASS()
class CLOUD9_API ACloud9WeaponGrenade : public ACloud9WeaponBase
{
	GENERATED_BODY()

	friend class ACloud9WeaponBase;

public:
	static const FName ExplosionComponentName;
	static const FName DetonationEffectComponentName;
	static const FName ActiveEffectComponentName;

	static constexpr let DebugSphereSegments = 32;
	static constexpr let DebugSphereLifeTime = 4.0f;
	static constexpr let DebugSphereThickness = 1.0f;

public:
	ACloud9WeaponGrenade();

	virtual FWeaponId GetWeaponId() const override;

	template <typename WeaponIdType>
	WeaponIdType GetWeaponId() const;

	const FGrenadeWeaponInfo* GetWeaponInfo() const;

protected:
	virtual bool OnInitialize(const FWeaponConfig& WeaponConfig) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnWeaponAddedToInventory() override;
	virtual void OnWeaponRemovedFromInventory() override;

	bool OnGrenadeActionFinished();
	bool OnGrenadeActionLoop();
	bool OnGrenadeThrown();

	bool Throw() const;

protected:
	/**
	 * Weapon Identifier
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, meta=(AllowPrivateAccess))
	EGrenade WeaponId;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	URadialForceComponent* Explosion;

	/**
	 * Effect when grenade detonated
	 */
	UPROPERTY(Category=Grenade, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UNiagaraComponent* DetonationEffect;

	/**
	 * Effect when grenade in active state
	 */
	UPROPERTY(Category=Grenade, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UNiagaraComponent* ActiveEffect;

	/**
	 * Detonation timer
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FTimerHandle DetonationTimerHandle;

	/**
	 * Active timer
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FTimerHandle ActiveTimerHandle;
};

template <>
inline EGrenade ACloud9WeaponGrenade::GetWeaponId<EGrenade>() const { return WeaponId; }

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

#include "Engine/DataTable.h"
#include "Cloud9/Weapon/Enums/WeaponId.h"
#include "Cloud9/Weapon/Enums/WeaponType.h"
#include "Cloud9/Weapon/Structures/WeaponDefinition.h"
#include "Cloud9/Weapon/Tables/WeaponMontages.h"
#include "WeaponDefinitionsAsset.generated.h"

UCLASS(BlueprintType)
class CLOUD9_API UWeaponDefinitionsAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetId PrimaryAssetId;

public: // functions
	bool GetWeaponDefinition(const FWeaponId& WeaponId, FWeaponDefinition& WeaponDefinition) const;

protected: // functions
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected: // properties
	/**
	 * Weapon info table for firearms
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* FirearmsWeaponsInfoTable;

	/**
	 * Weapon info table for melee
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* MeleeWeaponsInfoTable;

	/**
	 * Weapon info table for grenades
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UDataTable* GrenadeWeaponInfoTable;

	/**
	 * Weapon montages for different types
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	TMap<EWeaponType, FWeaponPosesMontages> WeaponActionMontages;

	/**
	 * Weapon tracer
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UParticleSystem* FirearmTracer;

private: // functions
	template <typename WeaponIdType, typename ValidatorType>
	TOptional<FWeaponDefinition> GetWeaponDefinition(
		UDataTable* WeaponsInfoTable,
		ValidatorType Validator,
		WeaponIdType WeaponId) const;
};

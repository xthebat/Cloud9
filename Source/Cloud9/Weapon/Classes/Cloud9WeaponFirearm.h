﻿// Copyright (c) 2023 Alexei Gladkikh
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
#include "Cloud9/Weapon/Enums/FirearmNames.h"

#include "Cloud9WeaponFirearm.generated.h"

class UCooldownActionComponent;
struct FWeaponActionMontages;
struct FFirearmWeaponInfo;

UCLASS()
class CLOUD9_API ACloud9WeaponFirearm : public ACloud9WeaponBase
{
	GENERATED_BODY()

	friend class ACloud9WeaponBase;

public:
	virtual FWeaponId GetWeaponId() const override;

protected:
	virtual bool OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin) override;

	virtual void OnWeaponAddedToInventory() override;
	virtual void OnWeaponRemovedFromInventory() override;

	virtual void Tick(float DeltaSeconds) override;

	bool Fire(const FFirearmWeaponInfo* WeaponInfo, float ImpulseMultiplier) const;

protected: // properties
	/**
	 * Weapon Identifier
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, meta=(AllowPrivateAccess))
	EFirearm WeaponId;
};

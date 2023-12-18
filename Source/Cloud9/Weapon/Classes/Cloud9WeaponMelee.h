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
#include "Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Enums/MeleeNames.h"
#include "Cloud9WeaponMelee.generated.h"

struct FMeleeWeaponInfo;

UCLASS()
class CLOUD9_API ACloud9WeaponMelee : public ACloud9WeaponBase
{
	GENERATED_BODY()

public:
	static const FName WeaponMeshComponentName;

public:
	ACloud9WeaponMelee();

	bool OnSpawn(EMelee WeaponName, FName WeaponSkin = FWeaponSkin::Default)
	{
		Name = WeaponName;
		Skin = WeaponSkin;
		return true;
	}

	virtual FName GetWeaponName() const override;
	virtual EWeaponClass GetWeaponClass() const override;
	virtual const UEnum* GetWeaponActions() const override;
	virtual bool CanBeDropped() const override;
	virtual const UStaticMeshSocket* GetSocketByName(FName SocketName) const override;
	virtual const UStaticMeshComponent* GetWeaponMesh() const override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	/**
	 * Weapon Identifier
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, meta=(AllowPrivateAccess))
	EMelee Name;

	/**
	 * Weapon mesh
	 */
	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UStaticMeshComponent* WeaponMesh;
};

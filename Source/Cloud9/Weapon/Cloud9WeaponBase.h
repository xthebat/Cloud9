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
#include "Cloud9/Character/Enums/Cloud9WeaponClass.h"
#include "Cloud9/Character/Enums/Cloud9WeaponType.h"
#include "GameFramework/Actor.h"
#include "Cloud9WeaponBase.generated.h"


UCLASS()
class CLOUD9_API ACloud9WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	static const FName CapsuleComponentName;
	static const FName MeshComponentName;
	static const FName MuzzleFlashComponentName;
	static const FName MeshCollisionProfile;
	static const FName EquippedWeaponSocketName;
	static const FName HolsteredPrimaryWeaponSocketName;
	static const FName HolsteredSecondaryWeaponSocketName;
	static const FName HolsteredGrenadeWeaponSocketName;

	ACloud9WeaponBase();

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable)
	EWeaponClass GetWeaponClass() const;

	UFUNCTION(BlueprintCallable)
	bool HasSecondaryAction() const;

	UFUNCTION(BlueprintCallable)
	bool CanBeDropped() const;

	UFUNCTION(BlueprintCallable)
	bool IsAutomatic() const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	UPROPERTY(Category=Weapon, BlueprintGetter=GetWeaponClass)
	EWeaponClass WeaponClass;

	UPROPERTY(Category=Weapon, BlueprintGetter=GetWeaponType)
	EWeaponType WeaponType;

	UPROPERTY(Category=Weapon, BlueprintGetter=HasSecondaryAction)
	bool bHasSecondaryAction;

	UPROPERTY(Category=Weapon, BlueprintGetter=CanBeDropped)
	bool bCanBeDropped;

	UPROPERTY(Category=Weapon, BlueprintGetter=IsAutomatic)
	bool bIsAutomatic;

	// EditDefaultsOnly

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float PrimaryActionCooldown;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float SecondaryActionCooldown;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float DeployTime;

	// Sounds

	UPROPERTY(Category=Sounds, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	USoundBase* PrimaryActionSound;

	UPROPERTY(Category=Sounds, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	USoundBase* SecondaryActionSound;

	UPROPERTY(Category=Sounds, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	USoundBase* DeploySound;
};

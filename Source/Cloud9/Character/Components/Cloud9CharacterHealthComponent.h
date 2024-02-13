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

#include "Cloud9/Character/Components/Cloud9CharacterComponent.h"

#include "Cloud9CharacterHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChange);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArmorChange);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHelmetChange);


class ACloud9Character;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9CharacterHealthComponent
	: public UActorComponent
	, public ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	UCloud9CharacterHealthComponent();

	void ChangeHealth(float Change);
	void ChangeArmor(float Change);
	void ChangeHasHelmet(bool NewState);

	UFUNCTION()
	void OnTakePointDamage(
		AActor* DamagedActor,
		float Damage,
		AController* InstigatedBy,
		FVector HitLocation,
		UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const UDamageType* DamageType,
		AActor* DamageCauser);

	UFUNCTION()
	void OnTakeRadialDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		FVector Origin,
		FHitResult HitInfo,
		AController* InstigatedBy,
		AActor* DamageCauser
	);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnArmorChange OnArmorChange;

	UPROPERTY(BlueprintAssignable)
	FOnHelmetChange OnHelmetChange;

	/** Current percentage health of character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	float Health;

	/** Current percentage armor of character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	float Armor;

	/** Current percentage armor of character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	bool bHasHelmet;
};

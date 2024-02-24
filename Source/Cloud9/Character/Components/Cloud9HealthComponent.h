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

#include "Cloud9/Character/Structures/HealthConfig.h"

#include "Cloud9HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChange, float, Health);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorChange, float, Armor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHelmetChange, bool, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDie, AActor*, Actor);

class ACloud9Character;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9HealthComponent();

	void Initialize(FHealthConfig Config);

	bool TakeHealthDamage(float Change);
	bool TakeArmorDamage(float Change);

	// Helmet is permanent(?)
	bool ChangeHasHelmet(bool NewState);

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

	float GetHealth() const { return Health; }
	float GetArmor() const { return Armor; }
	bool HasHelmet() const { return bHasHelmet; }
	bool IsAlive() const { return bIsAlive; }

public:
	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnHealthChange OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnArmorChange OnArmorChange;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnHelmetChange OnHelmetChange;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnCharacterDie OnCharacterDie;

protected:
	bool ChangeHealth(float NewHealth);
	bool ChangeArmor(float NewArmor);

	virtual void OnRegister() override;

	void AddAttackerScore(const AController* InstigatedBy) const;

protected:
	/** Current percentage health of character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	float Health;

	/** Current percentage armor of character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	float Armor;

	/** Current percentage armor of character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	bool bHasHelmet;

	/** Whether current character dead or alive */
	UPROPERTY(BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	bool bIsAlive;
};

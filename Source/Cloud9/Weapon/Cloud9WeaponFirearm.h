// 

#pragma once

#include "CoreMinimal.h"
#include "Cloud9WeaponBase.h"
#include "Cloud9WeaponFirearm.generated.h"

UCLASS()
class CLOUD9_API ACloud9WeaponFirearm : public ACloud9WeaponBase
{
	GENERATED_BODY()

public:
	ACloud9WeaponFirearm();

protected:
	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	float ReloadTime;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	int MagazineRoundsCount;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	int TotalRoundsCount;

	UPROPERTY(Category=Effects, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	UNiagaraComponent* MuzzleFlash;

	UPROPERTY(Category=Sounds, BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	USoundBase* ReloadSound;
};

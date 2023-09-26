#pragma once

#include "CoreMinimal.h"
#include "Cloud9CharacterComponent.h"
#include "Cloud9/Character/Enums/Cloud9WeaponSlot.h"
#include "Cloud9/Character/Enums/Cloud9WeaponType.h"
#include "Cloud9/Weapon/Cloud9WeaponPistol.h"
#include "Components/ActorComponent.h"
#include "Cloud9Inventory.generated.h"


class ACloud9WeaponKnife;
class ACloud9WeaponBase;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9Inventory
	: public UActorComponent
	, public ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	UCloud9Inventory();
	
	UFUNCTION(BlueprintCallable)
	void OnPoseUpdated();
	
	UFUNCTION(BlueprintCallable)
	bool SelectWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable)
	EWeaponType GetSelectedWeaponType();

	UFUNCTION(BlueprintCallable)
	EWeaponType GetPendingWeaponType();

	UFUNCTION(BlueprintCallable)
	bool SetWeaponAt(EWeaponSlot Slot, ACloud9WeaponBase* Weapon);
	
	UFUNCTION(BlueprintCallable)
	ACloud9WeaponBase* GetWeaponAt(EWeaponSlot Slot) const;
	
	UFUNCTION(BlueprintCallable)
	void OnWeaponChangeFinished();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACloud9WeaponKnife> DefaultKnifeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACloud9WeaponPistol> DefaultPistolClass;

	UPROPERTY()
	TArray<ACloud9WeaponBase*> WeaponSlots;
	
	EWeaponSlot SelectedWeaponSlot;
	EWeaponSlot PendingWeaponSlot;
};

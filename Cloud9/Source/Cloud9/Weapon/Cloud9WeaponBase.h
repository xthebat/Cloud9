#pragma once

#include "CoreMinimal.h"
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
	static const FName MeshCollisionProfile;
	static const FName WeaponSocketName;
	
	ACloud9WeaponBase();

	UFUNCTION(BlueprintCallable)
	bool CanBeDropped() const;
	
	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

	UPROPERTY(Category=Weapon, BlueprintGetter=CanBeDropped)
	bool bCanBeDropped;

	UPROPERTY(Category=Weapon, BlueprintGetter=GetWeaponType)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly)
	FTransform SocketTransform;
	
	UPROPERTY(Category=Weapon, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
};

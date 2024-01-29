// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawner.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class CLOUD9_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()

public:
	static FName RootComponentName;
	static FName TriggerBoxComponentName;
	static FName WeaponSampleComponentName;
	static FName GlowingEffectComponentName;
	static FName SpriteComponentName;

public:
	AWeaponSpawner();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit);

protected:
	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UBoxComponent* TriggerBoxComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UChildActorComponent* WeaponSampleComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UNiagaraComponent* GlowingEffectComponent;

	UPROPERTY(EditAnywhere, Category=Config)
	FWeaponConfig WeaponConfig;

	UPROPERTY(EditAnywhere, Category=Config)
	bool bIsDestroyOnActivation;

	UPROPERTY(EditAnywhere, Category=Glowing)
	UNiagaraSystem* GlowingEffect;

	UPROPERTY(EditAnywhere, Category=Glowing)
	bool bIsGlowingEffectPreview;

	UPROPERTY(EditAnywhere, Category=WeaponSample)
	float SampleScale;

	UPROPERTY(EditAnywhere, Category=WeaponSample)
	float RotationSpeedInDegree;

	UPROPERTY(EditAnywhere, Category=WeaponSample)
	bool bIsRandomInitialRotation;

	UPROPERTY(EditAnywhere, Category=Zone)
	FVector ZoneSize;

private:
#if WITH_EDITORONLY_DATA
	/** Billboard used to see the trigger in the editor */
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif
};

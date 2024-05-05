// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9EffectsComponent.generated.h"

class UCloud9HealthComponent;
class UCloud9CharacterEffectTrait;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9EffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9EffectsComponent();

	virtual void BeginPlay() override;

	UCloud9CharacterEffectTrait* AddEffect(TSubclassOf<UCloud9CharacterEffectTrait> EffectClass);

	bool RemoveEffect(UCloud9CharacterEffectTrait* Effect);

	void RemoveAllEffects();

protected:
	/**
	 * Set of initial effects (will be overridden by GameMode if IsNeedInitialization is set in Character) 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	TSet<TSubclassOf<UCloud9CharacterEffectTrait>> InitialEffects;

private:
	/**
	 * Currently applied set of effects
	 */
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> AppliedEffects;

	/**
	 * Subset of Effects that may tick
	 */
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> AppliedCanTickEffects;

	/**
	 * Subset of Effects that can be damaged
	 */
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> AppliedCanDamagedEffects;

	UFUNCTION()
	void OnDamageApplied(float Damage);

	FString GetOwnerName() const;

	UCloud9HealthComponent* GetHealthComponent() const;

	virtual void OnComponentCreated() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};

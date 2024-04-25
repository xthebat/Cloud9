// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9EffectsComponent.generated.h"

class UCloud9CharacterEffectTrait;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9EffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9EffectsComponent();

	UCloud9CharacterEffectTrait* AddEffect(TSubclassOf<UCloud9CharacterEffectTrait> EffectClass);

	bool RemoveEffect(UCloud9CharacterEffectTrait* Effect);

private:
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> Effects;

	/**
	 * Subset of Effects that may tick
	 */
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> CanTickEffects;

	/**
	 * Subset of Effects that can be damaged
	 */
	UPROPERTY()
	TSet<UCloud9CharacterEffectTrait*> CanDamagedEffects;

	UFUNCTION()
	void OnDamageApplied(float Damage);

	virtual void OnRegister() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};

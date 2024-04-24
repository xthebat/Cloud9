// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9/Character/Effects/Cloud9CharacterEffectInterface.h"

#include "Cloud9EffectsComponent.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9EffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9EffectsComponent();

	bool AddEffect(TSubclassOf<UCloud9CharacterEffectInterface> EffectClass);

	bool RemoveEffect(UObject* Effect);

private:
	UPROPERTY(meta=(MustImplement=ICloud9CharacterEffectInterface))
	TSet<UObject*> Effects;

	/**
	 * Subset of Effects that may tick
	 */
	UPROPERTY(meta=(MustImplement=ICloud9CharacterEffectInterface))
	TSet<UObject*> CanTickEffects;

	/**
	 * Subset of Effects that can be damaged
	 */
	UPROPERTY(meta=(MustImplement=ICloud9CharacterEffectInterface))
	TSet<UObject*> CanDamagedEffects;

	UFUNCTION()
	void OnDamageApplied(float Damage);

	virtual void OnRegister() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;
};

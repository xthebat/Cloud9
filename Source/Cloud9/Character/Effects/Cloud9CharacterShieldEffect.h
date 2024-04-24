// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9CharacterEffectInterface.h"

#include "Cloud9CharacterShieldEffect.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterShieldEffect : public UObject, public ICloud9CharacterEffectInterface
{
	GENERATED_BODY()

public:
	static const FName ShieldEnableName;

	UCloud9CharacterShieldEffect();

protected:
	UPROPERTY()
	float EffectTime;

	UPROPERTY()
	float ElapsedTime;

	virtual bool IsExtinguished_Implementation() const override;

	virtual bool OnApply_Implementation(UCloud9EffectsComponent* Container) override;

	virtual bool OnRemove_Implementation(UCloud9EffectsComponent* Container) override;

	virtual bool CanApply_Implementation(const UCloud9EffectsComponent* Container) override;

	virtual bool CanTick_Implementation() override;

	virtual bool CanDamaged_Implementation() override;

	virtual void OnTick_Implementation(const UCloud9EffectsComponent* Container, float DeltaSeconds) override;

	virtual void OnApplyDamage_Implementation(const UCloud9EffectsComponent* Container, float Damage) override;

private:
	bool ToggleEffect(const UCloud9EffectsComponent* Container, bool IsEnabled) const;
};

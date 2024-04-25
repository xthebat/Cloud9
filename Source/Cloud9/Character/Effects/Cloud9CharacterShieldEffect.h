// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9CharacterEffectTrait.h"

#include "Cloud9CharacterShieldEffect.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterShieldEffect : public UCloud9CharacterEffectTrait
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

	virtual void OnApply_Implementation() override;

	virtual void OnRemove_Implementation() override;

	virtual bool CanApply_Implementation() const override;

	virtual bool CanTick_Implementation() const override;

	virtual void OnTick_Implementation(float DeltaSeconds) override;

private:
	void ToggleEffect(bool IsEnabled) const;
};

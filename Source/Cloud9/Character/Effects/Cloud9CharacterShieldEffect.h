// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9CharacterEffectTrait.h"

#include "Cloud9CharacterShieldEffect.generated.h"


UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterShieldEffect : public UCloud9CharacterEffectTrait
{
	GENERATED_BODY()

public:
	static const FName ShieldEnableName;
	static const FName ShieldReflectName;
	static const FName ShieldPowerName;
	static const FName ShieldColorName;

	UCloud9CharacterShieldEffect();

protected:
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float Duration;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	FLinearColor Color;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	float Reflect;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	float Power;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	float ElapsedTime;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	bool IsEnabled;

	virtual bool IsExtinguished_Implementation() const override;

	virtual void OnApply_Implementation() override;

	virtual void OnRemove_Implementation() override;

	virtual bool CanApply_Implementation() const override;

	virtual bool CanTick_Implementation() const override;

	virtual void OnTick_Implementation(float DeltaSeconds) override;

	virtual void OnSkeletalMeshChanged_Implementation(bool bReinitPose) override;

private:
	void ToggleEffect(bool NewState);

	void ToggleVisual(bool NewState);
};

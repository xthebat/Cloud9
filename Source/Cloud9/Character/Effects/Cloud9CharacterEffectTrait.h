// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Cloud9CharacterEffectTrait.generated.h"

class UCloud9EffectsComponent;

UCLASS(Abstract, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterEffectTrait : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsExtinguished() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnApply();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRemove();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanApply() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanTick() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanDamaged() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnApplyDamage(float Damage);

	virtual bool IsExtinguished_Implementation() const;

	virtual void OnApply_Implementation();

	virtual void OnRemove_Implementation();

	virtual bool CanApply_Implementation() const;

	virtual bool CanTick_Implementation() const;

	virtual bool CanDamaged_Implementation() const;

	virtual void OnTick_Implementation(float DeltaSeconds);

	virtual void OnApplyDamage_Implementation(float Damage);

protected:
	const UCloud9EffectsComponent* GetContainer() const;
};

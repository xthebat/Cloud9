// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Cloud9CharacterEffectInterface.generated.h"

class UCloud9EffectsComponent;

UINTERFACE()
class UCloud9CharacterEffectInterface : public UInterface
{
	GENERATED_BODY()
};

class CLOUD9_API ICloud9CharacterEffectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsExtinguished() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool OnApply(UCloud9EffectsComponent* Container);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool OnRemove(UCloud9EffectsComponent* Container);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanApply(const UCloud9EffectsComponent* Container);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanTick();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanDamaged();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTick(const UCloud9EffectsComponent* Container, float DeltaSeconds);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnApplyDamage(const UCloud9EffectsComponent* Container, float Damage);
};

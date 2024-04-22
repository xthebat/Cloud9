// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Cloud9CharacterEffect.h"

#include "Cloud9CharacterShieldEffect.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9CharacterShieldEffect : public UActorComponent, public ICloud9CharacterEffect
{
	GENERATED_BODY()

public:
	UCloud9CharacterShieldEffect();

protected:
	virtual void BeginPlay() override;

	virtual bool Apply_Implementation(ACloud9Character* Character) override;

	virtual bool Remove_Implementation(ACloud9Character* Character) override;
};

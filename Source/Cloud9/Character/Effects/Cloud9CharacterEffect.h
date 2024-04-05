// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Cloud9CharacterEffect.generated.h"

class ACloud9Character;

UINTERFACE()
class UCloud9CharacterEffect : public UInterface
{
	GENERATED_BODY()
};


class CLOUD9_API ICloud9CharacterEffect
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Apply(ACloud9Character* Character);
};

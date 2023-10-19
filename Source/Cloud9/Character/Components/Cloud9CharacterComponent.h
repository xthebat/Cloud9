#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Cloud9CharacterComponent.generated.h"

class ACloud9Character;
class ACloud9PlayerController;

UINTERFACE()
class UCloud9CharacterComponent : public UInterface
{
	GENERATED_BODY()
};

class CLOUD9_API ICloud9CharacterComponent
{
	GENERATED_BODY()

public:
	ACloud9Character* GetPawn() const;

	ACloud9PlayerController* GetPlayerController() const;
};

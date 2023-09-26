#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9ControllerComponent.generated.h"


UINTERFACE()
class UCloud9ControllerComponent : public UInterface
{
	GENERATED_BODY()
};

class CLOUD9_API ICloud9ControllerComponent
{
	GENERATED_BODY()
public:
	ACloud9Character* GetPawn() const;

	ACloud9PlayerController* GetPlayerController() const;
};

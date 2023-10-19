#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cloud9Console.generated.h"

/**
 * May be inherited from UActorComponent for more easily access to World and replication.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CLOUD9_API UCloud9Console : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9Console();
};

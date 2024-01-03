// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9SoundPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CLOUD9_API UCloud9SoundPlayer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool PlaySingleSound(USoundBase* Sound, FVector Location, float Volume);
	static bool PlaySoundByName(const TMap<FName, USoundBase*>& Sounds, FName Name, FVector Location, float Volume);
	static bool PlayRandomSound(const TArray<USoundBase*>& Sounds, FVector Location, float Volume);
};

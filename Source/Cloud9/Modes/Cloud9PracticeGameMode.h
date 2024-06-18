// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameMode.h"
#include "Cloud9PracticeGameMode.generated.h"

UCLASS()
class CLOUD9_API ACloud9PracticeGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

	virtual void StartPlay() override;
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ConfigWidgetClass;

	
};

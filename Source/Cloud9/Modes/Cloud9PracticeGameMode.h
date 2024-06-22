// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameMode.h"
#include "Cloud9PracticeGameMode.generated.h"

class ACloud9PracticePlayerStart;

UCLASS(Blueprintable)
class CLOUD9_API ACloud9PracticeGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

public:
	ACloud9PracticeGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:
	ACloud9PracticePlayerStart* FindPlayerStartEx(const FString& IncomingName = TEXT("")) const;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ConfigWidgetClass;
};

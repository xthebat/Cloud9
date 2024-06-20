// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9GameMode.h"
#include "Cloud9PracticeGameMode.generated.h"

UCLASS()
class CLOUD9_API ACloud9PracticeGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

	static inline FString RangePreviewName = TEXT("RangePreview");
	static inline FString RangeStartName = TEXT("RangeStart");

	ACloud9PracticeGameMode();

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ConfigWidgetClass;
};

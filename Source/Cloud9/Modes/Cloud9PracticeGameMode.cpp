// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9PracticeGameMode.h"

#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"

ACloud9PracticeGameMode::ACloud9PracticeGameMode()
{
	bStartPlayersAsSpectators = true;
	// bDelayedStart = 0; // WTF?
}

void ACloud9PracticeGameMode::StartPlay()
{
	Super::StartPlay();
}

void ACloud9PracticeGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Maybe use HandleStartingNewPlayer?
	Super::PostLogin(NewPlayer);
	CRASH_IF_FAIL(IsValid(ConfigWidgetClass), "Starting widget class is invalid");
	let Widget = CreateWidget(NewPlayer, ConfigWidgetClass);
	Widget->AddToViewport();
	NewPlayer->SetInputMode(FInputModeUIOnly{});

	let Start = FindPlayerStart(NewPlayer, RangePreviewName);

	if (not IsValid(Start))
	{
		// TODO: Exit to main menu if map invalid for Practice GameMode
		OBJECT_ERROR("Can't load specified map with current game mode due to invalid start locations");
		return;
	}

	NewPlayer->GetSpectatorPawn()->SetActorLocationAndRotation(Start->GetActorLocation(), Start->GetActorRotation());
}

void ACloud9PracticeGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

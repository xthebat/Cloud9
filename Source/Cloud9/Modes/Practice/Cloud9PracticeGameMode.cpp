// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9PracticeGameMode.h"

#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpectatorPawn.h"

#include "Cloud9PracticePlayerStart.h"
#include "Cloud9/Hud/Cloud9HudBase.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Macro/Common.h"

ACloud9PracticeGameMode::ACloud9PracticeGameMode()
{
	IsTransferPawnsByDefault = false;
	IsInitializePawnsByDefault = true;
	bStartPlayersAsSpectators = true;
	// bDelayedStart = 0; // WTF?
}

void ACloud9PracticeGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Maybe use HandleStartingNewPlayer?
	Super::PostLogin(NewPlayer);

	CRASH_IF_FAIL(IsValid(ConfigWidgetClass), "Starting widget class is invalid");
	let Widget = CreateWidget(NewPlayer, ConfigWidgetClass);
	Widget->AddToViewport();
	NewPlayer->SetInputMode(FInputModeUIOnly{});

	if (let Hud = NewPlayer->GetHUD<ACloud9HudBase>(); IsValid(Hud))
	{
		Hud->SetGameHudEnabled(false);
		Hud->SetCrosshairEnabled(false);
	}

	let PlayerStart = FindPlayerStartEx();

	if (not IsValid(PlayerStart))
	{
		// TODO: Exit to a main menu if map invalid for Practice GameMode
		OBJECT_ERROR("Can't load specified map with current game mode due to invalid start locations");
		return;
	}

	FVector PreviewLocation = FVector::ZeroVector;
	FRotator PreviewRotation = FRotator::ZeroRotator;

	PlayerStart->GetPreviewLocationAndRotation(PreviewLocation, PreviewRotation);
	NewPlayer->GetSpectatorPawn()->SetActorLocationAndRotation(PreviewLocation, PreviewRotation);
}

void ACloud9PracticeGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

ACloud9PracticePlayerStart* ACloud9PracticeGameMode::FindPlayerStartEx(const FString& IncomingName) const
{
	var Sequence = TActorIterator<ACloud9PracticePlayerStart>(GetWorld()) | ETContainer::FromIterator{};
	if (not IncomingName.IsEmpty())
	{
		let Name = FName(IncomingName);
		return Sequence | ETContainer::FirstOrNullByPredicate{
			[&Name](let& It) { return It.PlayerStartTag == Name; }
		};
	}

	return Sequence | ETContainer::FirstOrNull{};
}

AActor* ACloud9PracticeGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	return FindPlayerStartEx(IncomingName);
}

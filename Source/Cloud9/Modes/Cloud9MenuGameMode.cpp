// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9MenuGameMode.h"

#include "Cloud9/Hud/Cloud9HudBase.h"

ACloud9MenuGameMode::ACloud9MenuGameMode()
{
	IsTransferPawnsByDefault = true;
	IsInitializePawnsByDefault = true;
}

void ACloud9MenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (let Hud = NewPlayer->GetHUD<ACloud9HudBase>(); IsValid(Hud))
	{
		Hud->SetGameHudEnabled(true);
		Hud->SetCrosshairEnabled(false);
	}
}

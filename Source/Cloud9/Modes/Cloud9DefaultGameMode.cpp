// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Hud/Cloud9HudBase.h"

ACloud9DefaultGameMode::ACloud9DefaultGameMode()
{
	IsTransferPawnsByDefault = true;
	IsInitializePawnsByDefault = true;
}

void ACloud9DefaultGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (let Hud = NewPlayer->GetHUD<ACloud9HudBase>(); IsValid(Hud))
	{
		Hud->SetGameHudEnabled(true);
		Hud->SetCrosshairEnabled(true);
	}
}

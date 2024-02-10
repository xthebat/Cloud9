// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

void ACloud9DefaultGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	let GameInstance = GetGameInstance<UCloud9GameInstance>();

	if (not IsValid(GameInstance))
	{
		log(Error, "GameInstance isn't valid");
		return;
	}

	let Character = Cast<ACloud9Character>(NewPlayer->GetCharacter());

	if (not IsValid(Character))
	{
		log(Error, "Character isn't valid");
	}
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

ACloud9DefaultGameMode::ACloud9DefaultGameMode()
{
	InitialWeaponSlot = EWeaponSlot::NotSelected;
}

bool ACloud9DefaultGameMode::OnWorldLoadComplete(FPlayerSavedInfo& SavedInfo)
{
	return true;
}

bool ACloud9DefaultGameMode::OnWorldTearDown(FPlayerSavedInfo& SavedInfo)
{
	let GameInstance = GetGameInstance<UCloud9GameInstance>();

	if (not IsValid(GameInstance))
	{
		log(Error, "GameInstance isn't valid");
		return false;
	}

	// GameInstance->GetLocalPlayers() | ETContainer::ForEachIndexed{
	// 	[this, &SavedInfo](var Index, let LocalPlayer)
	// 	{
	// 		let PlayerController = LocalPlayer->GetPlayerController(GetWorld());
	//
	// 		if (not IsValid(PlayerController))
	// 		{
	// 			log(Error, "PlayerController isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 			continue;
	// 		}
	//
	// 		let Character = PlayerController->template GetPawn<ACloud9Character>();
	//
	// 		if (not IsValid(Character))
	// 		{
	// 			log(Error, "Character isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 			continue;
	// 		}
	//
	// 		SavedInfo.WeaponConfigs[Index] = FWeaponConfig::FromWeapon(Character);
	// 	}
	// };

	let MyWorld = GetWorld();

	let LocalPlayer = GameInstance->GetLocalPlayers()[0];
	let PlayerController = LocalPlayer->GetPlayerController(MyWorld);
	let Character = PlayerController->GetPawn<ACloud9Character>();

	let q = Character->GetInventory()->GetWeapons()
		| ETContainer::Transform{[](let Weapon) { return 1; }}
		| ETContainer::ToArray{};

	// SavedInfo = GameInstance->GetLocalPlayers() | ETContainer::Associate{
	// 	[](let LocalPlayer) { return LocalPlayer->GetControllerId(); },
	// 	[MyWorld](let LocalPlayer)
	// 	{
	// 		let PlayerController = LocalPlayer->GetPlayerController(MyWorld);
	//
	// 		if (not IsValid(PlayerController))
	// 		{
	// 			log(Fatal, "PlayerController isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 			return {};
	// 		}
	//
	// 		let Character = PlayerController->template GetPawn<ACloud9Character>();
	// 		if (not IsValid(Character))
	// 		{
	// 			log(Fatal, "Character isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 			return {};
	// 		}
	//
	// 		return 
	// 	}
	// };

	// for (let LocalPlayer : GameInstance->GetLocalPlayers())
	// {
	// 	let PlayerController = LocalPlayer->GetPlayerController(GetWorld());
	//
	// 	if (not IsValid(PlayerController))
	// 	{
	// 		log(Error, "PlayerController isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 		continue;
	// 	}
	//
	// 	let Character = PlayerController->GetPawn<ACloud9Character>();
	//
	// 	if (not IsValid(Character))
	// 	{
	// 		log(Error, "Character isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
	// 		continue;
	// 	}
	//
	// 	Character->GetInventory()->
	//
	// 	           SavedInfo.WeaponConfigs[PlayerController->Get]
	// }

	return true;
}

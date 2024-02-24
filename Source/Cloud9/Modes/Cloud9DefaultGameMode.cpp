// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"

FName ACloud9DefaultGameMode::CtSidePlayer = TEXT("CT");
FName ACloud9DefaultGameMode::TSidePlayer = TEXT("T");
FName ACloud9DefaultGameMode::GodSidePlayer = TEXT("God");

ACloud9DefaultGameMode::ACloud9DefaultGameMode() {}

ACloud9Character* GetPlayerCharacter(const ULocalPlayer* LocalPlayer)
{
	let World = LocalPlayer->GetWorld();

	let PlayerController = LocalPlayer->GetPlayerController(World);

	if (not IsValid(PlayerController))
	{
		log(Fatal, "PlayerController isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
		return nullptr;
	}

	let Character = PlayerController->GetPawn<ACloud9Character>();
	if (not IsValid(Character))
	{
		log(Fatal, "Character isn't valid for LocalPlayer=%s", *LocalPlayer->GetName());
		return nullptr;
	}

	return Character;
}

bool ACloud9DefaultGameMode::OnWorldStart(FSavedInfo& SavedInfo)
{
	let GameInstance = GetCloud9GameInstance();

	if (SavedInfo.bIsLoadRequired)
	{
		GameInstance->GetLocalPlayers()
			| ETContainer::Filter{
				[&SavedInfo](let LocalPlayer)
				{
					return SavedInfo.Players.Contains(LocalPlayer->GetControllerId());
				}
			}
			| ETContainer::ForEach{
				[this, &SavedInfo](let LocalPlayer)
				{
					let Character = GetPlayerCharacter(LocalPlayer);
					let Inventory = Character->GetInventoryComponent();
					let& PlayerSavedInfo = SavedInfo.Players[LocalPlayer->GetControllerId()];
					Inventory->Initialize(PlayerSavedInfo.WeaponConfigs, PlayerSavedInfo.WeaponSlot);
				}
			};

		SavedInfo.Reset();
	}
	else
	{
		if (not InitialPlayerConfig.Contains(GodSidePlayer))
		{
			log(Error, "Can't initialize player for now...")
			return false;
		}

		let& PlayerConfig = InitialPlayerConfig[GodSidePlayer];

		GameInstance->GetLocalPlayers()
			| ETContainer::ForEach{
				[&PlayerConfig](let LocalPlayer)
				{
					let Character = GetPlayerCharacter(LocalPlayer);
					let Inventory = Character->GetInventoryComponent();
					let Health = Character->GetHealthComponent();

					Inventory->Initialize(PlayerConfig.WeaponConfigs, PlayerConfig.WeaponSlot);
					Health->Initialize(PlayerConfig.HealthConfig);
				}
			};
	}

	return true;
}

bool ACloud9DefaultGameMode::OnWorldTearDown(FSavedInfo& SavedInfo)
{
	let GameInstance = GetCloud9GameInstance();

	SavedInfo.Players = GameInstance->GetLocalPlayers()
		| ETContainer::Associate{
			[](let LocalPlayer) { return LocalPlayer->GetControllerId(); },
			[](let LocalPlayer)
			{
				var PlayerSavedInfo = FPlayerSavedInfo();
				let Character = GetPlayerCharacter(LocalPlayer);
				let Inventory = Character->GetInventoryComponent();

				PlayerSavedInfo.WeaponConfigs = Inventory->GetWeapons()
					| ETContainer::Filter{[](let Weapon) { return IsValid(Weapon); }}
					| ETContainer::Transform{[](let Weapon) { return FWeaponConfig::FromWeapon(Weapon); }}
					| ETContainer::ToArray{};

				if (let SelectedWeapon = Inventory->GetSelectedWeapon(); IsValid(SelectedWeapon))
				{
					PlayerSavedInfo.WeaponSlot = SelectedWeapon->GetWeaponSlot();
				}

				return PlayerSavedInfo;
			}
		};

	SavedInfo.bIsLoadRequired = true;

	return true;
}

// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Character/Cloud9Character.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "EngineUtils.h"
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"

FName ACloud9DefaultGameMode::PlayerConfigName = TEXT("God");
FName ACloud9DefaultGameMode::BotConfigName = TEXT("Bot");

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
		// TODO: Rework initialization process

		let PlayerConfig = InitialPlayerConfig.Find(PlayerConfigName);
		let BotConfig = InitialPlayerConfig.Find(BotConfigName);

		let MyWorld = GameInstance->GetWorld();
		if (not IsValid(MyWorld))
		{
			log(Error, "World isn't exist now -> can't initialize players and bots")
			return false;
		}

		TActorIterator<ACloud9Character>(MyWorld)
			| ETContainer::FromIterator{}
			| ETContainer::ForEach{
				[PlayerConfig, BotConfig](var& Character)
				{
					if (let Config = Character.IsBotControlled() ? BotConfig : PlayerConfig; Config == nullptr)
					{
						log(Warning, "[Character='%s'] Initialization skipped cus config wasn't specified",
						    *Character.GetName());
						return;
					}

					let Inventory = Character.GetInventoryComponent();
					let Health = Character.GetHealthComponent();

					Inventory->Initialize(PlayerConfig->WeaponConfigs, PlayerConfig->WeaponSlot);
					Health->Initialize(PlayerConfig->HealthConfig);

					PlayerConfig->Effects | ETContainer::ForEach{
						[&Character](let EffectClass) { Character.AddCharacterEffect(EffectClass); }
					};
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

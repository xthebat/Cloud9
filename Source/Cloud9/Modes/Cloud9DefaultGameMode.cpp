// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9DefaultGameMode.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Character/Cloud9Character.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Cloud9/Character/Components/Cloud9HealthComponent.h"
#include "Cloud9/Tools/Extensions/ACharacter.h"

FName ACloud9DefaultGameMode::PlayerConfigName = TEXT("God");
FName ACloud9DefaultGameMode::BotConfigName = TEXT("Bot");

ACloud9DefaultGameMode::ACloud9DefaultGameMode() {}

void ACloud9DefaultGameMode::SaveCharacter(const ACloud9Character* Character)
{
	GetCloud9GameInstance()->SaveCharacterInfo(Character);
}

void ACloud9DefaultGameMode::LoadCharacter(ACloud9Character* Character)
{
	if (Character->GetNeedInitialize())
	{
		let GameInstance = GetCloud9GameInstance();

		if (GameInstance->HasCharacterInfo(Character) and Character->IsPlayerControlled())
		{
			GameInstance->LoadCharacterInfo(Character);
		}
		else
		{
			InitializeCharacter(Character);
		}
	}
}

void ACloud9DefaultGameMode::InitializeCharacter(ACloud9Character* Character)
{
	let PlayerConfig = InitialPlayerConfig.Find(PlayerConfigName);
	let BotConfig = InitialPlayerConfig.Find(BotConfigName);

	let Config = Character->IsPlayerControlled() ? PlayerConfig : BotConfig;
	AssertOrVoid(Config, Warning, "Initialization skipped cus config wasn't specified for '%s'", *Character->GetName());

	let Inventory = Character->GetInventoryComponent();
	let Health = Character->GetHealthComponent();

	Inventory->Initialize(Config->WeaponConfigs, Config->WeaponSlot);
	Health->Initialize(Config->HealthConfig);

	Character->RemoveAllCharacterEffects();

	Config->Effects | ETContainer::ForEach{
		[Character](let EffectClass) { Character->AddCharacterEffect(EffectClass); }
	};
}

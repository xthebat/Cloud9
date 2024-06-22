// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "Cloud9GameMode.h"

#include "EngineUtils.h"
#include "Cloud9/Game/Cloud9GameState.h"
#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Contollers//Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Cloud9SpectatorPawn.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"

ACloud9GameMode::ACloud9GameMode()
{
	IsTransferPawnsByDefault = false;
	IsInitializePawnsByDefault = false;
	PlayerControllerClass = ACloud9PlayerController::StaticClass();
	SpectatorClass = ACloud9SpectatorPawn::StaticClass();
	DefaultPawnClass = ACloud9Character::StaticClass();
	GameStateClass = ACloud9GameState::StaticClass();
}

void ACloud9GameMode::SaveCharacter(const ACloud9Character* Character)
{
	GetCloud9GameInstance()->SaveCharacterInfo(Character);
}

void ACloud9GameMode::LoadCharacter(ACloud9Character* Character)
{
	if (Character->GetNeedInitialize())
	{
		if (IsTransferPawnsByDefault)
		{
			let GameInstance = GetCloud9GameInstance();
			if (Character->IsPlayerControlled() and IsValid(GameInstance) and GameInstance->HasCharacterInfo(Character))
			{
				GameInstance->LoadCharacterInfo(Character);
				return;
			}
		}

		if (IsInitializePawnsByDefault)
		{
			InitializeCharacter(Character);
		}
	}
}

void ACloud9GameMode::InitializeCharacter(ACloud9Character* Character)
{
	let PlayerConfig = InitialPlayerConfig.Find(PlayerConfigName);
	let BotConfig = InitialPlayerConfig.Find(BotConfigName);

	let Config = Character->IsPlayerControlled() ? PlayerConfig : BotConfig;
	OBJECT_VOID_IF_FAIL(
		Config, Warning,
		"Initialization skipped cus config wasn't specified for '%s'", *Character->GetName());

	let Inventory = Character->GetInventoryComponent();
	let Health = Character->GetHealthComponent();

	Inventory->Initialize(Config->WeaponConfigs, Config->WeaponSlot);
	Health->Initialize(Config->HealthConfig);

	Character->RemoveAllCharacterEffects();

	Config->Effects | ETContainer::ForEach{
		[Character](let EffectClass) { Character->AddCharacterEffect(EffectClass); }
	};
}

void ACloud9GameMode::StartPlay()
{
	Super::StartPlay();
}

void ACloud9GameMode::StartToLeaveMap()
{
	// TODO: Maybe move to a delegate pattern
	TActorIterator<ACloud9Character>(GetWorld())
		| ETContainer::FromIterator{}
		| ETContainer::ForEach{
			[](let& It) { It.OnLevelChanged(); }
		};

	if (let MyWorld = GetWorld(); MyWorld != nullptr)
	{
		OBJECT_VERBOSE("Cleanup world timers = %p", this);
		MyWorld | EUWorld::ClearAllTimers{};
	}

	Super::StartToLeaveMap();
}

UCloud9GameInstance* ACloud9GameMode::GetCloud9GameInstance() const
{
	let GameInstance = GetGameInstance<UCloud9GameInstance>();
	OBJECT_RETURN_IF_FAIL(IsValid(GameInstance), nullptr, Fatal, "GameInstance is invalid (required Cloud9 class)");
	return GameInstance;
}

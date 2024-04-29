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

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Cloud9/Structures/SavedInfo.h"
#include "Cloud9/Weapon/Enums/WeaponSlot.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"

#include "Cloud9GameInstance.generated.h"

class ACloud9GameMode;

UCLASS()
class CLOUD9_API UCloud9GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/**
	 * Function gets game mode for specified World
	 * 
	 * WARNING: Can only be used during game (avoid it in OnConstruction, etc.)
	 */
	static ACloud9GameMode* GetGameMode(const UWorld* World);

	/**
	 * Function gets a specialized type of game mode for specified World
	 * 
	 * WARNING: Can only be used during game (avoid it in OnConstruction, etc.)
	 */
	template <typename GameModeType = ACloud9GameMode>
	static GameModeType* GetGameMode(const UWorld* World) { return Cast<GameModeType>(GetGameMode(World)); }

	/**
	 * Function gets current game mode
	 * 
	 * WARNING: Can only be used during game (avoid it in OnConstruction, etc.)
	 */
	ACloud9GameMode* GetGameMode() const { return GetGameMode(GetWorld()); }

	/**
	 * Function gets a specialized type of current game mode
	 * 
	 * WARNING: Can only be used during game (avoid it in OnConstruction, etc.)
	 */
	template <typename GameModeType = ACloud9GameMode>
	GameModeType* GetGameMode() const { return Cast<GameModeType>(GetGameMode()); }

	friend class ACloud9GameMode;

protected:
	UPROPERTY()
	FSavedInfo SavedInfo;

	// TODO: Remove DefaultWeaponsConfig and InitialWeaponSlot from GameInstance

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	TArray<FWeaponConfig> DefaultWeaponsConfig;

	UPROPERTY(Category=Weapon, EditDefaultsOnly)
	EWeaponSlot InitialWeaponSlot;
};

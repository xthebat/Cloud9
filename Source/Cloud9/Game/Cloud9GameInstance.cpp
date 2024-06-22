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

#include "Cloud9GameInstance.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Character/Components/Cloud9InventoryComponent.h"
#include "Cloud9/Tools/Extensions/ACharacter.h"

// TODO: Add multiplayer save-load game (!!!)

void UCloud9GameInstance::SaveCharacterInfo(const ACloud9Character* Character)
{
	using namespace EACharacter;

	if (let PlayerId = Character | GetPlayerId{}; IsPlayerIdValid(PlayerId))
	{
		var PlayerSavedInfo = FPlayerSavedInfo();

		let Inventory = Character->GetInventoryComponent();

		PlayerSavedInfo.WeaponConfigs = Inventory->GetWeapons()
			| ETContainer::Filter{[](let Weapon) { return IsValid(Weapon); }}
			| ETContainer::Transform{[](let Weapon) { return FWeaponConfig::FromWeapon(Weapon); }}
			| ETContainer::ToArray{};

		if (let SelectedWeapon = Inventory->GetSelectedWeapon(); IsValid(SelectedWeapon))
		{
			PlayerSavedInfo.WeaponSlot = SelectedWeapon->GetWeaponSlot();
		}

		SavedInfo.Player = PlayerSavedInfo;
		SavedInfo.IsPlayerStored = true;
	}
}

void UCloud9GameInstance::LoadCharacterInfo(const ACloud9Character* Character) const
{
	using namespace EACharacter;

	if (HasCharacterInfo(Character))
	{
		let Inventory = Character->GetInventoryComponent();
		// let& PlayerSavedInfo = SavedInfo.Players[Character->GetFName()];
		let& PlayerSavedInfo = SavedInfo.Player;
		Inventory->Initialize(PlayerSavedInfo.WeaponConfigs, PlayerSavedInfo.WeaponSlot);
	}
}

bool UCloud9GameInstance::HasCharacterInfo(const ACloud9Character* Character) const
{
	using namespace EACharacter;

	let PlayerId = Character | GetPlayerId{};
	return SavedInfo.IsPlayerStored and IsPlayerIdValid(PlayerId);
}

USoundAttenuation* UCloud9GameInstance::GetAttenuationSettings() const
{
	return AttenuationSettings;
}

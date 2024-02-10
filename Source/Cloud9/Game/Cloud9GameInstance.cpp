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
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

const TArray<FWeaponConfig>& UCloud9GameInstance::GetDefaultWeaponsConfig() const { return DefaultWeaponsConfig; }

EWeaponSlot UCloud9GameInstance::GetInitialWeaponSlot() const { return InitialWeaponSlot; }

void UCloud9GameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	if (IsValid(OldWorld))
	{
		log(Verbose, "Cleanup world timers = %p", OldWorld);
		OldWorld | EUWorld::ClearAllTimers{};
	}

	for (let LocalPlayer : GetLocalPlayers())
	{
		let PlayerController = LocalPlayer->GetPlayerController(OldWorld);
		if (not IsValid(PlayerController))
		{
			log(Error, "PlayerController isn't valid");
			continue;
		}

		let Character = Cast<ACloud9Character>(PlayerController->GetCharacter());
		if (not IsValid(Character))
		{
			log(Error, "Character isn't valid");
			continue;
		}

		for (let Weapon : Character->GetInventory()->GetWeapons())
		{
			FWeaponConfig::FromWeapon(Weapon);
		}
	}

	Super::OnWorldChanged(OldWorld, NewWorld);
}

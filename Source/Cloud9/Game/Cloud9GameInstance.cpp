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

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Modes/Cloud9GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

ACloud9GameMode* UCloud9GameInstance::GetGameMode(const UWorld* World)
{
	if (not IsValid(World))
	{
		log(Error, "World isn't valid to get GameMode")
		return nullptr;
	}

	let MyGameMode = UGameplayStatics::GetGameMode(World);

	if (not IsValid(MyGameMode))
	{
		log(Error, "Current GameMode isn't valid")
		return nullptr;
	}

	return Cast<ACloud9GameMode>(MyGameMode);
}

void UCloud9GameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	FWorldDelegates::OnWorldBeginTearDown.AddStatic(&OnWorldBeginTearDown);
}

void UCloud9GameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);

	if (NewWorld != nullptr)
	{
		let WorldGameMode = GetGameMode(NewWorld);
		log(Display, "World = %p WorldGameMode = %p", NewWorld, WorldGameMode);

		let GameInstance = NewWorld->GetGameInstance<UCloud9GameInstance>();

		if (not GameInstance)
		{
			log(Error, "GameInstance isn't valid tear down Map='%s'", *NewWorld->GetMapName());
			return;
		}

		if (not WorldGameMode->OnWorldChanged(GameInstance->SavedInfo))
		{
			log(Error, "GameMode OnMapLoadComplete failure");
			return;
		}

		log(Display, "Successfully load Map='%s'", *NewWorld->GetMapName());
	}
}

void UCloud9GameInstance::OnWorldBeginTearDown(UWorld* World)
{
	log(Verbose, "Cleanup world timers = %p", World);
	World | EUWorld::ClearAllTimers{};

	let WorldGameMode = GetGameMode(World);
	log(Display, "World = %p WorldGameMode = %p", World, WorldGameMode);

	let GameInstance = World->GetGameInstance<UCloud9GameInstance>();

	if (not GameInstance)
	{
		log(Error, "GameInstance isn't valid tear down Map='%s'", *World->GetMapName());
		return;
	}

	if (not WorldGameMode->OnWorldTearDown(GameInstance->SavedInfo))
	{
		log(Error, "GameMode->OnWorldTearDown failure for Map='%s'", *World->GetMapName());
		return;
	}

	log(Display, "Successfully tear down Map='%s'", *World->GetMapName());
}

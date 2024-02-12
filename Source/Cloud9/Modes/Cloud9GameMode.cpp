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

#include "Cloud9/Game/Cloud9GameState.h"
#include "Cloud9/Contollers//Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9GameInstance.h"

ACloud9GameMode::ACloud9GameMode()
{
	PlayerControllerClass = ACloud9PlayerController::StaticClass();
	DefaultPawnClass = ACloud9Character::StaticClass();
	GameStateClass = ACloud9GameState::StaticClass();
}

bool ACloud9GameMode::OnWorldStart(FSavedInfo& SavedInfo)
{
	return true;
}

bool ACloud9GameMode::OnWorldTearDown(FSavedInfo& SavedInfo)
{
	return true;
}

void ACloud9GameMode::StartPlay()
{
	Super::StartPlay();
	OnWorldStart(GetCloud9GameInstance()->SavedInfo);
}

void ACloud9GameMode::StartToLeaveMap()
{
	Super::StartToLeaveMap();

	if (let MyWorld = GetWorld(); MyWorld != nullptr)
	{
		log(Verbose, "Cleanup world timers = %p", this);
		MyWorld | EUWorld::ClearAllTimers{};
	}

	OnWorldTearDown(GetCloud9GameInstance()->SavedInfo);
}

UCloud9GameInstance* ACloud9GameMode::GetCloud9GameInstance() const
{
	let GameInstance = GetGameInstance<UCloud9GameInstance>();

	if (not IsValid(GameInstance))
	{
		log(Fatal, "GameInstance isn't valid");
		return nullptr;
	}

	return GameInstance;
}

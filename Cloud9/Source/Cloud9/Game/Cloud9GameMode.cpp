// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cloud9GameMode.h"
#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "UObject/ConstructorHelpers.h"

ACloud9GameMode::ACloud9GameMode()
{
	PlayerControllerClass = ACloud9PlayerController::StaticClass();
	DefaultPawnClass = ACloud9Character::StaticClass();
}
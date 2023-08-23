// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cloud9GameMode.h"
#include "Cloud9PlayerController.h"
#include "Cloud9Character.h"
#include "UObject/ConstructorHelpers.h"

ACloud9GameMode::ACloud9GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACloud9PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
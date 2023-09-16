// Fill out your copyright notice in the Description page of Project Settings.


#include "Cloud9Console.h"
#include "Cloud9GameMode.h"
#include "Kismet/GameplayStatics.h"

ACloud9Console::ACloud9Console()
{
	// SetIsReplicatedByDefault(true);

	for (TFieldIterator<UFunction> It(GetClass()); It; ++It)
	{
		// Make sure the function belongs to us instead of UActorComponent.
		// Its functions will show up here since we extended it.
		if (GetClass()->IsChildOf(Cast<UClass>(It.GetStruct())))
		{
			It->FunctionFlags |= FUNC_Exec;
		}
	}
}

UWorld* ACloud9Console::GetMyWorld()
{
	return GEngine->GameViewport->GetWorld();
}

ACloud9GameMode* ACloud9Console::GetMyGameMode()
{
	return Cast<ACloud9GameMode>(UGameplayStatics::GetGameMode(GetMyWorld()));
}

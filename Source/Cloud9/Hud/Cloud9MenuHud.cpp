// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9MenuHud.h"

#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Tools/Macro/Common.h"

void ACloud9MenuHud::BeginPlay()
{
	Super::BeginPlay();

	if (let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController()))
	{
		Controller->ShowMouseCursor(true);
	}
}

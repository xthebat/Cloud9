// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9SpectatorPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACloud9SpectatorPawn::ACloud9SpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);
	CameraComponent->SetupAttachment(RootComponent, USpringArmComponent::SocketName);
}

// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9PracticePlayerStart.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


ACloud9PracticePlayerStart::ACloud9PracticePlayerStart(const FObjectInitializer& ObjectInitializer)
	: APlayerStart(ObjectInitializer)
{
	PreviewSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(PreviewSpringArmComponentName);
	PreviewSpringArmComponent->SetupAttachment(RootComponent);
	PreviewSpringArmComponent->SetRelativeRotation({-45.0f, 45.0f, 0.0f});
	PreviewSpringArmComponent->TargetArmLength = 150.0f;

	PreviewCameraComponent = CreateDefaultSubobject<UCameraComponent>(PreviewCameraComponentName);
	PreviewCameraComponent->SetupAttachment(PreviewSpringArmComponent, USpringArmComponent::SocketName);
}

void ACloud9PracticePlayerStart::GetPreviewLocationAndRotation(FVector& Location, FRotator& Rotator) const
{
	Location = PreviewCameraComponent->GetComponentLocation();
	Rotator = PreviewCameraComponent->GetComponentRotation();
}

void ACloud9PracticePlayerStart::GetPlayerStartLocationAndRotation(FVector& Location, FRotator& Rotator) const
{
	Location = GetActorLocation();
	Rotator = GetActorRotation();
}

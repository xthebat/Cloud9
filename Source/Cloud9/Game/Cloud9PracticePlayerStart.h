// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Cloud9PracticePlayerStart.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable, ClassGroup=Common, hidecategories=Collision)
class CLOUD9_API ACloud9PracticePlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	static inline FName PreviewSpringArmComponentName = TEXT("PreviewSpringArmComponent");
	static inline FName PreviewCameraComponentName = TEXT("PreviewCameraComponent");

	ACloud9PracticePlayerStart(const FObjectInitializer& ObjectInitializer);

	void GetPreviewLocationAndRotation(FVector& Location, FRotator& Rotator) const;

	void GetPlayerStartLocationAndRotation(FVector& Location, FRotator& Rotator) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	USpringArmComponent* PreviewSpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	UCameraComponent* PreviewCameraComponent;
};

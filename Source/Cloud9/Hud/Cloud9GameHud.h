// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Cloud9GameHud.generated.h"

UCLASS()
class CLOUD9_API ACloud9GameHud : public AHUD
{
	GENERATED_BODY()

public:
	static inline const FName CrosshairLengthName = TEXT("Length");
	static inline const FName CrosshairWidthName = TEXT("Width");
	static inline const FName CrosshairGapName = TEXT("Gap");
	static inline const FName CrosshairColorName = TEXT("Color");

	ACloud9GameHud();

	virtual void BeginPlay() override;
	bool SetupCrosshair(float Length, float Width, float Gap, FVector Color) const;
	void DrawCrosshair();

	virtual void DrawHUD() override;

protected:
	UPROPERTY()
	bool IsCrosshairEnabled;

	UPROPERTY()
	UMaterialInstanceDynamic* CrosshairMaterial;
};

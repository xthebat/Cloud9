// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Cloud9HudBase.generated.h"

class ACloud9PlayerController;
class UWidgetComponent;

UCLASS()
class CLOUD9_API ACloud9HudBase : public AHUD
{
	GENERATED_BODY()

public:
	static inline const FName CrosshairLengthName = TEXT("Length");
	static inline const FName CrosshairWidthName = TEXT("Width");
	static inline const FName CrosshairGapName = TEXT("Gap");
	static inline const FName CrosshairColorName = TEXT("Color");

	static inline const FName GameWidgetComponentName = TEXT("GameWidgetComponent");
	static inline const FName GameWidgetName = TEXT("GameWidget");

	ACloud9HudBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable)
	void SetCrosshairEnabled(bool IsEnabled);

	UFUNCTION(BlueprintCallable)
	void SetGameHudEnabled(bool IsEnabled);

protected:
	ACloud9PlayerController* GetCloud9PlayerController() const;

	bool SetupCrosshair(float Length, float Width, float Gap, FVector Color) const;

	void DrawCrosshair();

	UPROPERTY()
	bool IsCrosshairEnabled;

	UPROPERTY()
	UMaterialInstanceDynamic* CrosshairMaterial;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> GameWidgetClass;

	UPROPERTY()
	UWidgetComponent* GameWidgetComponent;
};

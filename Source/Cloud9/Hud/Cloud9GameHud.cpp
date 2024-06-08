// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9GameHud.h"

#include "Cloud9/Contollers/Cloud9MouseController.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Engine/Canvas.h"

ACloud9GameHud::ACloud9GameHud()
{
	IsCrosshairEnabled = true;
	CrosshairMaterial = nullptr;
}

void ACloud9GameHud::BeginPlay()
{
	Super::BeginPlay();

	if (let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController()))
	{
		let MouseControllerComponent = Controller->GetMouseControllerComponent();

		if (let Crosshair = MouseControllerComponent->GetCrosshairMaterial())
		{
			CrosshairMaterial = UMaterialInstanceDynamic::Create(Crosshair, this);
		}
	}
}

bool ACloud9GameHud::SetupCrosshair(float Length, float Width, float Gap, FVector Color) const
{
	if (IsValid(CrosshairMaterial))
	{
		CrosshairMaterial->SetScalarParameterValue(CrosshairLengthName, Length);
		CrosshairMaterial->SetScalarParameterValue(CrosshairWidthName, Width);
		CrosshairMaterial->SetScalarParameterValue(CrosshairGapName, Gap);
		CrosshairMaterial->SetVectorParameterValue(CrosshairColorName, Color);
		return true;
	}

	return false;
}

void ACloud9GameHud::DrawCrosshair()
{
	static let Settings = UCloud9DeveloperSettings::Get();

	if (let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController()); IsValid(Controller))
	{
		let MousePosition = Controller->GetMouseControllerComponent()->GetSensitivityMousePosition();

		int32 ViewPortX = 0;
		int32 ViewPortY = 0;

		Controller->GetViewportSize(ViewPortX, ViewPortY);

		if (SetupCrosshair(
			Settings->CrosshairLength,
			Settings->CrosshairWidth,
			Settings->CrosshairGap,
			Settings->CrosshairColor))
		{
			let Size = Settings->CrosshairSize;

			let ScreenX = (Canvas->SizeX - ViewPortX) / 2.0f + MousePosition.X - Size / 2.0f;
			let ScreenY = (Canvas->SizeY - ViewPortY) / 2.0f + MousePosition.Y - Size / 2.0f;

			DrawMaterial(CrosshairMaterial, ScreenX, ScreenY, Size, Size, 0, 0, 1, 1);
		}
	}
}

void ACloud9GameHud::DrawHUD()
{
	Super::DrawHUD();

	if (IsCrosshairEnabled)
	{
		DrawCrosshair();
	}
}

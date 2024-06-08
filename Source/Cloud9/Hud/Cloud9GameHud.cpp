// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9GameHud.h"

#include "Cloud9/Contollers/Cloud9MouseController.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Engine/Canvas.h"

void ACloud9GameHud::BeginPlay()
{
	Super::BeginPlay();

	if (let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController()))
	{
		Controller->ShowMouseCursor(false);
	}
}

void ACloud9GameHud::DrawHUD()
{
	Super::DrawHUD();

	FVector2D MousePosition = FVector2D::ZeroVector;

	if (let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController()))
	{
		int32 ViewPortX = 0;
		int32 ViewPortY = 0;

		Controller->GetViewportSize(ViewPortX, ViewPortY);

		OBJECT_VOID_IF_FAIL(
			Controller->GetMousePosition(MousePosition.X, MousePosition.Y),
			Error, "Can't get mouse position"
		);

		let MouseControllerComponent = Controller->GetMouseControllerComponent();

		if (let Crosshair = MouseControllerComponent->GetCrosshairMaterial())
		{
			let Size = MouseControllerComponent->GetCrosshairSize();

			let ScreenX = (Canvas->SizeX - ViewPortX) / 2.0f + MousePosition.X - Size / 2.0f;
			let ScreenY = (Canvas->SizeY - ViewPortY) / 2.0f + MousePosition.Y - Size / 2.0f;

			DrawMaterial(Crosshair, ScreenX, ScreenY, Size, Size, 0, 0, 1, 1);
		}
	}
}

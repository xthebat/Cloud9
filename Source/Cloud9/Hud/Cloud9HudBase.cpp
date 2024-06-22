// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9HudBase.h"

#include "Blueprint/UserWidget.h"
#include "Cloud9/Contollers/Cloud9MouseController.h"
#include "Cloud9/Contollers/Cloud9PlayerController.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Components/WidgetComponent.h"
#include "Engine/Canvas.h"

ACloud9HudBase::ACloud9HudBase()
{
	IsCrosshairEnabled = false;
	CrosshairMaterial = nullptr;

	GameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(GameWidgetComponentName);
	GameWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// GameWidgetComponent->SetupAttachment(RootComponent);
	RootComponent = GameWidgetComponent;
}

void ACloud9HudBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	OBJECT_VOID_IF_FAIL(IsValid(GameWidgetClass), Error, "GameWidgetClass wasn't specified");
	GameWidgetComponent->SetWidgetClass(GameWidgetClass);
}

void ACloud9HudBase::BeginPlay()
{
	Super::BeginPlay();

	let Controller = GetCloud9PlayerController();
	let MouseControllerComponent = Controller->GetMouseControllerComponent();
	if (let Crosshair = MouseControllerComponent->GetCrosshairMaterial())
	{
		CrosshairMaterial = UMaterialInstanceDynamic::Create(Crosshair, this);
	}
}

void ACloud9HudBase::DrawHUD()
{
	Super::DrawHUD();

	if (IsCrosshairEnabled)
	{
		DrawCrosshair();
	}
}

void ACloud9HudBase::SetCrosshairEnabled(bool IsEnabled)
{
	IsCrosshairEnabled = IsEnabled;
	GetCloud9PlayerController()->ShowMouseCursor(not IsEnabled);
}

void ACloud9HudBase::SetGameHudEnabled(bool IsEnabled)
{
	OBJECT_VOID_IF_FAIL(IsValid(GameWidgetComponent), Error, "GameWidgetComponent is invalid");

	// Pre-initialize widget to make possible enable it on PostLogin  
	GameWidgetComponent->InitWidget();

	let Widget = GameWidgetComponent->GetWidget();
	OBJECT_VOID_IF_FAIL(IsValid(Widget), Error, "Can't get widget from GameWidgetComponent");

	if (IsEnabled)
	{
		Widget->AddToViewport();
	}
	else
	{
		Widget->RemoveFromParent();
	}
}

ACloud9PlayerController* ACloud9HudBase::GetCloud9PlayerController() const
{
	let Controller = Cast<ACloud9PlayerController>(GetOwningPlayerController());
	CRASH_IF_FAIL(IsValid(Controller), "Owning controller is invalid");
	return Controller;
}

bool ACloud9HudBase::SetupCrosshair(float Length, float Width, float Gap, FVector Color) const
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

void ACloud9HudBase::DrawCrosshair()
{
	static let Settings = UCloud9DeveloperSettings::Get();

	let Controller = GetCloud9PlayerController();
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

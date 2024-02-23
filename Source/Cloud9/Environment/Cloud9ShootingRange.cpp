// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9ShootingRange.h"

#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

FName ACloud9ShootingRange::ZoneComponentName = "ZoneComponentName";
FName ACloud9ShootingRange::WidgetComponentName = "WidgetComponentName";

ACloud9ShootingRange::ACloud9ShootingRange()
{
	// Disable collision to remove firearms hit screen
	SetActorEnableCollision(false);
	PrimaryActorTick.bCanEverTick = true;
	ZoneComponent = CreateDefaultSubobject<UBoxComponent>(ZoneComponentName);
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(WidgetComponentName);
	WidgetComponent->SetupAttachment(ZoneComponent);
	WidgetClass = nullptr;
	Template = nullptr;
	RootComponent = ZoneComponent;
	ZoneSize = {100.0f, 100.0f, 100.0f};
	Count = 1;
	GridSize = FVector::OneVector;
	bIsStarted = false;
	TimeElapsed = 0.0f;
	Killed = 0;
}

float ACloud9ShootingRange::GetKillPerMinute() const
{
	if (FMath::IsNearlyZero(TimeElapsed))
	{
		return Killed;
	}

	return Killed / TimeElapsed * 60.0f;
}

void ACloud9ShootingRange::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ZoneComponent->SetBoxExtent(ZoneSize);
	WidgetComponent->SetWidgetClass(WidgetClass);
	WidgetComponent->SetRelativeRotation({0.0f, 90.0f, 0.0f});
	WidgetComponent->SetDrawSize({ZoneSize.X * 2.0f, ZoneSize.Z * 2.0f});
	// Not work's quit well (currently disable all collision in constructor)
	// But required for future to make possible for shooting range configuration
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACloud9ShootingRange::BeginPlay()
{
	Super::BeginPlay();
	SpawnShootingActors();
}

void ACloud9ShootingRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStarted)
	{
		TimeElapsed += DeltaTime;
	}
}

void ACloud9ShootingRange::OnChildActorDestroyed(AActor* DestroyedActor)
{
	bIsStarted = true;
	OnTargetDestroyed.Broadcast(++Killed);
	Actors.Remove(DestroyedActor);
	SpawnShootingActors();
}

bool ACloud9ShootingRange::SpawnShootingActors()
{
	if (not IsValid(Template))
	{
		log(Error, "Actor class not specified!");
		return false;
	}

	int Retries = 0;
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	while (Actors.Num() != Count)
	{
		// GridSize items can be zero (GridSnap should handle it)
		let Location = EFVector::Random(Origin - BoxExtent, Origin + BoxExtent, GridSize);
		let Actor = GetWorld()->SpawnActor(Template, &Location, nullptr);

		if (not IsValid(Actor))
		{
			log(Fatal, "Can't spawn actor at location = %s", *Location.ToString());
			return false;
		}

		if (Actors | ETContainer::AnyByPredicate{[Actor](let It) { return It->IsOverlappingActor(Actor); }})
		{
			Actor->Destroy();

			if (constexpr int MaxRetries = 10; Retries++ == MaxRetries)
			{
				log(
					Error,
					"[Range=%s] parameters seems to be invalid can't spawn specified count of Actors = %d",
					*GetName(), Count
				);
				return false;
			}

			continue;
		}

		Actors.Add(Actor);
		Actor->OnDestroyed.AddDynamic(this, &ACloud9ShootingRange::OnChildActorDestroyed);
	}

	return true;
}

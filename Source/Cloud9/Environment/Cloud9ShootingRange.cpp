// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9ShootingRange.h"

#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Components/BoxComponent.h"

FName ACloud9ShootingRange::ZoneComponentName = "ZoneComponentName";

ACloud9ShootingRange::ACloud9ShootingRange()
{
	PrimaryActorTick.bCanEverTick = true;
	ZoneComponent = CreateDefaultSubobject<UBoxComponent>(ZoneComponentName);
	RootComponent = ZoneComponent;
	ZoneSize = {100.0f, 100.0f, 100.0f};
	Count = 1;
	GridSize = FVector::OneVector;
}

void ACloud9ShootingRange::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ZoneComponent->SetBoxExtent(ZoneSize);
}

void ACloud9ShootingRange::BeginPlay()
{
	Super::BeginPlay();
}

void ACloud9ShootingRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnShootingActors();
}

bool ACloud9ShootingRange::SpawnShootingActors()
{
	int Retries = 0;
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	while (Actors.Num() != Count)
	{
		// GridSize items can be zero (GridSnap should handle it)
		let Location = EFVector::Random(BoxExtent - Origin, BoxExtent + Origin, GridSize);
		let Actor = GetWorld()->SpawnActor(Class.Get(), &Location);
		if (Actors | ETContainer::AnyByPredicate{[Actor](let It) { return It->IsOverlappingActor(Actor); }})
		{
			if (constexpr int MaxRetries = 10; Retries++ == MaxRetries)
			{
				log(
					Error,
					"[Range=%s] parameters seems to be invalid can't spawn specified count of Actors = %d",
					*GetName(), Count
				);
				SetActorTickEnabled(false);
				return false;
			}

			continue;
		}

		Actors.Add(Actor);
	}

	return true;
}

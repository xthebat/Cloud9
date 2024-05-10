// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9PracticeSpawner.h"

#include "Cloud9/Tools/Extensions/FVector.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Components/BoxComponent.h"

FName ACloud9PracticeSpawner::ZoneComponentName = "ZoneComponentName";

ACloud9PracticeSpawner::ACloud9PracticeSpawner()
{
	// Disable collision to remove firearms hit screen
	// SetActorEnableCollision(false);
	PrimaryActorTick.bCanEverTick = true;
	ZoneComponent = CreateDefaultSubobject<UBoxComponent>(ZoneComponentName);
	RootComponent = ZoneComponent;
	ZoneSize = {100.0f, 100.0f, 100.0f};
	ZoneComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MaxTargetsCount = 10;
	GridSize = FVector::OneVector;
	bIsEnabled = true;
	bIsStarted = false;
	TimeElapsed = 0.0f;
	Killed = 0;
	MaxTime = -1.0f;
}

float ACloud9PracticeSpawner::GetKillPerMinute() const
{
	if (FMath::IsNearlyZero(TimeElapsed))
	{
		return Killed;
	}

	return Killed / TimeElapsed * 60.0f;
}

void ACloud9PracticeSpawner::RestartShooting()
{
	for (let Actor : Actors)
	{
		Actor->OnDestroyed.RemoveAll(this);
		Actor->Destroy();
	}
	Actors.Reset();
	AddPracticeTargets();
	bIsStarted = false;
	Killed = 0;
	TimeElapsed = 0.0f;
	OnRestartShooting.Broadcast();
}

void ACloud9PracticeSpawner::SetMaxTargetsCount(int Value)
{
	MaxTargetsCount += Value;
}

void ACloud9PracticeSpawner::SetupTimer(float Time)
{
	MaxTime = Time;
}

void ACloud9PracticeSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ZoneComponent->SetBoxExtent(ZoneSize);
}

void ACloud9PracticeSpawner::BeginPlay()
{
	Super::BeginPlay();
	RestartShooting();
}

void ACloud9PracticeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStarted)
	{
		TimeElapsed += DeltaTime;
		if (MaxTime > 0.0f and TimeElapsed > MaxTime)
		{
			// TODO: Add sound
			bIsStarted = false;
		}
	}
}

void ACloud9PracticeSpawner::OnChildActorDestroyed(AActor* DestroyedActor)
{
	if (bIsEnabled)
	{
		bIsStarted = true;
		OnTargetDestroyed.Broadcast(++Killed);
		Actors.Remove(DestroyedActor);
		AddPracticeTargets();
	}
}

AActor* ACloud9PracticeSpawner::SpawnTarget_Implementation(FVector Location)
{
	if (not IsValid(Template))
	{
		log(Error, "[%s]: Actor class not specified", *GetName());
		bIsEnabled = false;
		SetActorTickEnabled(false);
		return nullptr;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;

	return GetWorld()->SpawnActor(Template, &Location, nullptr, ActorSpawnParameters);
}

bool ACloud9PracticeSpawner::AddPracticeTargets()
{
	if (bIsEnabled)
	{
		int Retries = 0;
		let Origin = GetActorLocation();
		let BoxExtent = ZoneComponent->Bounds.BoxExtent;
		while (Actors.Num() != MaxTargetsCount)
		{
			// GridSize items can be zero (GridSnap should handle it)
			let Location = EFVector::Random(Origin - BoxExtent, Origin + BoxExtent, GridSize);

			let Actor = SpawnTarget(Location);
			AssertOrReturn(IsValid(Actor), false, Error, "Can't spawn actor at location = %s", *Location.ToString());

			if (Actors | ETContainer::AnyByPredicate{[Actor](let It) { return It->IsOverlappingActor(Actor); }})
			{
				Actor->Destroy();

				constexpr int MaxRetries = 10;
				AssertOrReturn(
					Retries++ != MaxRetries, false,
					Error, "Parameters seems to be invalid can't spawn specified count of Actors = %d", MaxRetries);

				continue;
			}

			Actors.Add(Actor);
			Actor->OnDestroyed.AddDynamic(this, &ACloud9PracticeSpawner::OnChildActorDestroyed);
		}

		return true;
	}

	return false;
}

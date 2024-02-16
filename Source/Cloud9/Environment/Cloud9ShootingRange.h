// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cloud9ShootingRange.generated.h"

class UBoxComponent;

UCLASS()
class CLOUD9_API ACloud9ShootingRange : public AActor
{
	GENERATED_BODY()

public:
	static FName ZoneComponentName;

	ACloud9ShootingRange();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UBoxComponent* ZoneComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	TArray<AActor*> Actors;

	UPROPERTY(EditAnywhere, Category=Config)
	int Count;

	UPROPERTY(EditAnywhere, Category=Config)
	TSoftClassPtr<AActor> Class;

	UPROPERTY(EditAnywhere, Category=Config)
	FVector ZoneSize;

	UPROPERTY(EditAnywhere, Category=Config)
	FVector GridSize;

private:
	bool SpawnShootingActors();
};

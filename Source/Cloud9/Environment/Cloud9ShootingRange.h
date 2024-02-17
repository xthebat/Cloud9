// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cloud9ShootingRange.generated.h"

class UWidgetComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDestroyed, int, Count);

UCLASS()
class CLOUD9_API ACloud9ShootingRange : public AActor
{
	GENERATED_BODY()

public:
	static FName ZoneComponentName;
	static FName WidgetComponentName;

	ACloud9ShootingRange();

	UFUNCTION(BlueprintCallable)
	float GetKillPerMinute() const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnChildActorDestroyed(AActor* DestroyedActor);

	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnTargetDestroyed OnTargetDestroyed;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UBoxComponent* ZoneComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	TArray<AActor*> Actors;

	UPROPERTY(EditAnywhere, Category=Config)
	int Count;

	UPROPERTY(EditAnywhere, Category=Config)
	UClass* WidgetClass;

	UPROPERTY(EditAnywhere, Category=Config)
	UClass* Template;

	UPROPERTY(EditAnywhere, Category=Config)
	FVector ZoneSize;

	UPROPERTY(EditAnywhere, Category=Config)
	FVector GridSize;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	bool bIsStarted;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	int Killed;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	float TimeElapsed;

private:
	bool SpawnShootingActors();
};

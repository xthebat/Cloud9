// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cloud9PracticeSpawner.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDestroyed, int, Count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartShooting);

UCLASS()
class CLOUD9_API ACloud9PracticeSpawner : public AActor
{
	GENERATED_BODY()

public:
	static inline FName ZoneComponentName = TEXT("ZoneComponentName");

	ACloud9PracticeSpawner();

	UFUNCTION(BlueprintCallable)
	float GetKillPerMinute() const;

	UFUNCTION(BlueprintCallable)
	void RestartShooting();

	UFUNCTION(BlueprintCallable)
	void SetMaxTargetsCount(int Value);

	UFUNCTION(BlueprintCallable)
	void SetupTimer(float Time);

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnTargetDestroyed OnTargetDestroyed;

	UPROPERTY(BlueprintAssignable, Category=Events)
	FOnRestartShooting OnRestartShooting;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnChildActorDestroyed(AActor* DestroyedActor);

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UBoxComponent* ZoneComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	TArray<AActor*> Actors;

	UPROPERTY(EditAnywhere, Category=Zone)
	UClass* Template;

	UPROPERTY(EditAnywhere, Category=Zone)
	FVector ZoneSize;

	UPROPERTY(EditAnywhere, Category=Zone)
	FVector GridSize;

	UPROPERTY(EditAnywhere, Category=Config)
	int MaxTargetsCount;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	bool bIsStarted;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	float MaxTime;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	int Killed;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	float TimeElapsed;

	/**
	 * If spawner configuration is invalid then set to false
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Info)
	bool bIsEnabled;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Zone)
	AActor* SpawnTarget(FVector Location);

private:
	bool AddPracticeTargets();
};

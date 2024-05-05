// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Cloud9AIController.generated.h"

class ACloud9Character;

UCLASS()
class CLOUD9_API ACloud9AIController : public AAIController
{
	GENERATED_BODY()

public:
	ACloud9AIController();

	UFUNCTION(BlueprintCallable)
	void SetMovingOffset(FVector Value);

	UFUNCTION(BlueprintCallable)
	void SetChangeDirectionDelay(float Value);

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void SetMovingOffsetEx(FVector Value);

	void Move(FVector Destination);

	UPROPERTY()
	FVector BaseLocation;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Testing)
	FVector MovingOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Testing)
	float ChangeDirectionDelay;
};

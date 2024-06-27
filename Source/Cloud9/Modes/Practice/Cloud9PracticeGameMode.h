// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Modes/Base/Cloud9GameMode.h"
#include "Cloud9PracticeGameMode.generated.h"

class ACloud9PracticePlayerStart;

UENUM(BlueprintType)
enum class EPracticeTargetType : uint8
{
	Bots = 0,
	Dots = 1
};

USTRUCT(BlueprintType)
struct FPracticeRangeConfig
{
	GENERATED_BODY()

	// TODO: Add player equipment

	/** Target type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	EPracticeTargetType TargetType = EPracticeTargetType::Bots;

	/** Practice range target's count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	int32 TargetsCount = 5;

	/** Practice range round time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float PracticeTime = 60.0f;

	/** Target respawn time in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float TargetRespawnTime = 1.0f;

	/** Target minimum elevation from the floor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config,
		meta=(EditCondition="TargetType == EPracticeTargetType::Dots"))
	float MinTargetElevation = 0.0f;

	/** Target maximum elevation from the floor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config,
		meta=(EditCondition="TargetType == EPracticeTargetType::Dots"))
	float MaxTargetElevation = 200.0f;

	/** Target maximum offset from base point toward player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float MinTargetDistance = 0.0f;

	/** Target maximum offset from base point outward player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float MaxTargetDistance = 0.0f;

	/** Target movement speed in units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	bool MovementSpeed = 0.0f;

	/** Bots will have armor or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config,
		meta=(EditCondition="TargetType == EPracticeTargetType::Bots"))
	bool IsArmorEnabled = true;

	/** Bots will have a helmet or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config,
		meta=(EditCondition="TargetType == EPracticeTargetType::Bots"))
	bool IsHelmetEnabled = true;

	/** Dots color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config,
		meta=(EditCondition="TargetType == EPracticeTargetType::Dots"))
	FLinearColor Color = {1.0f, 1.0f, 1.0f};
};

USTRUCT(BlueprintType)
struct FPracticeRangeInfo
{
	GENERATED_BODY()

	/** Practice range name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FName RangeName;

	/** Maximum targets on practice range at the same time (min. always = 1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	int32 MaxTargetsCount = 20;

	/** Minimum round practice time in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float MinPracticeTime = 5.0f;

	/** Maximum round practice time in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	float MaxPracticeTime = 300.0f;

	/** Practice range preview image for a configuration menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	UTexture2D* PreviewImage = nullptr;

	/** Practice range place description for a configuration menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FText Description;

	/** Practice range default config. parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	FPracticeRangeConfig Defaults;
};

UCLASS(Blueprintable)
class CLOUD9_API ACloud9PracticeGameMode : public ACloud9GameMode
{
	GENERATED_BODY()

public:
	ACloud9PracticeGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:
	ACloud9PracticePlayerStart* FindPlayerStartEx(const FString& IncomingName = TEXT("")) const;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ConfigWidgetClass;
};

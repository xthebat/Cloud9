#pragma once

#include "CoreMinimal.h"
#include "Cloud9DeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FUnUsedStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IntField0 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatField1 = 0.0f;
};

UENUM()
enum class EUnUsedEnum : int32
{
	Special = 0,
	Anything = 1,
	Everything = 2,
	Whatever = 3,
};

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Save Game Settings"))
class CLOUD9_API UCloud9DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Settings)
	static const UCloud9DeveloperSettings* GetCloud9DeveloperSettings();

	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION(BlueprintCallable)
	void Log() const;

	// Debug

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawHitCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawDeprojectedCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsShowMouseCursor;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int NetGraph;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	EUnUsedEnum UnUsedEnum;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FUnUsedStruct UnUsedStruct;
};

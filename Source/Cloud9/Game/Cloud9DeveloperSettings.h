#pragma once

#include "CoreMinimal.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"
#include "Cloud9DeveloperSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Save Game Settings"))
class CLOUD9_API UCloud9DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Settings)
	static const UCloud9DeveloperSettings* GetCloud9DeveloperSettings()
	{
		static auto bIsLoaded = false;
		static const auto Settings = GetDefault<UCloud9DeveloperSettings>();

		// if (!bIsLoaded)
		// {
			UE_LOG(LogCloud9, Display, TEXT("%s"), *UCloud9ToolsLibrary::UObjectToString(Settings));
			
			// UE_LOG(LogCloud9, Display, TEXT("IsDrawHitCursorLine: %d"), Settings->bIsShowMouseCursor);
			// UE_LOG(LogCloud9, Display, TEXT("IsDrawDeprojectedCursorLine: %d"), Settings->bIsDrawDeprojectedCursorLine);
			// UE_LOG(LogCloud9, Display, TEXT("IsShowMouseCursor: %d"), Settings->bIsShowMouseCursor);
			// UE_LOG(LogCloud9, Display, TEXT("NetGraph: %d"), Settings->NetGraph);
			// bIsLoaded = true;
		// }

		return Settings;
	}

	UFUNCTION(BlueprintCallable)
	void Save() { UpdateDefaultConfigFile(); }

	// Debug

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawHitCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsDrawDeprojectedCursorLine;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsShowMouseCursor;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int NetGraph;
};

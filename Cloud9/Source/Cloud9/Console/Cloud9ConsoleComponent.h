#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9ConsoleComponent.generated.h"

class UCloud9Console;
class ACloud9GameMode;

/**
 * 
 */
UCLASS()
class CLOUD9_API UCloud9ConsoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9ConsoleComponent();

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCloud9Console> ConsoleClass;

protected:
	virtual void BeginPlay() override;

private: // variables
	UPROPERTY()
	UCloud9Console* Console;
};

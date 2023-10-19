#include "Cloud9ConsoleComponent.h"
#include "Cloud9Console.h"
#include "Cloud9/Cloud9.h"

UCloud9ConsoleComponent::UCloud9ConsoleComponent()
{
	// SetIsReplicatedByDefault(true);

	ConsoleClass = UCloud9Console::StaticClass();
	Console = nullptr;
}

void UCloud9ConsoleComponent::BeginPlay()
{
	Super::BeginPlay();

	// We only want to spawn the objects on the server, so they replicate down to the client.
	if (!GetOwner()->HasAuthority())
		return;

	if (ConsoleClass == nullptr)
		return;

	const auto Owner = GetOwner();

	Console = NewObject<UCloud9Console>(Owner, ConsoleClass);
}

bool UCloud9ConsoleComponent::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	if (Console->ProcessConsoleExec(Cmd, Ar, Executor))
		return true;

	return Super::ProcessConsoleExec(Cmd, Ar, Executor);
}

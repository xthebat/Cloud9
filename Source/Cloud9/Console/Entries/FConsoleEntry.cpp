#include "FConsoleEntry.h"

void FConsoleEntry::Initialize(IConsoleCommand* CCom, UWorld* World)
{
	Command = CCom;
}

bool FConsoleEntry::ExecuteConsoleCommand(const FString& Args, UWorld* World) const
{
	TArray<FString> Strings;
	Args.ParseIntoArrayWS(Strings);

	if (IsArgsValid(Strings))
	{
		return Command->Execute(Strings, World, *GLog);
	}

	return false;
}

#include "Cloud9Console.h"

#include "Cloud9/Cloud9.h"

UCloud9Console::UCloud9Console()
{
	// SetIsReplicatedByDefault(true);

	for (TFieldIterator<UFunction> It(GetClass()); It; ++It)
	{
		// Make sure the function belongs to us instead of UActorComponent.
		// Its functions will show up here since we extended it.
		if (GetClass()->IsChildOf(Cast<UClass>(It.GetStruct())))
		{
			It->FunctionFlags |= FUNC_Exec;
		}
	}
}

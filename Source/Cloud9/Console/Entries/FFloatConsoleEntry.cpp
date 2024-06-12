// Copyright (c) 2024 Alexei Gladkikh

#include "FFloatConsoleEntry.h"

#include "Cloud9/Tools/Cloud9StringLibrary.h"
#include "Cloud9/Tools/Macro/Common.h"

bool FFloatConsoleEntry::FromConsoleArgs(const TArray<FString>& Args)
{
	if (let Value = ConvertFromArgs(Args))
	{
		ValueRef = *Value;
		return true;
	}

	return false;
}

bool FFloatConsoleEntry::IsArgsValid(const TArray<FString>& Args) const { return ConvertFromArgs(Args).IsSet(); }

FString FFloatConsoleEntry::GetValueAsString() { return UCloud9StringLibrary::FloatToString(ValueRef); }

TOptional<float> FFloatConsoleEntry::ConvertFromArgs(const TArray<FString>& Args)
{
	if (Args.Num() == 1 and UCloud9StringLibrary::IsStringContainsFloat(Args[0]))
	{
		return UCloud9StringLibrary::StringToFloat(Args[0]);
	}

	return {};
}

// Copyright (c) 2024 Alexei Gladkikh

#include "FIntConsoleEntry.h"

#include "Cloud9/Tools/Cloud9StringLibrary.h"
#include "Cloud9/Tools/Macro/Common.h"

bool FIntConsoleEntry::FromConsoleArgs(const TArray<FString>& Args)
{
	if (let Value = ConvertFromArgs(Args))
	{
		ValueRef = *Value;
		return true;
	}

	return false;
}

bool FIntConsoleEntry::IsArgsValid(const TArray<FString>& Args) const { return ConvertFromArgs(Args).IsSet(); }

FString FIntConsoleEntry::GetValueAsString() { return UCloud9StringLibrary::IntToString(ValueRef); }

TOptional<int32> FIntConsoleEntry::ConvertFromArgs(const TArray<FString>& Args)
{
	if (Args.Num() == 1 and UCloud9StringLibrary::IsStringContainsInt(Args[0]))
	{
		return UCloud9StringLibrary::StringToInt(Args[0]);
	}

	return {};
}

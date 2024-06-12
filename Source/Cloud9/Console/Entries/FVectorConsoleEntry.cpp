// Copyright (c) 2024 Alexei Gladkikh

#include "FVectorConsoleEntry.h"
#include "Cloud9/Tools/Macro/Common.h"

bool FVectorConsoleEntry::FromConsoleArgs(const TArray<FString>& Args)
{
	if (let Vector = ConvertFromArgs(Args))
	{
		ValueRef = *Vector;
		return true;
	}

	return false;
}

bool FVectorConsoleEntry::IsArgsValid(const TArray<FString>& Args) const { return ConvertFromArgs(Args).IsSet(); }

FString FVectorConsoleEntry::GetValueAsString() { return ValueRef.ToString(); }

TOptional<FVector> FVectorConsoleEntry::ConvertFromArgs(const TArray<FString>& Args)
{
	if (Args.Num() == 3)
	{
		var Vector = FVector::ZeroVector;
		let String = FString::Printf(TEXT("X=%s Y=%s Z=%s"), *Args[0], *Args[1], *Args[2]);
		Vector.InitFromString(String);
		return Vector;
	}

	return {};
}

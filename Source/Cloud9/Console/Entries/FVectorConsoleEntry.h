// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"
#include "Cloud9/Tools/Macro/Common.h"

class FVectorConsoleEntry final : public FConsoleEntry
{
public:
	FVectorConsoleEntry(FVector& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override
	{
		if (Args.Num() == 3)
		{
			FVector Vector = FVector::ZeroVector;
			let String = FString::Printf(TEXT("X=%s Y=%s Z=%s"), *Args[0], *Args[1], *Args[2]);
			if (Vector.InitFromString(String))
			{
				ValueRef = Vector;
				return true;
			}
		}

		return false;
	}

	virtual FString GetValueAsString() override
	{
		return ValueRef.ToString();
	}

	// virtual bool SetFromString(const FString& String) override
	// {
	// 	if (FVector Vector; Vector.InitFromString(String))
	// 	{
	// 		return SetFromArgs(
	// 			{
	// 				UCloud9StringLibrary::FloatToString(Vector.X),
	// 				UCloud9StringLibrary::FloatToString(Vector.Y),
	// 				UCloud9StringLibrary::FloatToString(Vector.Z),
	// 			}
	// 		);
	// 	}
	//
	// 	return false;
	// }

private:
	FVector& ValueRef;
};

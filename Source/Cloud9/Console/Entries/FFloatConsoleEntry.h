// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"
#include "Cloud9/Tools/Cloud9StringLibrary.h"

class FFloatConsoleEntry final : public FConsoleEntry
{
public:
	FFloatConsoleEntry(float& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override
	{
		if (Args.Num() == 1 and UCloud9StringLibrary::IsStringContainsFloat(Args[0]))
		{
			ValueRef = UCloud9StringLibrary::StringToFloat(Args[0]);
			return true;
		}

		return false;
	}

	virtual FString GetValueAsString() override
	{
		return UCloud9StringLibrary::FloatToString(ValueRef);
	}

private:
	float& ValueRef;
};

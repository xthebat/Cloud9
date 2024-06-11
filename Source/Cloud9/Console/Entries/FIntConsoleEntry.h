// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"
#include "Cloud9/Tools/Cloud9StringLibrary.h"

class FIntConsoleEntry final : public FConsoleEntry
{
public:
	FIntConsoleEntry(int32& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override
	{
		if (Args.Num() == 1 and UCloud9StringLibrary::IsStringContainsInt(Args[0]))
		{
			ValueRef = UCloud9StringLibrary::StringToInt(Args[0]);
			return true;
		}

		return false;
	}

	virtual FString GetValueAsString() override
	{
		return UCloud9StringLibrary::IntToString(ValueRef);
	}

private:
	int32& ValueRef;
};

// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"

class FIntConsoleEntry final : public FConsoleEntry
{
public:
	FIntConsoleEntry(int32& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override;
	virtual bool IsArgsValid(const TArray<FString>& Args) const override;
	virtual FString GetValueAsString() override;

protected:
	static TOptional<int32> ConvertFromArgs(const TArray<FString>& Args);

private:
	int32& ValueRef;
};

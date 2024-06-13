// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"

class FVectorConsoleEntry final : public FConsoleEntry
{
public:
	FVectorConsoleEntry(FVector& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual bool FromConsoleArgs(const TArray<FString>& Args) override;
	virtual bool IsArgsValid(const TArray<FString>& Args) const override;
	virtual FString GetValueAsString() override;

protected:
	static TOptional<FVector> ConvertFromArgs(const TArray<FString>& Args);

private:
	FVector& ValueRef;
};

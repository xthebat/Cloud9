// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "FConsoleEntry.h"

struct FInputAxisKeyMapping;
class UInputSettings;

class FKeyConsoleEntry final : public FConsoleEntry
{
public:
	FKeyConsoleEntry(FKey& ValueRef, const FString& Name, const FString& Help)
		: FConsoleEntry(Name, Help), ValueRef(ValueRef) {}

	virtual void Initialize(IConsoleCommand* CCom, UWorld* World) override;
	virtual bool FromConsoleArgs(const TArray<FString>& Args) override;
	virtual bool IsArgsValid(const TArray<FString>& Args) const override;
	virtual FString GetValueAsString() override;

protected:
	static TOptional<FKey> ConvertFromArgs(const TArray<FString>& Args);

private:
	FKey& ValueRef;
};

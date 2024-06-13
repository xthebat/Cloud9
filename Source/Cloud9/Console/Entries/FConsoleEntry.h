// Copyright (c) 2024 Alexei Gladkikh

#pragma once

class FConsoleEntry
{
public:
	static inline FString UndefinedConsoleValue = "UNDEFINED";

	FConsoleEntry(const FString& Name, const FString& Help)
		: Name(Name)
		, Help(Help) {}

	virtual ~FConsoleEntry() = default;

	bool ExecuteConsoleCommand(const FString& Args, UWorld* World) const;

	virtual void Initialize(IConsoleCommand* CCom, UWorld* World);
	virtual bool IsArgsValid(const TArray<FString>& Args) const = 0;
	virtual bool FromConsoleArgs(const TArray<FString>& Args) = 0;
	virtual FString GetValueAsString() = 0;

	const FString& GetName() const { return Name; }
	const FString& GetHelp() const { return Help; }

protected:
	FString Name;
	FString Help;
	IConsoleCommand* Command = nullptr;
};

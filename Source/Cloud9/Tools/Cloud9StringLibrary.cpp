// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9StringLibrary.h"

#include "Macro/Common.h"
#include "Misc/DefaultValueHelper.h"

float UCloud9StringLibrary::StringToFloat(const FString& String)
{
	var Result = 0.0f;
	let InvariantString = SanitizeString(String);
	FDefaultValueHelper::ParseFloat(InvariantString, Result);
	return Result;
}

float UCloud9StringLibrary::TextToFloat(const FText& Text)
{
	return StringToFloat(Text.ToString());
}

bool UCloud9StringLibrary::IsStringContainsFloat(const FString& String)
{
	var Result = 0.0f;
	let InvariantString = SanitizeString(String);
	return FDefaultValueHelper::ParseFloat(InvariantString, Result);
}

bool UCloud9StringLibrary::IsTextContainsFloat(const FText& Text)
{
	return IsStringContainsFloat(Text.ToString());
}

FString UCloud9StringLibrary::SanitizeString(const FString& String)
{
	return String.Replace(TEXT(","), TEXT("."));
}

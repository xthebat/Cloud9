// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9StringLibrary.h"

#include "Macro/Common.h"
#include "Misc/DefaultValueHelper.h"

FString UCloud9StringLibrary::FloatToString(float Value, int32 Precision)
{
	// Base on https://nerivec.github.io/old-ue4-wiki/pages/float-as-string-with-precision.html

	let Rounded = roundf(Value);

	if (FMath::Abs(Value - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		Value = Rounded;
	}

	var Format = FNumberFormattingOptions();
	Format.MinimumIntegralDigits = 1;
	Format.MaximumIntegralDigits = 10000;
	Format.MinimumFractionalDigits = Precision;
	Format.MaximumFractionalDigits = Precision;

	return FText::AsNumber(Value, &Format).ToString();
}

FString UCloud9StringLibrary::IntToString(int32 Value)
{
	return FString::Printf(TEXT("%d"), Value);
}

FString UCloud9StringLibrary::VectorToString(const FVector& Vector, int32 Precision)
{
	return FString::Printf(
		TEXT("X=%s Y=%s Z=%s"),
		*FloatToString(Vector.X, Precision),
		*FloatToString(Vector.Y, Precision),
		*FloatToString(Vector.Z, Precision)
	);
}

float UCloud9StringLibrary::StringToFloat(const FString& String)
{
	var Result = 0.0f;
	let InvariantString = SanitizeFloatString(String);
	FDefaultValueHelper::ParseFloat(InvariantString, Result);
	return Result;
}

float UCloud9StringLibrary::StringToInt(const FString& String)
{
	int32 Result = 0;
	FDefaultValueHelper::ParseInt(String, Result);
	return Result;
}

FVector UCloud9StringLibrary::StringToVector(const FString& String)
{
	FVector Vector;
	let InvariantString = SanitizeFloatString(String);
	Vector.InitFromString(InvariantString);
	return Vector;
}

bool UCloud9StringLibrary::IsStringContainsFloat(const FString& String)
{
	let InvariantString = SanitizeFloatString(String);
	return FDefaultValueHelper::IsStringValidFloat(InvariantString);
}

bool UCloud9StringLibrary::IsStringContainsInt(const FString& String)
{
	return FDefaultValueHelper::IsStringValidInteger(String);
}

bool UCloud9StringLibrary::IsStringContainsVector(const FString& String)
{
	FVector Vector;
	return Vector.InitFromString(String);
}

bool UCloud9StringLibrary::IsStringContainsKey(const FString& String)
{
	let Key = FKey(*String);
	return Key.IsValid();
}

FString UCloud9StringLibrary::BoolToIntString(bool Value)
{
	return Value ? "1" : "0";
}

bool UCloud9StringLibrary::IntStringToBool(const FString& String)
{
	return String != "0";
}

FString UCloud9StringLibrary::SanitizeFloatString(const FString& String)
{
	return String.Replace(TEXT(","), TEXT("."));
}

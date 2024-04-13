// Copyright (c) 2024 Alexei Gladkikh

#pragma once

namespace Math
{
	template <class T, class U>
	T InverseLerp(const T& A, const T& B, const U& X) { return static_cast<T>((X - A) / (B - A)); }

	// see mathlib.h from cstrike15_src

	template <typename InputType, typename OutputType>
	constexpr InputType Select(InputType Value, OutputType A, OutputType B)
	{
		return Value < 0 ? A : B;
	}

	template <typename ValueType, typename InRangeType, typename OutRangeType>
	constexpr OutRangeType RemapValue(
		ValueType Value,
		InRangeType InRangeMin,
		InRangeType InRangeMax,
		OutRangeType OutRangeMin,
		OutRangeType OutRangeMax)
	{
		if (InRangeMin == InRangeMax)
		{
			return Select(Value - InRangeMax, OutRangeMax, OutRangeMin);
		}

		let InRange = static_cast<OutRangeType>(InRangeMax - InRangeMin);
		let OutRange = OutRangeMax - OutRangeMin;
		return OutRangeMin + OutRange * static_cast<OutRangeType>(Value - InRangeMin) / InRange;
	}

	template <typename ValueType, typename InRangeType, typename OutRangeType>
	constexpr OutRangeType RemapValueClamped(
		ValueType Value,
		InRangeType InRangeMin,
		InRangeType InRangeMax,
		OutRangeType OutRangeMin,
		OutRangeType OutRangeMax)
	{
		if (InRangeMin == InRangeMax)
		{
			return Select(Value - InRangeMax, OutRangeMax, OutRangeMin);
		}

		let InRange = static_cast<OutRangeType>(InRangeMax - InRangeMin);
		let Temp = FMath::Clamp(static_cast<OutRangeType>(Value - InRangeMin) / InRange, 0.0f, 1.0f);
		let OutRange = OutRangeMax - OutRangeMin;
		return OutRangeMin + OutRange * Temp;
	}
}

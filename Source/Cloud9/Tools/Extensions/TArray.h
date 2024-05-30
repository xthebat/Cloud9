// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "TContainer.h"

namespace ETArray
{
	/**
	 * Creates uninitialized vector with specified size.
	 *
	 * @tparam ValueType Element type
	 * @param Size Size of Array
	 */
	template <typename ValueType>
	constexpr auto ArrayOf(int Size)
	{
		var Result = TArray<ValueType>();
		Result.SetNumUninitialized(Size);
		return Result;
	}

	/**
	 * Creates vector with specified size and initialize each element using specified block.
	 *
	 * @param Size Size of Array
	 * @param Block Array item initializer by it index
	 */
	template <typename InitializerType>
	constexpr auto ArrayOf(int Size, InitializerType Block)
	{
		using ReturnType = typename TInvokeResult<InitializerType, int>::Type;
		var Result = ArrayOf<ReturnType>(Size);
		for (var k : Result | ETContainer::Indexes{})
		{
			Result[k] = Block(k);
		}
		return Result;
	}

	struct Sorted
	{
		template <typename ElementType>
		FORCEINLINE TArray<ElementType> operator()(const TArray<ElementType>& Self) const
		{
			var Result = Self;
			Result.StableSort();
			return Result;
		}

		OPERATOR_BODY(Sorted)
	};
}

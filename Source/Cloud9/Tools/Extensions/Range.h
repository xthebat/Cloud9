// Copyright 2023 (c) 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "Cloud9/Cloud9.h"

namespace ERange
{
	// Iterators extensions

	template <typename IteratorType>
	concept incrementable = requires(IteratorType Iterator)
	{
		++Iterator;
	};

	template <typename IteratorType, typename SizeType>
	concept moveable =
		incrementable<IteratorType>
		&& requires(IteratorType Iterator, SizeType Distance)
		{
			Iterator += Distance;
		};

	template <typename IteratorType, typename ValueType>
	concept dereferencable = requires(IteratorType Iterator)
	{
		{ *Iterator } -> std::convertible_to<ValueType>;
	};

	template <typename SizeType>
	struct Advance : TOperator<Advance<SizeType>>
	{
		explicit Advance(SizeType Distance) : Distance(Distance) { }

		constexpr auto operator()(incrementable auto Self) const
		{
			for (SizeType i = 0; i < Distance; ++i) { ++Self; }
			return Self;
		}

		constexpr auto operator()(moveable<SizeType> auto Self) const
		{
			Self += Distance;
			return Self;
		}

	private:
		SizeType Distance;
	};

	struct GetValue : TOperator<GetValue>
	{
		constexpr auto operator()(auto Self) const { return *Self; }
	};

	// TArray extensions
	struct Random : TOperator<Random>
	{
		template <typename CollectionType, typename TResult = typename CollectionType::ElementType>
		constexpr TOptional<TResult> operator()(CollectionType& Self) const
		{
			if (Self.Num() != 0)
			{
				return Self.CreateConstIterator()
					| Advance(FMath::RandRange(0, Self.Num() - 1))
					| GetValue();
			}

			return {};
		}
	};
}

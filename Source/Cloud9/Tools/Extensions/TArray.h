// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "Sequence.h"
#include "Cloud9/Cloud9.h"

namespace Private_ETArray {}

namespace ETArray
{
	template <class BlockType>
	struct ForEach
	{
		const BlockType& Block;

		template <typename RangeType>
		constexpr void operator()(RangeType&& Range) const
		{
			for (let& It : Range)
			{
				Block(It);
			}
		}

		OPERATOR_BODY(ForEach)
	};

	template <typename PredicateType>
	struct Filter
	{
		const PredicateType& Predicate;

		template <typename ContainerType>
		struct TFilterIterator
		{
			using ElementType = typename TDecay<ContainerType>::Type::ElementType;

			constexpr TFilterIterator(ContainerType&& Container, Filter&& Operator)
				: Container(Container)
				, Iterator(Container.CreateConstIterator())
				, Operator(Operator)
				, bIsFound(false) {}

			constexpr void Initialize() { FindNext(); }

			constexpr TFilterIterator& operator++()
			{
				FindNext(true);
				return *this;
			}

			constexpr const ElementType& operator->() const { return Iterator; }

			constexpr ElementType operator*() const { return *Iterator; }

			constexpr explicit operator bool() const { return not Iterator; }

		private:
			void FindNext(bool Force = false)
			{
				if (Force)
				{
					bIsFound = false;
					++Iterator;
				}

				if (not bIsFound)
				{
					while (Iterator and not Operator.Predicate(*Iterator))
					{
						++Iterator;
					}
					bIsFound = true;
				}
			}

			using IteratorType = typename TDecay<ContainerType>::Type::TConstIterator;

			ContainerType Container;
			IteratorType Iterator;

			Filter Operator;
			bool bIsFound;
		};

		SEQUENCE_OPERATOR_BODY(Filter, TFilterIterator)
	};
}

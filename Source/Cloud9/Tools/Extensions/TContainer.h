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

#include "Cloud9/Tools/Concepts.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"
#include "Cloud9/Tools/Containers/Sequence.h"
#include "Cloud9/Tools/Extensions/TOptional.h"

namespace ETIterator
{
	template <typename SizeType>
	struct Advance
	{
		explicit Advance(SizeType Distance) : Distance(Distance) {}

		constexpr auto operator()(Concepts::incrementable auto Self) const
		{
			for (SizeType i = 0; i < Distance; ++i) { ++Self; }
			return Self;
		}

		constexpr auto operator()(Concepts::plusassingable<SizeType> auto Self) const
		{
			Self += Distance;
			return Self;
		}

		OPERATOR_BODY(Advance)

	private:
		SizeType Distance;
	};

	struct GetValue
	{
		constexpr auto operator()(auto Self) const { return *Self; }

		OPERATOR_BODY(GetValue)
	};
}

namespace Private_ETContainer
{
	template <typename OperatorType, typename ContainerType>
	struct TFromIteratorIterator;

	template <typename OperatorType, typename ContainerType>
	struct TIndexesIterator;

	template <typename OperatorType, typename ContainerType>
	struct TTransformIterator;

	template <typename OperatorType, typename ContainerType>
	struct TFilterIterator;

	template <typename OperatorType, typename ContainerType>
	struct TDropIterator;

	template <typename OperatorType, typename ContainerType>
	struct TTakeIterator;
}

namespace ETContainer
{
	struct FromIterator
	{
		template <typename ElementType>
		constexpr auto operator()(TFieldIterator<ElementType>&& Self) const
		{
			using FIteratorType = TFieldIterator<ElementType>;
			var Iterator = Private_ETContainer::TFromIteratorIterator<ElementType, FIteratorType>(Self);
			return TSequence<ElementType, decltype(Iterator)>(MoveTemp(Iterator));
		}

		OPERATOR_BODY(FromIterator)
	};

	struct Random
	{
		template <typename ContainerType, typename ElementType = typename ContainerType::ElementType>
		constexpr TOptional<ElementType> operator()(ContainerType&& Self) const
		{
			if (Self.Num() != 0)
			{
				return Self.CreateConstIterator()
					| ETIterator::Advance(FMath::RandRange(0, Self.Num() - 1))
					| ETIterator::GetValue();
			}

			return {};
		}

		OPERATOR_BODY(Random)
	};

	template <typename BlockType>
	struct ForEach
	{
		const BlockType& Block;

		template <typename ContainerType>
		constexpr void operator()(ContainerType&& Self) const
		{
			for (let& It : Self)
			{
				Block(It);
			}
		}

		OPERATOR_BODY(ForEach)
	};

	template <typename BlockType>
	struct ForEachIndexed
	{
		const BlockType& Block;

		template <typename ContainerType>
		constexpr void operator()(ContainerType&& Self) const
		{
			int Index = 0;
			for (let& It : Self)
			{
				Block(Index++, It);
			}
		}

		OPERATOR_BODY(ForEachIndexed)
	};

	template <typename KeyBlockType, typename ValueBlockType>
	struct Associate
	{
		const KeyBlockType& KeyBlock;
		const ValueBlockType& ValueBlock;

		template <typename ContainerType>
		constexpr auto operator()(ContainerType&& Self) const
		{
			using FKeyType = typename TInvokeResult<KeyBlockType, typename ContainerType::ElementType>::Type;
			using FValueType = typename TInvokeResult<ValueBlockType, typename ContainerType::ElementType>::Type;

			TMap<FKeyType, FValueType> Output;
			for (let& It : Self)
			{
				Output.Emplace(KeyBlock(It), ValueBlock(It));
			}
			return Output;
		}

		OPERATOR_BODY(Associate)
	};

	struct Indexes
	{
		SEQUENCE_OPERATOR_BODY(Indexes, Private_ETContainer::TIndexesIterator)
	};

	template <typename InOperationType>
	struct Transform
	{
		using OperationType = InOperationType;

		const OperationType& Operation;

		SEQUENCE_OPERATOR_BODY(Transform, Private_ETContainer::TTransformIterator)
	};

	template <typename PredicateType>
	struct Filter
	{
		const PredicateType& Predicate;

		SEQUENCE_OPERATOR_BODY(Filter, Private_ETContainer::TFilterIterator)
	};

	template <typename PredicateType>
	struct Find
	{
		const PredicateType& Predicate;

		template <typename ContainerType>
		constexpr auto operator()(ContainerType&& Self) const
		{
			using ElementType = typename TDecay<ContainerType>::Type::ElementType;

			for (let& It : Self)
			{
				if (Predicate(It))
				{
					return TOptional<ElementType>(It);
				}
			}

			return TOptional<ElementType>();
		}

		OPERATOR_BODY(Find)
	};

	template <typename PredicateType>
	struct AnyByPredicate
	{
		const PredicateType& Predicate;

		template <typename ContainerType>
		constexpr bool operator()(ContainerType&& Self) const
		{
			return Self | Find{Predicate} | ETOptional::IsSet();
		}

		OPERATOR_BODY(AnyByPredicate)
	};

	template <typename PredicateType>
	struct AllByPredicate
	{
		const PredicateType& Predicate;

		template <typename ContainerType>
		constexpr bool operator()(ContainerType&& Self) const
		{
			// true if all elements are true (empty collection is correspond this condition)  
			for (let& It : Self)
			{
				if (not Predicate(It))
				{
					return false;
				}
			}

			return true;
		}

		OPERATOR_BODY(AllByPredicate)
	};

	struct All
	{
		template <typename ContainerType>
		constexpr bool operator()(ContainerType&& Self) const
		{
			// true if all elements are true (empty collection is correspond this condition)  
			for (let& It : Self)
			{
				if (not It)
				{
					return false;
				}
			}

			return true;
		}

		OPERATOR_BODY(All)
	};

	template <typename BlockType>
	void Repeat(int Count, const BlockType& Block)
	{
		for (int i = 0; i < Count; ++i)
		{
			Block(i);
		}
	}

	struct Drop
	{
		int Count = 0;

		SEQUENCE_OPERATOR_BODY(Drop, Private_ETContainer::TDropIterator)
	};

	struct Take
	{
		int Count = 0;

		SEQUENCE_OPERATOR_BODY(Take, Private_ETContainer::TTakeIterator)
	};

	struct ToArray
	{
		int ToDrop = 0;
		int ToTake = INT_MAX;

		template <typename ContainerType>
		auto operator()(ContainerType&& Self) const
		{
			TArray<typename TDecay<ContainerType>::Type::ElementType> Result{};
			for (let& It : Self
			     | Drop{ToDrop}
			     | Take{ToTake})
			{
				Result.Add(It);
			}
			return Result;
		}

		OPERATOR_BODY(ToArray)
	};
}

namespace Private_ETContainer
{
	template <typename InElementType, typename InIteratorType>
	struct TFromIteratorIterator
	{
		using IteratorType = InIteratorType;
		using ElementType = InElementType;

		constexpr explicit TFromIteratorIterator(IteratorType Iterator): Iterator(Iterator) {}

		// ReSharper disable once CppMemberFunctionMayBeStatic
		constexpr void Initialize() {}

		constexpr TFromIteratorIterator& operator++()
		{
			++Iterator;
			return *this;
		}

		constexpr const ElementType& operator*() { return **Iterator; }

		// constexpr ElementType operator*() { return **Iterator; }

		constexpr explicit operator bool() const { return not Iterator; }

	private:
		IteratorType Iterator;;
	};

	template <typename OperatorType, typename ContainerType>
	struct TIndexesIterator
	{
		using ElementType = int;

		constexpr TIndexesIterator(ContainerType&& Container, OperatorType&& Operator)
			: Container(Container)
			, Iterator(Container.CreateConstIterator())
			, Operator(Operator) {}

		// ReSharper disable once CppMemberFunctionMayBeStatic
		constexpr void Initialize() {}

		constexpr TIndexesIterator& operator++()
		{
			++Index;
			++Iterator;
			return *this;
		}

		constexpr const ElementType& operator*() const { return Index; }

		constexpr explicit operator bool() const { return not Iterator; }

	private:
		using IteratorType = typename TDecay<ContainerType>::Type::TConstIterator;

		ContainerType Container;
		IteratorType Iterator;
		OperatorType Operator;

		ElementType Index = 0;
	};

	template <typename OperatorType, typename ContainerType>
	struct TTransformIterator
	{
		using ElementType = typename TInvokeResult<
			typename OperatorType::OperationType,
			typename TDecay<ContainerType>::Type::ElementType
		>::Type;

		constexpr TTransformIterator(ContainerType&& Container, OperatorType&& Operator)
			: Container(Container)
			, Iterator(Container.CreateConstIterator())
			, Operator(Operator) {}

		// ReSharper disable once CppMemberFunctionMayBeStatic
		constexpr void Initialize() {}

		constexpr TTransformIterator& operator++()
		{
			++Iterator;
			return *this;
		}

		// constexpr const ElementType& operator->() const { return Iterator; }

		constexpr const ElementType& operator*()
		{
			Cache = Operator.Operation(*Iterator);
			return *Cache;
		}

		constexpr explicit operator bool() const { return not Iterator; }

	private:
		using IteratorType = typename TDecay<ContainerType>::Type::TConstIterator;

		ContainerType Container;
		IteratorType Iterator;
		OperatorType Operator;

		TOptional<ElementType> Cache;
	};

	template <typename OperatorType, typename ContainerType>
	struct TFilterIterator
	{
		using ElementType = typename TDecay<ContainerType>::Type::ElementType;

		constexpr TFilterIterator(ContainerType&& Container, OperatorType&& Operator)
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

		constexpr const ElementType& operator*() { return *Iterator; }

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
		OperatorType Operator;

		bool bIsFound;
	};

	template <typename OperatorType, typename ContainerType>
	struct TDropIterator
	{
		using ElementType = typename TDecay<ContainerType>::Type::ElementType;

		constexpr TDropIterator(ContainerType&& Container, OperatorType&& Operator)
			: Container(Container)
			, Iterator(Container.CreateConstIterator())
			, Operator(Operator) {}

		constexpr void Initialize()
		{
			int Dropped = 0;
			while (Operator.Count != 0 and Dropped < Operator.Count and Iterator)
			{
				++Dropped;
				++Iterator;
			}
		}

		constexpr TDropIterator& operator++()
		{
			++Iterator;
			return *this;
		}

		constexpr const ElementType& operator->() const { return Iterator; }

		constexpr const ElementType& operator*() { return *Iterator; }

		constexpr explicit operator bool() const { return not Iterator; }

	private:
		using IteratorType = typename TDecay<ContainerType>::Type::TConstIterator;

		ContainerType Container;
		IteratorType Iterator;
		OperatorType Operator;
	};

	template <typename OperatorType, typename ContainerType>
	struct TTakeIterator
	{
		using ElementType = typename TDecay<ContainerType>::Type::ElementType;

		constexpr TTakeIterator(ContainerType&& Container, OperatorType&& Operator)
			: Container(Container)
			, Iterator(Container.CreateConstIterator())
			, Operator(Operator) {}

		constexpr void Initialize() {}

		constexpr TTakeIterator& operator++()
		{
			++Iterator;
			++Taken;
			return *this;
		}

		constexpr const ElementType& operator->() const { return Iterator; }

		constexpr const ElementType& operator*() { return *Iterator; }

		constexpr explicit operator bool() const
		{
			return Operator.Count != INT_MAX and Taken == Operator.Count or not Iterator;
		}

	private:
		using IteratorType = typename TDecay<ContainerType>::Type::TConstIterator;

		ContainerType Container;
		IteratorType Iterator;
		OperatorType Operator;

		int Taken = 0;
	};
}

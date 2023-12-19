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

namespace Sequence
{
	template <typename InElementType, typename IteratorType>
	class TSequence
	{
	public:
		constexpr explicit TSequence(TSequence&& Other) noexcept
			: Iterator(MoveTemp(Other.Iterator))
			, bIsInitialized(Other.bIsInitialized) {}

		constexpr explicit TSequence(IteratorType Iterator)
			: Iterator(MoveTemp(Iterator))
			, bIsInitialized(false) {}

		using ElementType = InElementType;

		TSequence& operator++()
		{
			++Iterator;
			return *this;
		}

		ElementType operator*() const { return *Iterator; }

		constexpr explicit operator bool() const { return not Iterator; }

		using TIterator = TSequence;
		using TConstIterator = TSequence<const ElementType, IteratorType>;

		struct FEndTag {};

		constexpr auto begin()
		{
			Initialize();
			return TIterator(MoveTemp(*this));
		}

		constexpr auto end() const
		{
			return TSequence::FEndTag();
		}

		constexpr bool IsEmpty()
		{
			Initialize();
			return Iterator;
		}

		constexpr const ElementType& Front()
		{
			Initialize();
			return *Iterator;
		}

		constexpr bool operator!=([[maybe_unused]] FEndTag Other) const { return not Iterator; }

		constexpr bool operator==([[maybe_unused]] FEndTag Other) const { return not this->operator!=(Other); }

	private:
		void Initialize()
		{
			if (not bIsInitialized)
			{
				Iterator.Initialize();
				bIsInitialized = true;
			}
		}

		IteratorType Iterator;
		bool bIsInitialized;
	};

#define SEQUENCE_OPERATOR_BODY(OperatorType, IteratorType)\
	template <typename RangeType>\
	constexpr auto operator()(RangeType&& Range)\
	{\
		using FIterator = IteratorType<RangeType>;\
		var Iterator = FIterator(Forward<RangeType>(Range), MoveTemp(*this));\
		return Sequence::TSequence<typename FIterator::ElementType, FIterator>(MoveTemp(Iterator));\
	}\
	OPERATOR_BODY(OperatorType)
}

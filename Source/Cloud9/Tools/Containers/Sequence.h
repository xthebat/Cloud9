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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

// No namespace to meet UE4 code-style

template <typename InElementType, typename IteratorType>
class TSequence
{
public:
	constexpr explicit TSequence(IteratorType Iterator)
		: Iterator(MoveTemp(Iterator))
		, bIsInitialized(false) {}

	using ElementType = InElementType;

	/**
	 * Function returns next element of sequence.
	 * 
	 * NOTE: Sequence is itself iterator so function increments
	 * internal iterator and then return next self value.
	 */
	TSequence& operator++()
	{
		++Iterator;
		return *this;
	}

	/**
	 * Function returns current underlying Iterator value.
	 */
	const ElementType& operator*() { return *Iterator; }

	/**
	 * Function returns false if sequence is empty.
	 */
	constexpr explicit operator bool() const { return not Iterator; }

	constexpr const ElementType& Front()
	{
		Initialize();
		return *Iterator;
	}

	// Compatability with UE4 containers (at least TArray)

	using TIterator = TSequence;
	using TConstIterator = TSequence<const ElementType, IteratorType>;

	constexpr TConstIterator& CreateConstIterator()
	{
		Initialize();
		return *reinterpret_cast<TConstIterator*>(this);
	}

	// Compatability with stl-like for-based loop

	/**
	 * Tag class required to check if sequence has been finished.
	 */
	struct FEndTag {};

	/**
	 * Functions returns begin iterator value to support for-based loop.
	 * @see note for `TSequence& operator++()`.
	 */
	constexpr TSequence& begin()
	{
		Initialize();
		return *this;
	}

	/**
	 * Functions return end iterator value to support for-based loop.
	 * NOTE: In fact special tag-object returned only to check if
	 * underlying Iterator finished or not.
	 */
	constexpr FEndTag end() const { return TSequence::FEndTag(); }

	/**
	 * Functions checks if sequence is finished or not by compare with EndTag.
	 * @param Other EndTag value.
	 */
	constexpr bool operator!=([[maybe_unused]] FEndTag Other) const { return not Iterator; }

	/**
	 * Functions checks if sequence is not finished or not by compare with EndTag.
	 * @param Other EndTag value.
	 */
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
		using FIterator = IteratorType<OperatorType, RangeType>;\
		var Iterator = FIterator(Forward<RangeType>(Range), MoveTemp(*this));\
		return TSequence<typename FIterator::ElementType, FIterator>(MoveTemp(Iterator));\
	}\
	OPERATOR_BODY(OperatorType)

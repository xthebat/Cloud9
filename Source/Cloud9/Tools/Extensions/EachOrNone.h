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

#include "Cloud9/Cloud9.h"

namespace Private_EachOrNone
{
	template <typename TOperation, typename... TTypes>
	struct THelper {};

	template <typename TOperation, typename TFirst, typename... TRest>
	struct THelper<TOperation, TFirst, TRest...>
	{
		using ReturnType = typename TInvokeResult<TOperation, TFirst>::Type;

		explicit THelper(TOperation Operation) : Operation(Operation) {}

		template <typename TValue>
		friend constexpr auto operator|(TValue Value, THelper&& Self)
		{
			return THelper::Call(Value, Self);
		}

		template <typename TValue, typename... TTypes>
		static constexpr TOptional<ReturnType> Call(TValue Value, THelper<TTypes...>& Self)
		{
			using ArgType = typename TRemovePointer<TFirst>::Type;
			if (let Casted = CastField<ArgType>(Value))
			{
				return Self.Operation(Casted);
			}

			return THelper<TOperation, TFirst, TTypes...>::Call(Value, Self);
		}

		template <typename TValue, typename TLast>
		static constexpr TOptional<ReturnType> Call(TValue Value, THelper<TLast>& When)
		{
			using ArgType = typename TRemovePointer<TLast>::Type;
			if (let Casted = CastField<ArgType>(Value))
			{
				return When.Operation(Casted);
			}

			return {};
		}

		TOperation Operation;
	};
}

/**
 * TODO: Fix EachOrNone
 */
template <typename... TTypes>
struct EachOrNone
{
	template <typename TOperation>
	static auto Exec(TOperation Operation)
	{
		return Private_EachOrNone::THelper<TOperation, TTypes...>(Operation);
	}
};

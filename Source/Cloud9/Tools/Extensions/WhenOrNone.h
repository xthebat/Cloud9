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

#include "TFirstArgumentOf.h"
#include "TResultOf.h"
#include "Cloud9/Cloud9.h"

template <typename... TFunctions>
struct WhenOrNone {};

template <typename TFirst, typename... TRest>
struct WhenOrNone<TFirst, TRest...>
{
	WhenOrNone(TFirst First, TRest... Rest) : First(First), Rest(Rest...) {}

	template <typename TValue>
	friend constexpr auto operator|(TValue Value, WhenOrNone&& Self)
	{
		using ReturnType = typename TResultOf<decltype(Self.First)>::Type;
		return WhenOrNone::Call<ReturnType>(Value, Self);
	}

	template <typename TResult, typename TValue, typename... TFunctions>
	static constexpr TOptional<TResult> Call(TValue Value, WhenOrNone<TFunctions...>& Self)
	{
		if (let Result = WhenOrNone<TFirst>::template Call<TResult>(Value, Self.First))
		{
			return Result;
		}

		return WhenOrNone<TRest...>::template Call<TResult>(Value, Self.Rest);
	}

	template <typename TResult, typename TValue, typename TLast>
	static constexpr TOptional<TResult> Call(TValue Value, WhenOrNone<TLast>& When)
	{
		return WhenOrNone<TLast>::template Call<TResult>(Value, When.First);
	}

	template <typename TResult, typename TValue, typename TFunction>
	static constexpr TOptional<TResult> Call(TValue Value, TFunction Function)
	{
		using ReturnType = typename TResultOf<TFunction>::Type;
		using ArgType = typename TRemovePointer<typename TFirstArgumentOf<TFunction>::Type>::Type;
		using ValueType = typename TRemovePointer<TValue>::Type;
		static_assert(TIsSame<TResult, ReturnType>::Value, "All WhenOrNone branches must return same type");

		ArgType* Casted;

		if constexpr (TIsSame<ValueType, FProperty>::Value)
		{
			Casted = CastField<ArgType>(Value);
		}
		else if constexpr (TIsSame<ValueType, UObject>::Value)
		{
			Casted = Cast<ArgType>(Value);
		}
		// else
		// {
		// 	// https://forums.unrealengine.com/t/how-do-i-cast-between-polymorphic-classes-that-dont-extend-uobject/368660
		// 	COMPILE_WARNING("bUseRTTI = true; in your build.cs file to enable cast for raw pointers")
		// 	Casted = dynamic_cast<ArgType*>(Value);
		// }

		if (Casted != nullptr)
		{
			return Function(Casted);
		}

		return {};
	}

private:
	TFirst First;

	WhenOrNone<TRest...> Rest;
};

template <typename... Ts>
WhenOrNone(Ts...) -> WhenOrNone<Ts...>;

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
#include "TFirstArgumentOf.h"
#include "TResultOf.h"

template <typename...>
struct WhenOrNone {};

template <typename FirstType, typename... RestType>
struct WhenOrNone<FirstType, RestType...>
{
	WhenOrNone(FirstType First, RestType... Rest) : First(First), Rest(Rest...) {}

	template <typename ValueType>
	friend constexpr auto operator|(ValueType Value, WhenOrNone&& Self)
	{
		using ReturnType = typename TResultOf<decltype(Self.First)>::Type;
		return WhenOrNone::Call<ReturnType>(Value, Self);
	}

	template <typename ResultType, typename ValueType, typename... FunctionTypes>
	static constexpr TOptional<ResultType> Call(ValueType Value, WhenOrNone<FunctionTypes...>& Self)
	{
		if (let Result = WhenOrNone<FirstType>::template Call<ResultType>(Value, Self.First))
		{
			return Result;
		}

		return WhenOrNone<RestType...>::template Call<ResultType>(Value, Self.Rest);
	}

	template <typename ResultType, typename ValueType, typename LastType>
	static constexpr TOptional<ResultType> Call(ValueType Value, WhenOrNone<LastType>& When)
	{
		return WhenOrNone<LastType>::template Call<ResultType>(Value, When.First);
	}

	template <typename ResultType, typename ValueType, typename FunctionType>
	static constexpr TOptional<ResultType> Call(ValueType Value, FunctionType Function)
	{
		using ReturnType = typename TResultOf<FunctionType>::Type;
		using ArgType = typename TRemovePointer<typename TFirstArgumentOf<FunctionType>::Type>::Type;
		using InnerType = typename TRemovePointer<ValueType>::Type;
		static_assert(TIsSame<ResultType, ReturnType>::Value, "All WhenOrNone branches must return same type");

		ArgType* Casted;

		if constexpr (TIsSame<InnerType, FProperty>::Value)
		{
			Casted = CastField<ArgType>(Value);
		}
		else if constexpr (TIsDerivedFrom<InnerType, UObject>::Value)
		{
			log(Error, "Try to cast '%s'", *ArgType::StaticClass()->GetName())
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
	FirstType First;

	WhenOrNone<RestType...> Rest;
};

template <typename... Ts>
WhenOrNone(Ts...) -> WhenOrNone<Ts...>;

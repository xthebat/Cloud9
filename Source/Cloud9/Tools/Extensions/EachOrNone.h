#pragma once

#include "Cloud9/Cloud9.h"

template <typename TOperation, typename... TTypes>
struct Helper {};

template <typename TOperation, typename TFirst, typename... TRest>
struct Helper<TOperation, TFirst, TRest...>
{
	using ReturnType = typename TInvokeResult<TOperation, TFirst>::Type;

	explicit Helper(TOperation Operation) : Operation(Operation) {}

	template <typename TValue>
	friend constexpr auto operator|(TValue Value, Helper&& Self)
	{
		return Helper::Call(Value, Self);
	}

	template <typename TValue, typename... TTypes>
	static constexpr TOptional<ReturnType> Call(TValue Value, Helper<TTypes...>& Self)
	{
		using ArgType = typename TRemovePointer<TFirst>::Type;
		if (let Casted = CastField<ArgType>(Value))
		{
			return Self.Operation(Casted);
		}

		return Helper<TOperation, TFirst, TTypes...>::Call(Value, Self);
	}

	template <typename TValue, typename TLast>
	static constexpr TOptional<ReturnType> Call(TValue Value, Helper<TLast>& When)
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

template <typename... TTypes>
struct EachOrNone
{
	template <typename TOperation>
	static auto Exec(TOperation Operation)
	{
		return Helper<TOperation, TTypes...>(Operation);
	}
};

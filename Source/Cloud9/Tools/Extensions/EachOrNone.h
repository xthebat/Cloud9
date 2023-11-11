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

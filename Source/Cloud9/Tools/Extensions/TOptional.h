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

namespace ETOptional
{
	template <typename DefaultValueType>
	struct Get : TOperator<Get<DefaultValueType>>
	{
	public:
		constexpr explicit Get() : DefaultValueGetter(NullOptionalDereference) {}

		template <typename T> requires std::invocable<T>
		constexpr explicit Get(const T& Default) : DefaultValueGetter(Default) {}

		template <typename T> requires not std::invocable<T>
		constexpr explicit Get(const T& Default) : DefaultValueGetter([&] { return Default; }) {}

		template <typename ValueType>
		constexpr const ValueType& operator()(const TOptional<ValueType>& Self)
		{
			// or TIsSame<DefaultValueType, nullptr_t>::Value

			// nullptr_t is acceptable because we case crash on dereference
			// using log(Fatal) if not default argument was set   
			static_assert(
				TIsSame<DefaultValueType, ValueType>::Value or TIsSame<DefaultValueType, nullptr_t>::Value,
				"Default value type and value type should be the same or default value type should be nullptr_t"
			);

			if (not Self.IsSet())
			{
				DefaultValue = DefaultValueGetter();
				return *reinterpret_cast<const ValueType*>(&DefaultValue);
			}

			return *Self;
		}

	private:
		TFunction<DefaultValueType()> DefaultValueGetter;

		DefaultValueType DefaultValue;

		constexpr static DefaultValueType NullOptionalDereference()
		{
			log(Fatal, "nullopt dereference");
			return {};
		}
	};

	Get() -> Get<nullptr_t>;

	template <typename T> requires std::invocable<T>
	Get(const T&) -> Get<TInvokeResult_T<T>>;

	template <typename T> requires not std::invocable<T>
	Get(const T&) -> Get<T>;

	template <typename BlockType>
	struct OnNull : TOperator<OnNull<BlockType>>
	{
	public:
		constexpr explicit OnNull(BlockType Block) : Block(Block) {}

		template <typename ValueType>
		constexpr const TOptional<ValueType>& operator()(const TOptional<ValueType>& Self)
		{
			if (not Self.IsSet())
			{
				Block();
			}

			return Self;
		}

	private:
		BlockType Block;
	};

	template <typename BlockType>
	struct OnSet : TOperator<OnSet<BlockType>>
	{
	public:
		constexpr explicit OnSet(BlockType Block) : Block(Block) {}

		template <typename ValueType>
		constexpr const TOptional<ValueType>& operator()(const TOptional<ValueType>& Self)
		{
			if (Self.IsSet())
			{
				Block(*Self);
			}

			return Self;
		}

	private:
		BlockType Block;
	};

	struct IsSet : TOperator<IsSet>
	{
	public:
		template <typename ValueType>
		constexpr bool operator()(const TOptional<ValueType>& Self) { return Self.IsSet(); }
	};
}

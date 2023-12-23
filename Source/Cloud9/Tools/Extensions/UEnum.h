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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"
#include "Cloud9/Tools/Macro/Template.h"

namespace EUEnum
{
	template <typename BlockType, typename ResultType>
	struct AsStaticEnum
	{
		ResultType Default;
		BlockType Block;

		template <typename EnumValueType, enumtype(EnumValueType)>
		FORCEINLINE auto operator()(EnumValueType Self) const
		{
			static_assert(TIsEnum<EnumValueType>::Value, "Should only call this with enum types");

			// https://forums.unrealengine.com/t/how-to-retrieve-uenum-by-type/418501/4
			if (let EnumClass = StaticEnum<EnumValueType>())
			{
				let EnumValue = static_cast<uint8>(Self);
				return Block(EnumClass, EnumValue);
			}

			log(Error, "Can't get static enum class for value '%d'", Self);

			return Default;
		}

		OPERATOR_BODY(AsStaticEnum)
	};

	struct GetEnumName
	{
		template <typename EnumValueType, enumtype(EnumValueType)>
		FORCEINLINE FName operator()(EnumValueType Self) const
		{
			return Self | AsStaticEnum{
				FName(NAME_None),
				[](let EnumClass, let EnumValue) { return EnumClass->GetEnumName(EnumValue); }
			};
		}

		OPERATOR_BODY(GetEnumName)
	};

	struct GetEnumFullValueName
	{
		template <typename EnumValueType, enumtype(EnumValueType)>
		FORCEINLINE FName operator()(EnumValueType Self) const
		{
			return Self | AsStaticEnum{
				FName(NAME_None),
				[](let EnumClass, let EnumValue) { return EnumClass->GetNameByValue(EnumValue); }
			};
		}

		OPERATOR_BODY(GetEnumFullValueName)
	};

	/**
	 * Extension returns only name of value unlike standard method of UEnum::GetNameByIndex
	 *
	 * @param ValueId Only meaningful when called against UEnum not enum class
	 * @returns Name of enum value
	 */
	struct GetValueName
	{
		int ValueId = -1;

		FORCEINLINE FName operator()(const UEnum* Self) const
		{
			if (ValueId < 0)
			{
				log(Error, "GetValueName should not be called with ValueId < 0");
				return {NAME_None};
			}

			return GetOnlyValueName(Self, ValueId);
		}

		template <typename EnumValueType, enumtype(EnumValueType)>
		FORCEINLINE FName operator()(EnumValueType Self) const
		{
			return Self | AsStaticEnum{
				FName(NAME_None),
				[this](let Enum, let Value) { return GetOnlyValueName(Enum, Value); }
			};
		}

		OPERATOR_BODY(GetValueName)

	private:
		static FName GetOnlyValueName(const UEnum* Enum, const int Value)
		{
			let Name = Enum->GetNameByIndex(Value);

			if (Name.IsNone())
			{
				log(Error, "Invalid value name");
				return {NAME_None};
			}

			let String = Name.ToString();

			FString EnumNamePtr;
			FString ValueNamePtr;

			let IsSplit = String.Split(
				TEXT("::"),
				&EnumNamePtr,
				&ValueNamePtr,
				ESearchCase::CaseSensitive,
				ESearchDir::FromEnd);

			return IsSplit ? FName{*ValueNamePtr} : FName{Name};
		}
	};
}

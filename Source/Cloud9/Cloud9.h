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

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCloud9, Log, All);

// Simplification of const auto/auto
#define let const auto
#define var auto

// https://www.kevin-agwaze.com/logging-in-unreal-made-simple-with-helper-macros/

// Current Class Name + Function Name where this is called
#define TRACE_STR_CUR_CLASS_FUNC (FString(__FUNCTION__))

// Current Class where this is called
#define TRACE_STR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

// Current Function Name where this is called
#define TRACE_STR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

// Current Line Number in the code where this is called
#define TRACE_STR_CUR_LINE  (FString::FromInt(__LINE__))

// Current Class and Line Number where this is called
#define TRACE_STR_CUR_CLASS_LINE (TRACE_STR_CUR_CLASS + ":" + TRACE_STR_CUR_LINE + " ")

// Current Class Name + Function Name and Line Number where this is called
#define TRACE_STR_CUR_CLASS_FUNC_LINE (TRACE_STR_CUR_CLASS_FUNC + ":" + TRACE_STR_CUR_LINE + " ")

// Current Function Signature where this is called
#define TRACE_STR_CUR_FUNCSIG (FString(__FUNCSIG__))

// Screen Message
#define screen(FormatString, ...) ( \
	GEngine->AddOnScreenDebugMessage( \
		-1, 10.0f, FColor::Yellow, \
		*(TRACE_STR_CUR_CLASS_FUNC_LINE + ": " + (FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))) \
	) \
)

// UE LOG

#define log(Severity, FormatString, ...) \
	UE_LOG(\
		LogCloud9, \
		Severity, \
		TEXT("%s: %s"), \
		*TRACE_STR_CUR_CLASS_FUNC_LINE, \
		*FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) \
	)

#define assert(Condition) check(Condition)

#define assertf(Condition, FormatString, ...) \
	checkf(Condition, TEXT(FormatString), ##__VA_ARGS__)

// Operator base class

template <typename FunctionType>
class TOperator
{
public:
	template <typename SelfType>
	constexpr friend auto operator|(SelfType* Self, FunctionType&& Function)
	{
		return Function(Self);
	}

	template <typename SelfType>
	constexpr friend auto operator|(const SelfType* Self, FunctionType&& Function)
	{
		return Function(Self);
	}

	template <typename SelfType>
	constexpr friend auto operator|(SelfType& Self, FunctionType&& Function)
	{
		return Function(Forward<SelfType>(Self)); // TODO: May be use Function(MoveTempIfPossible(Self)) ???
	}

	template <typename SelfType>
	constexpr friend auto operator|(SelfType&& Self, FunctionType&& Function)
	{
		return Function(Forward<SelfType>(Self));
	}
};


#define OPERATOR_BODY(OperatorType) \
	template <typename SelfType> \
	constexpr friend auto operator|(SelfType* Self, OperatorType&& Operator)\
	{\
		return Operator(Self);\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(const SelfType* Self, OperatorType&& Operator)\
	{\
		return Operator(Self);\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(SelfType& Self, OperatorType&& Operator)\
	{\
		return Operator(Forward<SelfType>(Self));\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(SelfType&& Self, OperatorType&& Operator)\
	{\
		return Operator(Forward<SelfType>(Self));\
	}

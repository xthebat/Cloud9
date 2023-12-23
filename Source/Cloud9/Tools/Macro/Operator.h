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

#include "Cloud9/Tools/Extensions/Stl.h"

#define OPERATOR_BODY(InOperatorType) \
	\
	using OperatorType = InOperatorType; \
	\
	template <typename SelfType> \
	constexpr friend auto operator|(SelfType* Self, InOperatorType&& Operator)\
	{\
		return Operator(Self);\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(const SelfType* Self, InOperatorType&& Operator)\
	{\
		return Operator(Self);\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(SelfType& Self, InOperatorType&& Operator)\
	{\
		return Operator(Forward<SelfType>(Self));\
	}\
	\
	template <typename SelfType>\
	constexpr friend auto operator|(SelfType&& Self, InOperatorType&& Operator)\
	{\
		return Operator(Forward<SelfType>(Self));\
	}

template <typename, typename = void>
struct TIsOperator
{
	enum { Value = false };
};

template <typename T>
struct TIsOperator<T, Stl::TVoid<typename T::OperatorType>>
{
	enum { Value = true };
};

// Operator base class

// TODO: TOperator should be replaced with OPERATOR_BODY macro
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

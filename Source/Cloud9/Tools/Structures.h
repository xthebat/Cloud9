// Copyright (c) 2024 Alexei Gladkikh

#pragma once

template <typename ErrorType, typename ValueType>
struct TErrorValue
{
	ErrorType Error;
	TOptional<ValueType> Value;

	TErrorValue(ErrorType Error, ValueType&& Value) : Error(Error), Value(MoveTemp(Value)) {}

	TErrorValue(ValueType&& Value) : TErrorValue({}, Value) {}

	TErrorValue(ErrorType Error) : TErrorValue(Error, {}) {}
};

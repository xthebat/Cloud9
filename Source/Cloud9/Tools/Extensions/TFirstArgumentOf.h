#pragma once

namespace Private_TFirstArgumentOf
{
	// ReSharper disable once CppFunctionIsNotImplemented
	template <typename Function, typename ReturnType, typename A, typename... Rest>
	A Helper(ReturnType (Function::*)(A, Rest...));

	// ReSharper disable once CppFunctionIsNotImplemented
	template <typename Function, typename ReturnType, typename A, typename... Rest>
	A Helper(ReturnType (Function::*)(A, Rest...) const);
}

/**
 * Trait for the type of the first argument of invoking function (don't gonna work with static function now).
 */
template <typename F>
struct TFirstArgumentOf
{
	using Type = decltype(Private_TFirstArgumentOf::Helper(&F::operator()));
};

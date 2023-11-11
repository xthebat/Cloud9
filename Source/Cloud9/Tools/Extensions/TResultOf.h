#pragma once

namespace Private_TResultOf
{
	// ReSharper disable once CppFunctionIsNotImplemented
	template <typename Function, typename ReturnType, typename A, typename... Rest>
	ReturnType ResultOfHelper(ReturnType (Function::*)(A, Rest...));

	// ReSharper disable once CppFunctionIsNotImplemented
	template <typename Function, typename ReturnType, typename A, typename... Rest>
	ReturnType ResultOfHelper(ReturnType (Function::*)(A, Rest...) const);
}

/**
 * Trait for the type of the result when invoking.
 * Unlike TInvokeResult aren't using SFINAE to disable 'Type' if F can't be called with given args. 
 */
template <typename F>
struct TResultOf
{
	using Type = decltype(Private_TResultOf::ResultOfHelper(&F::operator()));
};

#pragma once

#include "Templates/IsSame.h"

/**
 * @brief Checks to see if the given type is "callable" with the given parameter types.
 *
 * @tparam ReturnType The expected return type.
 * @tparam CallableType The callable type to check.
 * @tparam ParamTypes The parameter types.
 */
template<typename ReturnType, typename CallableType, typename... ParamTypes>
struct TIsCallable
{
	static constexpr bool Value = requires(CallableType callable, ParamTypes... params)
	{
		callable(params...);
		requires (TIsSame<ReturnType, decltype(callable(params...))>::Value);
	};
};

template<typename ReturnType, typename CallableType, typename... ParamTypes>
inline constexpr bool IsCallable = TIsCallable<ReturnType, CallableType, ParamTypes...>::Value;
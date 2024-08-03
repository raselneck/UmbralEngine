#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/IntegralConstant.h"

/**
 * @brief Checks to see if the given type is "complete" at the current time.
 *
 * @tparam T The type to check.
 */
template<typename T>
struct TIsTypeComplete
{
	static constexpr bool Value = requires()
	{
		sizeof(T);
	};
};

/**
 * @brief Checks to see if the given type is "incomplete" at the current time.
 *
 * @tparam T The type to check.
 */
template<typename T>
struct TIsTypeIncomplete
{
	static constexpr bool Value = Not<TIsTypeComplete<T>>;
};

template<typename T>
inline constexpr bool IsTypeComplete = TIsTypeComplete<T>::Value;

template<typename T>
inline constexpr bool IsTypeIncomplete = Not<TIsTypeComplete<T>>;
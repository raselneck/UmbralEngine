#pragma once

#include "Templates/Move.h"

/**
 * @brief Swaps two elements.
 *
 * @tparam T The element type.
 * @param first The first element.
 * @param second The second element.
 */
template<typename T>
constexpr void Swap(T& first, T& second)
{
	T temp = MoveTemp(first);
	first = MoveTemp(second);
	second = MoveTemp(temp);
}
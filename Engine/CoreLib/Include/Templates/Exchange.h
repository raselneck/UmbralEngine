#pragma once

#include "Templates/Forward.h"
#include "Templates/Move.h"

/**
 * @brief Replaces the value of \p object with \p newValue and returns the old value of \p object.
 *
 * @tparam T The type of the object whose value is being updated.
 * @tparam U The type of the new value for \p object.
 * @param object The object whose value is to be updated.
 * @param newValue The new value for \p object.
 * @return The old value of \p object.
 */
template<typename T, typename U = T>
[[nodiscard]] constexpr T Exchange(T& object, U&& newValue)
{
	T oldValue = MoveTemp(object);
	object = Forward<U>(newValue);
	return MoveTemp(oldValue);
}
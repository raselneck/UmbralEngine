#pragma once

#include "Templates/IsConstVolatile.h"
#include "Templates/IsReference.h"

/**
 * @brief Used to indicate that an object may be moved from, but prevents const values from being moved.
 *
 * @tparam T The object's type.
 * @param value The object value to move.
 * @return \p value casted to an r-value reference.
 */
template<typename T>
[[nodiscard]] constexpr AddRValueReference<RemoveReference<T>> MoveTemp(T& value)
{
	using ReturnType = AddRValueReference<RemoveReference<T>>;

	static_assert(TIsConst<T>::Value == false, "Cannot move a const value");
	return static_cast<ReturnType>(value);
}

template<typename T>
[[nodiscard]] constexpr auto MoveTempIfPossible(T& value)
{
	if constexpr (IsConst<T>)
	{
		return value;
	}
	else
	{
		return MoveTemp(value);
	}
}
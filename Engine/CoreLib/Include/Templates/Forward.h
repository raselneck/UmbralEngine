#pragma once

#include "Templates/IsReference.h"

/**
 * @brief Forwards an l-value as an r-value.
 *
 * @tparam T The value type.
 * @param value The value to forward.
 * @return \p value as an r-value.
 */
template<typename T>
[[nodiscard]] constexpr T&& Forward(RemoveReference<T>& value) noexcept
{
	return static_cast<T&&>(value);
}

/**
 * @brief Forwards an r-value as an r-value, and prohibits forwarding of r-values as l-values.
 *
 * @tparam T The value type.
 * @param value The value to forward.
 * @return \p value as an r-value.
 */
template<typename T>
[[nodiscard]] constexpr T&& Forward(RemoveReference<T>&& value) noexcept
{
	static_assert(IsLValueReference<T> == false, "Cannot forward an l-value as an r-value");
	return static_cast<T&&>(value);
}
#pragma once

#include "Templates/IsReference.h"

// https://en.cppreference.com/w/cpp/utility/forward

template<typename T>
[[nodiscard]] constexpr T&& Forward(RemoveReference<T>& value) noexcept
{
	return static_cast<T&&>(value);
}

template<typename T>
[[nodiscard]] constexpr T&& Forward(RemoveReference<T>&& value) noexcept
{
	static_assert(IsLValueReference<T> == false, "Cannot forward an l-value as an r-value");
	return static_cast<T&&>(value);
}

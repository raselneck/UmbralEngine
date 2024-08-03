#pragma once

#include "Engine/Hashing.h"
#include "Templates/IsSame.h"

/**
 * @brief Checks to see if the given type is hashable.
 *
 * @tparam T The type.
 */
template<typename T>
struct TIsHashable
{
	static constexpr bool Value = requires(T value)
	{
		::GetHashCode(value);
		requires (TIsSame<uint64, decltype(::GetHashCode(value))>::Value);
	};
};
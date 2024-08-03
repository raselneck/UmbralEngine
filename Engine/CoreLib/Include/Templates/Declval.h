#pragma once

#include "Templates/IsReference.h"

// https://en.cppreference.com/w/cpp/utility/declval

namespace Private
{
	template<typename T>
	constexpr bool AlwaysFalse = false;
}

template<typename T>
typename TAddRValueReference<T>::Type declval() noexcept
{
	static_assert(Private::AlwaysFalse<T>, "declval not allowed in an evaluated context");
}
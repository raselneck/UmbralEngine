#pragma once

#include "Templates/IsEnum.h"
#include "Templates/TypeIdentity.h"

// https://en.cppreference.com/w/cpp/types/underlying_type

namespace Private
{
	template<bool IsEnumType, typename T>
	struct TUnderlyingTypeHelper
	{
		using Type = T;
	};

	template<typename T>
	struct TUnderlyingTypeHelper<true, T>
	{
		using Type = typename TTypeIdentity<__underlying_type(T)>::Type;
	};
}

template<typename T>
using TUnderlyingType = Private::TUnderlyingTypeHelper<IsEnum<T>, T>;
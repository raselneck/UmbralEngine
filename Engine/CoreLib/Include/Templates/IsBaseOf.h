#pragma once

#include "Templates/IntegralConstant.h"

template<typename BaseType, typename DerivedType>
struct TIsBaseOf
{
	static constexpr bool Value = __is_base_of(BaseType, DerivedType);
};

template<typename BaseType, typename DerivedType>
inline constexpr bool IsBaseOf = TIsBaseOf<BaseType, DerivedType>::Value;
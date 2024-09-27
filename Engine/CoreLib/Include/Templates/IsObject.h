#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsVoid.h"

// This is how Microsoft's STL defines std::is_object_v
template<typename T>
struct TIsObject : TBoolConstant<IsConst<const T> && IsVoid<T> == false>
{
};

template<typename T>
inline constexpr bool IsObject = TIsObject<T>::Value;
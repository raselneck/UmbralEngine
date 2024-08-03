#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsConstVolatile.h"

// https://en.cppreference.com/w/cpp/types/is_floating_point

namespace Private
{
	template<typename T> struct TIsFloatImpl : FFalseType { };
	template<> struct TIsFloatImpl<float> : FTrueType { };
	template<> struct TIsFloatImpl<double> : FTrueType { };
	template<> struct TIsFloatImpl<long double> : FTrueType { };
}

template<typename T>
using TIsFloat = Private::TIsFloatImpl<typename TRemoveConstVolatile<T>::Type>;
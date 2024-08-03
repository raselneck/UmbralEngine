#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/IntegralConstant.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsFunction.h"

// https://en.cppreference.com/w/cpp/types/is_member_pointer

namespace Private
{
	template<typename T>
	struct TIsMemberPointerImpl : FFalseType { };

	template<typename T, typename U>
	struct TIsMemberPointerImpl<T U::*> : FTrueType { };
}

template<typename T>
using TIsMemberPointer = Private::TIsMemberPointerImpl<typename TRemoveCV<T>::Type>;

// https://en.cppreference.com/w/cpp/types/is_member_function_pointer

namespace Private
{
	template<typename T>
	struct TIsMemberFunctionPointerImpl : FFalseType { };

	template<typename T, typename U>
	struct TIsMemberFunctionPointerImpl<T U::*> : TIsFunction<T> { };
}

template<typename T>
using TIsMemberFunctionPointer = Private::TIsMemberFunctionPointerImpl<typename TRemoveCV<T>::Type>;

// https://en.cppreference.com/w/cpp/types/is_member_object_pointer

template<typename T>
using TIsMemberObjectPointer = TAnd<TIsMemberPointer<T>, TNot<TIsMemberFunctionPointer<T>>>;
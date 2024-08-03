#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsReference.h"
#include <type_traits>

// https://en.cppreference.com/w/cpp/types/is_assignable

template<typename ToType, typename FromType>
using TIsAssignable = TBoolConstant<__is_assignable(ToType, FromType)>;

template<typename T>
using TIsCopyAssignable = TIsAssignable<T, typename TAddLValueReference<T>::Type>;

template<typename T>
using TIsMoveAssignable = TIsAssignable<T, typename TAddRValueReference<T>::Type>;

template<typename ToType, typename FromType>
using TIsTriviallyAssignable = TBoolConstant<__is_trivially_assignable(ToType, FromType)>;

template<typename T>
using TIsTriviallyCopyAssignable = TIsTriviallyAssignable<T, typename TAddLValueReference<T>::Type>;

template<typename T>
using TIsTriviallyMoveAssignable = TIsTriviallyAssignable<T, typename TAddRValueReference<T>::Type>;
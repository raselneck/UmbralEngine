#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/IsConvertible.h"

// https://en.cppreference.com/w/cpp/types/is_enum

template<typename T>
using TIsEnum = TBoolConstant<__is_enum(T)>;

template<typename T>
inline constexpr bool IsEnum = TIsEnum<T>::Value;

// https://en.cppreference.com/w/cpp/types/is_scoped_enum

template<typename T>
using TIsEnumClass = TAnd<TIsEnum<T>, TNot<TIsConvertible<T, int>>>;

template<typename T>
inline constexpr bool IsEnumClass = TIsEnumClass<T>::Value;
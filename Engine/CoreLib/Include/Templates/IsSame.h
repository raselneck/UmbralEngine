#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/IntegralConstant.h"

// https://en.cppreference.com/w/cpp/types/is_same

template<typename T, typename U>
struct TIsSame : FFalseType
{
};

template<typename T>
struct TIsSame<T, T> : FTrueType
{
};

template<typename T, typename U>
inline constexpr bool IsSame = TIsSame<T, U>::Value;

// https://en.cppreference.com/w/cpp/concepts/same_as
// TODO Should we have a type prefix for concepts?

template<typename T, typename U>
concept SameAs = TAnd<TIsSame<T, U>, TIsSame<U, T>>::Value;
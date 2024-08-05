#pragma once

#include "Engine/IntTypes.h"
#include "Templates/AndNotOr.h"
#include "Templates/IntegralConstant.h"
#include "Templates/IsPOD.h"

/**
 * @brief Used to determine if a type can be zero-constructed. A type can be zero-constructed if it would be valid to say that the type
 *        has been successfully initialized for an equivalently-sized region of memory filled with zero bytes.
 *
 * @tparam T The type.
 */
template<typename T>
struct TIsZeroConstructible : TIsPOD<T>
{
};

template<typename T>
struct TIsZeroConstructible<T[]> : TIsZeroConstructible<T>
{
};

template<typename T, usize N>
struct TIsZeroConstructible<T[N]> : TIsZeroConstructible<T>
{
};

template<typename T>
inline constexpr bool IsZeroConstructible = TIsZeroConstructible<T>::Value;
#pragma once

#include "Templates/IsSame.h"

template<typename T> using TIsVoid = TIsSame<T, void>;

template<typename T>
inline constexpr bool IsVoid = TIsVoid<T>::Value;
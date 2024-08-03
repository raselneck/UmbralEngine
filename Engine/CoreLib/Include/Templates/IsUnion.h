#pragma once

#include "Templates/IntegralConstant.h"

// https://en.cppreference.com/w/cpp/types/is_union

template<typename T>
using TIsUnion = TBoolConstant<__is_union(T)>;
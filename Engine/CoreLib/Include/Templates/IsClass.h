#pragma once

#include "Templates/IntegralConstant.h"

// https://en.cppreference.com/w/cpp/types/is_class

template<typename T> using TIsClass = TBoolConstant<__is_class(T)>;
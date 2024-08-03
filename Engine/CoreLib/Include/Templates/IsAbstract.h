#pragma once

#include "Templates/IntegralConstant.h"

// https://en.cppreference.com/w/cpp/types/is_abstract

template<typename T>
using TIsAbstract = TBoolConstant<__is_abstract(T)>;
#pragma once

#include "Templates/IntegralConstant.h"

/**
 * @brief Checks to see if a type can be considered plain-old-data.
 *
 * @tparam T The type.
 */
template<typename T>
struct TIsPOD : TBoolConstant<__is_pod(T)>
{
};
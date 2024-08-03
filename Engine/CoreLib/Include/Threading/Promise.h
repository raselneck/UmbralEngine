#pragma once

#include "Misc/Badge.h"

/**
 * @brief Defines a way to check on the result of an async operation.
 *
 * @tparam T The type of the result of the async operation.
 */
template<typename T>
class TPromise
{
};

/**
 * @brief Defines a way to check on the result of an async operation.
 */
template<>
class TPromise<void>
{
};
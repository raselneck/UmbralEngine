#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief Defines a typed iteration decision.
 */
enum class EIterationDecision : bool
{
	Continue = true,
	Break = false
};
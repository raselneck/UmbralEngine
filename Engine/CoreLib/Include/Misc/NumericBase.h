#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief An enumeration of numeric bases.
 *
 * @remarks The values may be cast to an integer for their equivalent values.
 */
enum class ENumericBase : uint8
{
	Binary = 2,
	Octal = 8,
	Decimal = 10,
	Hexadecimal = 16
};
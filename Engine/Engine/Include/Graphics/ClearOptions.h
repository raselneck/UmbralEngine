#pragma once

#include "Misc/EnumMacros.h"

/**
 * @brief Defines the buffers for clearing when calling FGraphicsDevice.Clear.
 */
enum class EClearOptions
{
	/** @brief No buffer. */
	None = 0,

	/** @brief Color buffer. */
	Color = (1 << 0),

	/** @brief Depth buffer. */
	Depth = (1 << 1),

	/** @brief Stencil buffer. */
	Stencil = (1 << 2),

	/** @brief All buffers. */
	All = Color | Depth | Stencil
};

ENUM_FLAG_OPERATORS(EClearOptions)
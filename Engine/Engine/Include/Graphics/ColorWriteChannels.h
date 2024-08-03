#pragma once

#include "Misc/EnumMacros.h"

/**
 * @brief Defines the color channels for render target blending operations.
 */
enum class EColorWriteChannels
{
	/** @brief No channels selected. */
	None = 0,
	/** @brief Red channel selected. */
	Red = (1 << 0),
	/** @brief Green channel selected. */
	Green = (1 << 1),
	/** @brief Blue channel selected. */
	Blue = (1 << 2),
	/** @brief Alpha channel selected. */
	Alpha = (1 << 3),
	/** @brief All channels selected. */
	All = Red | Green | Blue | Alpha
};

ENUM_FLAG_OPERATORS(EColorWriteChannels)
#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines formats for depth-stencil buffer.
 */
enum class EDepthFormat
{
	/** @brief Depth-stencil buffer will not be created. */
	None,

	/** @brief 16-bit depth buffer. */
	Depth16,

	/** @brief 24-bit depth buffer. */
	Depth24,

	/** @brief 32-bit depth-stencil buffer. Where 24-bit depth and 8-bit for stencil used. */
	Depth24Stencil8
};

/**
 * @brief Converts a depth format value to a string.
 *
 * @param value The depth format value.
 * @return The depth format value as a string.
 */
constexpr FStringView ToString(const EDepthFormat value)
{
#define RETURN_CASE_NAME(Name) case EDepthFormat::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(None);
	RETURN_CASE_NAME(Depth16);
	RETURN_CASE_NAME(Depth24);
	RETURN_CASE_NAME(Depth24Stencil8);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
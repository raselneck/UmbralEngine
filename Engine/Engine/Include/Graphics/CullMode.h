#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines a culling mode for faces in rasterization process.
 */
enum class ECullMode
{
	/** @brief Do not cull faces. */
	None,

	/** @brief Cull faces with clockwise order. */
	CullClockwiseFace,

	/** @brief Cull faces with counter clockwise order. */
	CullCounterClockwiseFace
};

/**
 * @brief Converts a cull mode value to a string.
 *
 * @param value The cull move value.
 * @return The cull mode value as a string.
 */
constexpr FStringView ToString(const ECullMode value)
{
#define RETURN_CASE_NAME(Name) case ECullMode::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(None);
	RETURN_CASE_NAME(CullClockwiseFace);
	RETURN_CASE_NAME(CullCounterClockwiseFace);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines options for filling the primitive.
 */
enum class EFillMode
{
	/** @brief Draw solid faces for each primitive. */
	Solid,

	/** @brief Draw lines for each primitive. */
	WireFrame
};

/**
 * @brief Converts a fill mode value to a string.
 *
 * @param value The fill mode value.
 * @return The fill mode value as a string.
 */
constexpr FStringView ToString(const EFillMode value)
{
#define RETURN_CASE_NAME(Name) case EFillMode::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(Solid);
	RETURN_CASE_NAME(WireFrame);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
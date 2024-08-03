#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines stencil buffer operations.
 */
enum class EStencilOperation
{
	/** @brief Does not update the stencil buffer entry. */
	Keep,

	/** @brief Sets the stencil buffer entry to 0. */
	Zero,

	/** @brief Replaces the stencil buffer entry with a reference value. */
	Replace,

	/** @brief Increments the stencil buffer entry, wrapping to 0 if the new value exceeds the maximum value. */
	Increment,

	/** @brief Decrements the stencil buffer entry, wrapping to the maximum value if the new value is less than 0. */
	Decrement,

	/** @brief Increments the stencil buffer entry, clamping to the maximum value. */
	IncrementSaturation,

	/** @brief Decrements the stencil buffer entry, clamping to 0. */
	DecrementSaturation,

	/** @brief Inverts the bits in the stencil buffer entry. */
	Invert
};

/**
 * @brief Converts a stencil operation value to a string.
 *
 * @param value The stencil operation value.
 * @return The stencil operation value as a string.
 */
constexpr FStringView ToString(const EStencilOperation value)
{
#define RETURN_CASE_NAME(Name) case EStencilOperation::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(Keep);
	RETURN_CASE_NAME(Zero);
	RETURN_CASE_NAME(Replace);
	RETURN_CASE_NAME(Increment);
	RETURN_CASE_NAME(Decrement);
	RETURN_CASE_NAME(IncrementSaturation);
	RETURN_CASE_NAME(DecrementSaturation);
	RETURN_CASE_NAME(Invert);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
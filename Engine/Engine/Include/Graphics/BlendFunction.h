#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines a function for color blending.
 */
enum class EBlendFunction
{
	/**
	 * @brief The function will adds destination to the source.
	 *        (srcColor * srcBlend) + (destColor * destBlend)
	 */
	Add,

	/**
	 * @brief The function will subtracts destination from source.
	 *        (srcColor * srcBlend) - (destColor * destBlend)
	 */
	Subtract,

	/**
	 * @brief The function will subtracts source from destination.
	 *        (destColor * destBlend) - (srcColor * srcBlend)
	 */
	ReverseSubtract,

	/**
	 * @brief The function will extracts minimum of the source and destination.
	 *        min((srcColor * srcBlend),(destColor * destBlend))
	 */
	Min,

	/**
	 * @brief The function will extracts maximum of the source and destination.
	 *        max((srcColor * srcBlend),(destColor * destBlend))
	 */
	Max
};

/**
 * @brief Converts a blend function value to a string.
 *
 * @param value The blend function value.
 * @return The blend function value as a string.
 */
constexpr FStringView ToString(const EBlendFunction value)
{
#define RETURN_CASE_NAME(Name) case EBlendFunction::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(Add);
	RETURN_CASE_NAME(Subtract);
	RETURN_CASE_NAME(ReverseSubtract);
	RETURN_CASE_NAME(Min);
	RETURN_CASE_NAME(Max);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
#pragma once

#include "Containers/StringView.h"

/**
 * @brief The comparison function used for depth, stencil, and alpha tests.
 */
enum class ECompareFunction
{
	/** @brief Always passes the test. */
	Always,

	/** @brief Never passes the test. */
	Never,

	/** @brief Passes the test when the new pixel value is less than current pixel value. */
	Less,

	/** @brief Passes the test when the new pixel value is less than or equal to current pixel value. */
	LessEqual,

	/** @brief Passes the test when the new pixel value is equal to current pixel value. */
	Equal,

	/** @brief Passes the test when the new pixel value is greater than or equal to current pixel value. */
	GreaterEqual,

	/** @brief Passes the test when the new pixel value is greater than current pixel value. */
	Greater,

	/** @brief Passes the test when the new pixel value does not equal to current pixel value. */
	NotEqual
};

/**
 * @brief Converts a compare function value to a string.
 *
 * @param value The compare function value.
 * @return The compare function value as a string.
 */
constexpr FStringView ToString(const ECompareFunction value)
{
#define RETURN_CASE_NAME(Name) case ECompareFunction::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(Always);
	RETURN_CASE_NAME(Never);
	RETURN_CASE_NAME(Less);
	RETURN_CASE_NAME(LessEqual);
	RETURN_CASE_NAME(Equal);
	RETURN_CASE_NAME(GreaterEqual);
	RETURN_CASE_NAME(Greater);
	RETURN_CASE_NAME(NotEqual);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}
#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines information about a parsed attribute.
 */
class FParsedAttributeInfo
{
public:

	/** @brief The attribute's name. */
	FStringView Name;

	/** @brief The attribute's value. This may be empty. */
	FStringView Value;
};
#pragma once

#include "Engine/IntTypes.h"
#include "Misc/StringFormatting.h"

/**
 * @brief Defines a location in source text.
 */
class FLoxSourceLocation
{
public:

	/** @brief The line in the source text. */
	int32 Line = 0;

	/** @brief The column in the source text. */
	int32 Column = 0;
};

template<>
struct TFormatter<FLoxSourceLocation>
{
	void BuildString(const FLoxSourceLocation& value, FStringBuilder& builder) const;
	bool Parse(FStringView formatString);
};
#pragma once

#include "Engine/IntTypes.h"
#include "Misc/StringFormatting.h"

/**
 * @brief Defines a location in source text.
 */
class FSourceLocation
{
public:

	/** @brief The line in the source text. */
	int32 Line = 0;

	/** @brief The column in the source text. */
	int32 Column = 0;
};

template<>
struct TFormatter<FSourceLocation>
{
	void BuildString(const FSourceLocation& value, FStringBuilder& builder) const;
	bool Parse(FStringView formatString);
};
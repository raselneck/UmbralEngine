#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "MetaTool/AttributeInfo.h"
#include "MetaTool/EnumUtilities.h"

/**
 * @brief Defines information about a parsed enumeration entry.
 */
class FParsedEnumEntryInfo
{
public:

	/** @brief The entry's name. */
	FStringView EntryName;
};

/**
 * @brief Defines information about a parsed enumeration.
 */
class FParsedEnumInfo
{
public:

	/** @brief The enum's name. */
	FStringView EnumName;

	/** @brief The enum's underlying type. */
	FStringView UnderlyingType = "int32"_sv;

	/** @brief The enum's entries. */
	TArray<FParsedEnumEntryInfo> EnumEntries;

	/** @brief The enum's attributes. */
	TArray<FParsedAttributeInfo> Attributes;
};
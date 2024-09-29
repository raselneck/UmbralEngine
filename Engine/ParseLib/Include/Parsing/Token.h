#pragma once

#include "Containers/StringView.h"
#include "Parsing/SourceLocation.h"
#include "Parsing/TokenType.h"

/**
 * @brief Defines a token.
 */
class FToken
{
public:

	/** The token's text. This will only be valid as long as the source string is kept in memory. */
	FStringView Text;

	/** The source location of the token. */
	FSourceLocation Location;

	/** The token's type. */
	ETokenType Type = ETokenType::Identifier;

	/** The zero-based index of the token within the source. */
	int32 SourceIndex = INDEX_NONE;

	/** The length, or number of characters, of the token within the source. */
	int32 SourceLength = 0;
};
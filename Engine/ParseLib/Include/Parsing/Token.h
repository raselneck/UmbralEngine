#pragma once

#include "Containers/String.h"
#include "Parsing/TokenType.h"

/**
 * @brief Defines a token.
 */
class FToken
{
public:

	/** The token's text. */
	FString Text;

	/** The token's type. */
	ETokenType Type = ETokenType::Identifier;

	/** The source line the token is located at. */
	int32 Line = 0;

	/** The source column the token is located at. */
	int32 Column = 0;
};
#pragma once

#include "Containers/Array.h"
#include "Parsing/Token.h"

/**
 * @brief Defines a token scanner.
 */
class FTokenScanner
{
public:

	/**
	 * @brief Gets the tokens from the last scan.
	 *
	 * @return The tokens from the last scan.
	 */
	[[nodiscard]] TSpan<const FToken> GetTokens() const
	{
		return m_Tokens.AsSpan();
	}

private:

	TArray<FToken> m_Tokens;
};
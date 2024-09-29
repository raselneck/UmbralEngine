#pragma once

#include "Containers/Array.h"
#include "Engine/Error.h"
#include "Parsing/SourceLocation.h"
#include "Parsing/Token.h"

// TODO Make this virtual OR provide a plethora of configuration options

/**
 * @brief The error type used by the token scanner.
 */
using FScannerError = TError<FSourceLocation>;

/**
 * @brief Defines a scanner which can convert source text into a collection of tokens..
 */
class FScanner final
{
public:

	/**
	 * @brief Gets the errors from the last scan.
	 *
	 * @return The tokens from the last scan.
	 */
	[[nodiscard]] TSpan<const FScannerError> GetErrors() const
	{
		return m_Errors.AsSpan();
	}

	/**
	 * @brief Gets the tokens from the last scan.
	 *
	 * @return The tokens from the last scan.
	 */
	[[nodiscard]] TSpan<const FToken> GetTokens() const
	{
		return m_Tokens.AsSpan();
	}

	/**
	 * @brief Checks to see if this scanner encountered any errors.
	 *
	 * @return True if this scanner encountered any errors, otherwise false.
	 */
	[[nodiscard]] bool HasErrors() const
	{
		return m_Errors.Num() > 0;
	}

	/**
	 * @brief Scans the given text for tokens.
	 *
	 * @param text The text.
	 */
	void ScanTextForTokens(FStringView text);

protected:

	/**
	 * @brief Adds a token solely based on its type.
	 *
	 * @param tokenType The token type.
	 */
	[[maybe_unused]] FToken& AddToken(ETokenType tokenType);

	/**
	 * @brief Returns the current character and advances to the next.
	 *
	 * @return The current character.
	 */
	[[maybe_unused]] FStringView::CharType AdvanceChar();

	/**
	 * @brief Gets the currently scanned token's text.
	 *
	 * @return The currently scanned token's text.
	 */
	[[nodiscard]] FStringView GetCurrentTokenText() const;

	/**
	 * @brief Checks to see if this scanner is at the end of the source text.
	 *
	 * @return True if this scanner is at the end of the source text, otherwise false.
	 */
	[[nodiscard]] bool IsAtEnd() const;

	/**
	 * @brief Attempts to match an expected character. If the next character is \p expected, then this scanner will advance.
	 *
	 * @param expected The character to match.
	 * @return True if \p ch was matched, otherwise false.
	 */
	[[nodiscard]] bool Match(FStringView::CharType expected);

	/**
	 * @brief Peeks at the next character in the source text.
	 *
	 * @return The next character in the source text.
	 */
	[[nodiscard]] FStringView::CharType Peek() const;

	/**
	 * @brief Peeks ahead at the character after the next character in the source text.
	 *
	 * @return The character after the next character in the source text.
	 */
	[[nodiscard]] FStringView::CharType PeekNext() const;

	/**
	 * @brief Peeks at the previous character in the source text.
	 *
	 * @return The previous character in the source text.
	 */
	[[nodiscard]] FStringView::CharType PeekPrevious() const;

	/**
	 * @brief Scans for an identifier token.
	 */
	void ScanIdentifier();

	/**
	 * @brief Attempts to scan a number literal.
	 */
	void ScanNumberLiteral();

	/**
	 * @brief Attempts to scan a string literal.
	 */
	void ScanStringLiteral();

	/**
	 * @brief Scans the next token from the source.
	 */
	void ScanToken();

	/**
	 * @brief Moves ahead to the next non-whitespace character.
	 */
	void SkipWhitespace();

private:

	TArray<FScannerError> m_Errors;
	TArray<FToken> m_Tokens;
	FStringView m_Text;
	FStringView::SizeType m_CurrentIndex = 0; // Current index of the character cursor in m_Text
	FStringView::SizeType m_StartIndex = 0;   // Starting index of the current token being parsed
	FSourceLocation m_CurrentLocation;
	FSourceLocation m_StartLocation;
};
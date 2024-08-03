#pragma once

#include "Containers/Array.h"
#include "Lox/Error.h"
#include "Lox/Token.h"

/**
 * @brief Defines a scanner for retrieving Lox tokens from source text.
 */
class FLoxScanner
{
public:

	/**
	 * @brief Gets this scanner's errors, if any were encountered.
	 *
	 * @return This scanner's errors.
	 */
	[[nodiscard]] TSpan<const FLoxError> GetErrors() const
	{
		return m_Errors.AsSpan();
	}

	/**
	 * @brief Gets this scanner's generated tokens.
	 *
	 * @return This scanner's generated tokens.
	 */
	[[nodiscard]] TSpan<const FLoxToken> GetTokens() const
	{
		return m_Tokens.AsSpan();
	}

	/**
	 * @brief Checks to see if this scanner has encountered any errors.
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

private:

	/**
	 * @brief Adds a token solely based on its type.
	 *
	 * @param tokenType The token type.
	 */
	[[maybe_unused]] FLoxToken& AddToken(ELoxTokenType tokenType);

	/**
	 * @brief Adds a token based on its type and its literal value.
	 *
	 * @param tokenType The token type.
	 * @param value The token's value.
	 */
	[[maybe_unused]] FLoxToken& AddToken(ELoxTokenType tokenType, FLoxValue value);

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

	TArray<FLoxToken> m_Tokens;
	TArray<FLoxError> m_Errors;
	FStringView m_Text;
	FStringView::SizeType m_CurrentIndex = 0; // Current index of the character cursor in m_Text
	FStringView::SizeType m_StartIndex = 0;   // Starting index of the current token being parsed
	FLoxSourceLocation m_CurrentLocation;
	FLoxSourceLocation m_StartLocation;
};
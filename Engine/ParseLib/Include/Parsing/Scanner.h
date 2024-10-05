#pragma once

#include "Containers/Array.h"
#include "Parsing/ParseError.h"
#include "Parsing/SourceLocation.h"
#include "Parsing/Token.h"

// TODO Make this virtual OR provide a plethora of configuration options

/**
 * @brief Defines a scanner which can convert source text into a collection of tokens..
 */
class FScanner
{
public:

	/**
	 * @brief Destroys this scanner.
	 */
	virtual ~FScanner() = default;

	/**
	 * @brief Gets the errors from the last scan.
	 *
	 * @return The tokens from the last scan.
	 */
	[[nodiscard]] TSpan<const FParseError> GetErrors() const
	{
		return m_Errors.AsSpan();
	}

	/**
	 * @brief Gets the marker for the beginning of a line comment.
	 *
	 * @return The marker for the beginning of a line comment.
	 */
	[[nodiscard]] FStringView GetLineCommentBegin() const
	{
		return m_LineCommentBegin;
	}

	/**
	 * @brief Gets the beginning of a multi-line comment.
	 *
	 * @return The beginning of a multi-line comment.
	 */
	[[nodiscard]] FStringView GetMultiLineCommentBegin() const
	{
		return m_MultiLineCommentBegin;
	}

	/**
	 * @brief Gets the ending of a multi-line comment.
	 *
	 * @return The ending of a multi-line comment.
	 */
	[[nodiscard]] FStringView GetMultiLineCommentEnd() const
	{
		return m_MultiLineCommentEnd;
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

	/**
	 * @brief Sets the line comment beginning marker.
	 *
	 * @param lineCommentBegin The new line comment beginning marker.
	 */
	void SetLineCommentBegin(const FStringView lineCommentBegin)
	{
		m_LineCommentBegin = lineCommentBegin;
	}

	/**
	 * @brief Sets the markers for a multi-line comment.
	 *
	 * @param multiLineBegin The new beginning of a multi-line comment.
	 * @param multiLineEnd The new ending of a multi-line comment.
	 */
	void SetMultiLineComment(const FStringView multiLineBegin, const FStringView multiLineEnd)
	{
		m_MultiLineCommentBegin = multiLineBegin;
		m_MultiLineCommentEnd = multiLineEnd;
	}

	/**
	 * @brief Sets whether or not this scanner should record comment tokens.
	 *
	 * @param shouldRecordComments True if this scanner should record comment tokens, false if not.
	 */
	void SetShouldRecordComments(bool shouldRecordComments)
	{
		m_ShouldRecordComments = shouldRecordComments;
	}

	/**
	 * @brief Whether or not this scanner should record comment tokens. Comments will still be properly scanned and skipped if they are setup.
	 *
	 * @return True if this scanner should record comment tokens, false if not.
	 */
	[[nodiscard]] bool ShouldRecordComments() const
	{
		return m_ShouldRecordComments;
	}

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
	 * @brief Gets the source text for a token based off of its source index and length.
	 *
	 * @param token The token.
	 * @return The token's source text.
	 */
	[[nodiscard]] FStringView GetTokenText(const FToken& token) const;

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
	 * @return True if \p expected was matched, otherwise false.
	 */
	[[nodiscard]] bool Match(FStringView::CharType expected);

	/**
	 * @brief Attempts to match an expected string. If the string \p expected is matched, then this scanner will advance.
	 *
	 * @param expected The string to match.
	 * @return True if \p expected was matched, otherwise false.
	 */
	[[nodiscard]] bool Match(FStringView expected);

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
	 * @brief Scans for a single line comment.
	 */
	void ScanLineComment();

	/**
	 * @brief Scans for a multi-line comment.
	 */
	void ScanMultiLineComment();

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

	/**
	 * @brief Attempts to scan a token from the current source text position.
	 *
	 * @return True if a token was scanned, otherwise false.
	 */
	virtual bool TryScanTokenFromCurrentPosition() { return false; }

private:

	TArray<FParseError> m_Errors;
	TArray<FToken> m_Tokens;
	FStringView m_LineCommentBegin;
	FStringView m_MultiLineCommentBegin;
	FStringView m_MultiLineCommentEnd;
	FStringView m_Text;
	FStringView::SizeType m_CurrentIndex = 0; // Current index of the character cursor in m_Text
	FStringView::SizeType m_StartIndex = 0;   // Starting index of the current token being parsed
	FSourceLocation m_CurrentLocation;
	FSourceLocation m_StartLocation;
	bool m_ShouldRecordComments = false;
};

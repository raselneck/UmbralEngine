#include "Containers/HashMap.h"
#include "Misc/StringParsing.h"
#include "Parsing/Scanner.h"

/**
 * @brief Checks to see if the given character is an alphabetic character.
 *
 * @param ch The character.
 * @return True if \p ch is alphabetic, otherwise false.
 */
static constexpr bool IsAlpha(const FStringView::CharType ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
}

/**
 * @brief Checks to see if the given character is a numeric digit.
 *
 * @param ch The character.
 * @return True if \p ch is a numeric digit, otherwise false.
 */
static constexpr bool IsDigit(const FStringView::CharType ch)
{
	return ch >= '0' && ch <= '9';
}

/**
 * @brief Checks to see if the given character is alphabetic or numeric.
 *
 * @param ch The character.
 * @return True if the character is a number or letter, otherwise false.
 */
static constexpr bool IsAlphaNumeric(const FStringView::CharType ch)
{
	return IsAlpha(ch) || IsDigit(ch);
}

void FScanner::ScanTextForTokens(const FStringView text)
{
	m_Errors.Reset();
	m_Tokens.Reset();
	m_Text = text;
	m_CurrentIndex = 0;
	m_StartIndex = 0;
	m_StartLocation = { 1, 1 };
	m_CurrentLocation = { 1, 1 };

	while (IsAtEnd() == false)
	{
		SkipWhitespace();

		if (IsAtEnd())
		{
			break;
		}

		m_StartIndex = m_CurrentIndex;
		m_StartLocation = m_CurrentLocation;
		ScanToken();
	}
}

FToken& FScanner::AddToken(const ETokenType tokenType)
{
	FToken& token = m_Tokens.AddDefaultGetRef();
	token.Text = GetCurrentTokenText();
	token.Location = m_StartLocation;
	token.Type = tokenType;
	token.SourceIndex = m_StartIndex;
	token.SourceLength = m_CurrentIndex - m_StartIndex;

	return token;
}

FStringView::CharType FScanner::AdvanceChar()
{
	if (m_Text.IsValidIndex(m_CurrentIndex) == false)
	{
		return '\0';
	}

	const FStringView::CharType result = m_Text[m_CurrentIndex];
	++m_CurrentIndex;
	++m_CurrentLocation.Column;

	if ((Peek() == '\n') || (Peek() == '\r' && PeekNext() != '\n'))
	{
		++m_CurrentLocation.Line;
		m_CurrentLocation.Column = 0;
	}

#if 0
	if (Peek() == '\r')
	{
		// New line is \r\n
		if (PeekNext() == '\n')
		{
			// Do nothing because then the \n will be handled by the next AdvanceChar
		}
		// New line is \r
		else
		{
			// Record new line position
		}
	}
	// New line is \n
	else if (Peek() == '\n')
	{
		// Record new line position
	}
#endif

	return result;
}

FStringView FScanner::GetCurrentTokenText() const
{
	const int32 tokenLength = m_CurrentIndex - m_StartIndex;
	return m_Text.Substring(m_StartIndex, tokenLength);
}

FStringView FScanner::GetTokenText(const FToken& token) const
{
	return m_Text.Substring(token.SourceIndex, token.SourceLength);
}

bool FScanner::IsAtEnd() const
{
	return m_CurrentIndex >= m_Text.Length();
}

bool FScanner::Match(const FStringView::CharType expected)
{
	if (IsAtEnd())
	{
		return false;
	}

	if (m_Text[m_CurrentIndex] != expected)
	{
		return false;
	}

	++m_CurrentIndex;

	return true;
}

bool FScanner::Match(const FStringView expected)
{
	if (IsAtEnd() || expected.IsEmpty())
	{
		return false;
	}

	for (FStringView::SizeType idx = 0; idx < expected.Length(); ++idx)
	{
		if (m_Text.IsValidIndex(m_CurrentIndex + idx) && m_Text[m_CurrentIndex + idx] == expected[idx])
		{
			continue;
		}

		return false;
	}

	m_CurrentIndex += expected.Length();

	return true;
}

FStringView::CharType FScanner::Peek() const
{
	if (IsAtEnd())
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex];
}

FStringView::CharType FScanner::PeekNext() const
{
	if (m_CurrentIndex + 1 >= m_Text.Length())
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex + 1];
}

FStringView::CharType FScanner::PeekPrevious() const
{
	if (m_CurrentIndex <= 0)
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex - 1];
}

void FScanner::ScanIdentifier()
{
	while (IsAlphaNumeric(Peek()))
	{
		AdvanceChar();
	}

	AddToken(ETokenType::Identifier);
}

void FScanner::ScanLineComment()
{
	while (IsAtEnd() == false && Peek() != '\n' && Peek() != '\r')
	{
		AdvanceChar();
	}

	FToken& token = AddToken(ETokenType::Comment);
	token.SourceIndex += m_LineCommentBegin.Length();
	token.SourceLength -= m_LineCommentBegin.Length();
	token.Text = GetTokenText(token);
}

void FScanner::ScanMultiLineComment()
{
	while (IsAtEnd() == false && Match(m_MultiLineCommentEnd) == false)
	{
		AdvanceChar();
	}

	FToken& token = AddToken(ETokenType::Comment);
	token.SourceIndex += m_MultiLineCommentBegin.Length();
	token.SourceLength -= m_MultiLineCommentBegin.Length() + m_MultiLineCommentEnd.Length();
	token.Text = GetTokenText(token);
}

void FScanner::ScanNumberLiteral()
{
	while (IsDigit(Peek()))
	{
		AdvanceChar();
	}

	AddToken(ETokenType::Number);
}

void FScanner::ScanStringLiteral()
{
	while (Peek() != '"' && IsAtEnd() == false)
	{
		if (Peek() == '\n' || Peek() == '\r')
		{
			(void)m_Errors.Emplace(m_CurrentLocation, "Unexpected new line in string"_sv);
			return;
		}

		AdvanceChar();
	}

	if (IsAtEnd())
	{
		(void)m_Errors.Emplace(m_CurrentLocation, "Encountered unterminated string"_sv);
		return;
	}

	AdvanceChar(); // The closing "

	// Trim the surrounding quotes for the string value
	FToken& token = AddToken(ETokenType::String);
	token.SourceIndex = m_StartIndex + 1;
	token.SourceLength = (m_CurrentIndex - 1) - (m_StartIndex + 1);
	token.Text = GetTokenText(token);
}

void FScanner::ScanToken()
{
	if (TryScanTokenFromCurrentPosition())
	{
		return;
	}

	if (Match(m_LineCommentBegin))
	{
		return ScanLineComment();
	}
	if (Match(m_MultiLineCommentBegin))
	{
		return ScanMultiLineComment();
	}

	const char ch = AdvanceChar();
	switch (ch)
	{
	// TODO Configurable :)
	case '"':
		ScanStringLiteral();
		break;

	case '\'': AddToken(ETokenType::SingleQuote);   break;
	case '(':  AddToken(ETokenType::LeftParen);     break;
	case ')':  AddToken(ETokenType::RightParent);   break;
	case '[':  AddToken(ETokenType::LeftBracket);   break;
	case ']':  AddToken(ETokenType::RightBracket);  break;
	case '{':  AddToken(ETokenType::LeftBrace);     break;
	case '}':  AddToken(ETokenType::RightBrace);    break;
	case '<':  AddToken(ETokenType::LessThan);      break;
	case '>':  AddToken(ETokenType::GreaterThan);   break;
	case '_':  AddToken(ETokenType::Underscore);    break;
	case '.':  AddToken(ETokenType::Period);        break;
	case ',':  AddToken(ETokenType::Comma);         break;
	case ':':  AddToken(ETokenType::Colon);         break;
	case ';':  AddToken(ETokenType::Semicolon);     break;
	case '+':  AddToken(ETokenType::Plus);          break;
	case '-':  AddToken(ETokenType::Minus);         break;
	case '*':  AddToken(ETokenType::Asterisk);      break;
	case '/':  AddToken(ETokenType::Slash);         break;
	case '=':  AddToken(ETokenType::Equal);         break;
	case '^':  AddToken(ETokenType::Caret);         break;
	case '!':  AddToken(ETokenType::Exclamation);   break;
	case '?':  AddToken(ETokenType::Question);      break;
	case '&':  AddToken(ETokenType::Ampersand);     break;
	case '%':  AddToken(ETokenType::Percent);       break;
	case '#':  AddToken(ETokenType::Octothorpe);    break;
	case '~':  AddToken(ETokenType::Tilde);         break;
	case '`':  AddToken(ETokenType::Backtick);      break;

	default:
		if (IsDigit(ch))
		{
			ScanNumberLiteral();
		}
		else if (IsAlpha(ch))
		{
			ScanIdentifier();
		}
		else
		{
			m_Errors.Add(FParseError::Format(m_CurrentLocation, "Unexpected character \"{}\""_sv, ch));
		}
		break;
	}
}

void FScanner::SkipWhitespace()
{
	const auto IsWhitespace = [](const FStringView::CharType ch)
	{
		return ch <= ' ';
	};

	while (IsAtEnd() == false && IsWhitespace(Peek()))
	{
		AdvanceChar();
	}
}

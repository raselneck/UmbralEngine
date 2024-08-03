#include "Containers/HashMap.h"
#include "Lox/Scanner.h"
#include "Misc/StringParsing.h"

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

void FLoxScanner::ScanTextForTokens(const FStringView text)
{
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

FLoxToken& FLoxScanner::AddToken(const ELoxTokenType tokenType)
{
	return AddToken(tokenType, {});
}

FLoxToken& FLoxScanner::AddToken(const ELoxTokenType tokenType, FLoxValue value)
{
	const FStringView text = GetCurrentTokenText();

	FLoxToken& token = m_Tokens.AddDefaultGetRef();
	token.Value = MoveTemp(value);
	token.Text = FString { text };
	token.SourceLocation = m_StartLocation;
	token.Type = tokenType;

	return token;
}

FStringView::CharType FLoxScanner::AdvanceChar()
{
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

FStringView FLoxScanner::GetCurrentTokenText() const
{
	const int32 tokenLength = m_CurrentIndex - m_StartIndex;
	return m_Text.Substring(m_StartIndex, tokenLength);
}

bool FLoxScanner::IsAtEnd() const
{
	return m_CurrentIndex >= m_Text.Length();
}

bool FLoxScanner::Match(const FStringView::CharType expected)
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

FStringView::CharType FLoxScanner::Peek() const
{
	if (IsAtEnd())
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex];
}

FStringView::CharType FLoxScanner::PeekNext() const
{
	if (m_CurrentIndex + 1 >= m_Text.Length())
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex + 1];
}

FStringView::CharType FLoxScanner::PeekPrevious() const
{
	if (m_CurrentIndex <= 0)
	{
		return FStringView::CharTraitsType::NullChar;
	}

	return m_Text[m_CurrentIndex - 1];
}

void FLoxScanner::ScanIdentifier()
{
	static const THashMap<FStringView, ELoxTokenType> keywords
	{{
		{ "true"_sv, ELoxTokenType::True },
		{ "false"_sv, ELoxTokenType::False },
		{ "null"_sv, ELoxTokenType::Null },

		{ "and"_sv, ELoxTokenType::And },
		{ "or"_sv, ELoxTokenType::Or },
		{ "not"_sv, ELoxTokenType::Not },
		{ "if"_sv, ELoxTokenType::If },
		{ "else"_sv, ELoxTokenType::Else },
		{ "let"_sv, ELoxTokenType::Let },
		{ "const"_sv, ELoxTokenType::Const },
		{ "while"_sv, ELoxTokenType::While },
		{ "for"_sv, ELoxTokenType::For },
		{ "switch"_sv, ELoxTokenType::Switch },
		{ "case"_sv, ELoxTokenType::Case },
		{ "function"_sv, ELoxTokenType::Function },
		{ "return"_sv, ELoxTokenType::Return },
		{ "class"_sv, ELoxTokenType::Class },
		{ "interface"_sv, ELoxTokenType::Interface },
		{ "super"_sv, ELoxTokenType::Super },

		{ "is"_sv, ELoxTokenType::Is },
		{ "int"_sv, ELoxTokenType::IntTerm },
		{ "float"_sv, ELoxTokenType::FloatTerm },
		{ "string"_sv, ELoxTokenType::StringTerm },
		{ "bool"_sv, ELoxTokenType::BoolTerm },
	}};

	while (IsAlphaNumeric(Peek()))
	{
		AdvanceChar();
	}

	const FStringView identifier = GetCurrentTokenText();
	if (const ELoxTokenType* identifierType = keywords.Find(identifier))
	{
		FLoxToken& token = AddToken(*identifierType);

		if (token.Type == ELoxTokenType::True)
		{
			token.Value = FLoxValue::FromBool(true);
		}
		else if (token.Type == ELoxTokenType::False)
		{
			token.Value = FLoxValue::FromBool(false);
		}
	}
	else
	{
		AddToken(ELoxTokenType::Identifier);
	}
}

void FLoxScanner::ScanNumberLiteral()
{
	while (IsDigit(Peek()))
	{
		AdvanceChar();
	}

	// Look for a fractional part
	bool isFloat = false;
	if (Peek() == '.' && IsDigit(PeekNext()))
	{
		isFloat = true;

		AdvanceChar(); // Consume the "."

		while (IsDigit(Peek()))
		{
			AdvanceChar();
		}
	}

	const FStringView numberText = GetCurrentTokenText();

	if (isFloat)
	{
		const TOptional<double> numberValue = FStringParser::TryParseDouble(numberText);
		if (numberValue.HasValue())
		{
			AddToken(ELoxTokenType::Float, FLoxValue::FromFloat(numberValue.GetValue()));
		}
		else
		{
			(void)m_Errors.Emplace(m_StartLocation, "Failed to parse float value"_sv);
		}
	}
	else
	{
		const TOptional<int64> numberValue = FStringParser::TryParseInt64(numberText);
		if (numberValue.HasValue())
		{
			AddToken(ELoxTokenType::Integer, FLoxValue::FromInt(numberValue.GetValue()));
		}
		else
		{
			(void)m_Errors.Emplace(m_StartLocation, "Failed to parse integer value"_sv);
		}
	}
}

void FLoxScanner::ScanStringLiteral()
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
	const int32 valueLength = (m_CurrentIndex - 1) - (m_StartIndex + 1);
	const FStringView value = m_Text.Substring(m_StartIndex + 1, valueLength);
	AddToken(ELoxTokenType::String, FLoxValue::FromString(value));
}

void FLoxScanner::ScanToken()
{
	const char ch = AdvanceChar();
	switch (ch)
	{
	case '+':
		AddToken(Match('=') ? ELoxTokenType::PlusEqual : ELoxTokenType::Plus);
		break;
	case '-':
		AddToken(Match('=') ? ELoxTokenType::MinusEqual : ELoxTokenType::Minus);
		break;
	case '*':
		AddToken(Match('=') ? ELoxTokenType::AsteriskEqual : ELoxTokenType::Asterisk);
		break;
	case '/':
		// Full line comment
		if (Match('/'))
		{
			while (Peek() != '\n' && IsAtEnd() == false)
			{
				AdvanceChar();
			}
		}
		else
		{
			AddToken(Match('=') ? ELoxTokenType::SlashEqual : ELoxTokenType::Slash);
		}
		break;
	case '^':
		AddToken(ELoxTokenType::Caret);
		break;
	case '=':
		AddToken(Match('=') ? ELoxTokenType::EqualEqual : ELoxTokenType::Equal);
		break;
	case '!':
		AddToken(Match('=') ? ELoxTokenType::BangEqual : ELoxTokenType::Bang);
		break;
	case '>':
		AddToken(Match('=') ? ELoxTokenType::GreaterEqual : ELoxTokenType::Greater);
		break;
	case '<':
		AddToken(Match('=') ? ELoxTokenType::LessEqual : ELoxTokenType::Less);
		break;

	case '(':
		AddToken(ELoxTokenType::LeftParen);
		break;
	case ')':
		AddToken(ELoxTokenType::RightParen);
		break;
	case '[':
		AddToken(ELoxTokenType::LeftBracket);
		break;
	case ']':
		AddToken(ELoxTokenType::RightBracket);
		break;
	case '{':
		AddToken(ELoxTokenType::LeftBrace);
		break;
	case '}':
		AddToken(ELoxTokenType::RightBrace);
		break;

	case '.':
		AddToken(ELoxTokenType::Dot);
		break;
	case ',':
		AddToken(ELoxTokenType::Comma);
		break;
	case ':':
		AddToken(ELoxTokenType::Colon);
		break;
	case ';':
		AddToken(ELoxTokenType::Semicolon);
		break;
	case '?':
		AddToken(ELoxTokenType::QuestionMark);
		break;

	case '"':
		ScanStringLiteral();
		break;

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
			// TODO Maybe m_StartLocation
			m_Errors.Add(FLoxError::Format(m_CurrentLocation, "Unexpected character \"{}\""_sv, ch));
		}
		break;
	}
}

void FLoxScanner::SkipWhitespace()
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
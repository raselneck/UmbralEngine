#include "Parsing/Parser.h"

void FParser::ParseTokens(const TSpan<const FToken> tokens)
{
	m_Errors.Reset();
	m_Tokens = tokens;
	m_TokenIndex = 0;

	if (m_Tokens.IsEmpty())
	{
		return;
	}

	OnParseBegin();

	while (IsAtEnd() == false && ParseFromCurrentToken() == EIterationDecision::Continue)
	{
	}

	OnParseEnd();
}

const FToken& FParser::AdvanceToken()
{
	if (IsAtEnd() == false)
	{
		++m_TokenIndex;
	}

	return PeekPrevious();
}

bool FParser::Check(const ETokenType type) const
{
	if (IsAtEnd())
	{
		return false;
	}

	return Peek().Type == type;
}

bool FParser::Consume(const ETokenType tokenType, const FStringView message)
{
	if (Check(tokenType))
	{
		AdvanceToken();
		return true;
	}

	if (Peek().Type == ETokenType::EndOfSource)
	{
		RecordError(PeekPrevious().Location, message);
	}
	else
	{
		RecordError(Peek().Location, message);
	}

	return false;
}

bool FParser::IsAtEnd() const
{
	return m_TokenIndex >= m_Tokens.Num();
}

const FToken& FParser::Peek() const
{
	if (m_Tokens.IsValidIndex(m_TokenIndex))
	{
		return m_Tokens[m_TokenIndex];
	}

	return FToken::EndOfSource;
}

const FToken& FParser::PeekNext() const
{
	if (m_Tokens.IsValidIndex(m_TokenIndex + 1))
	{
		return m_Tokens[m_TokenIndex + 1];
	}

	return FToken::EndOfSource;
}

const FToken& FParser::PeekPrevious() const
{
	if (m_Tokens.IsValidIndex(m_TokenIndex - 1))
	{
		return m_Tokens[m_TokenIndex - 1];
	}

	return FToken::EndOfSource;
}

void FParser::RecordError(FSourceLocation location, FString&& message)
{
	(void)m_Errors.Emplace(MoveTemp(location), MoveTemp(message));
}

void FParser::RecordError(FSourceLocation location, FStringView message)
{
	(void)m_Errors.Emplace(MoveTemp(location), MoveTemp(message));
}
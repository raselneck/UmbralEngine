#include "Lox/Parser.h"

void FLoxParser::ParseTokens(const TSpan<const FLoxToken> tokens)
{
	m_Errors.Reset();
	m_Statements.Reset();
	m_Tokens = tokens;
	m_TokenIndex = 0;

	if (m_Tokens.IsEmpty())
	{
		return;
	}

	while (IsAtEnd() == false)
	{
		TUniquePtr<FLoxExpression> expression = ParseExpression();
		if (expression.IsNull())
		{
			continue;
		}

		TUniquePtr<FLoxExpressionStatement> statement = MakeUnique<FLoxExpressionStatement>(MoveTemp(expression));
		(void)m_Statements.Emplace(MoveTemp(statement));
	}
}

const FLoxToken& FLoxParser::AdvanceToken()
{
	if (IsAtEnd() == false)
	{
		++m_TokenIndex;
	}

	return PeekPrevious();
}

bool FLoxParser::Check(const ELoxTokenType type) const
{
	if (IsAtEnd())
	{
		return false;
	}

	return Peek().Type == type;
}

bool FLoxParser::Consume(const ELoxTokenType tokenType, const FStringView message)
{
	if (Check(tokenType))
	{
		AdvanceToken();
		return true;
	}

	if (Peek().Type == ELoxTokenType::EOF)
	{
		RecordError(PeekPrevious().SourceLocation, message);
	}
	else
	{
		RecordError(Peek().SourceLocation, message);
	}

	return false;
}

bool FLoxParser::IsAtEnd() const
{
	return m_TokenIndex >= m_Tokens.Num();
}

TUniquePtr<FLoxExpression> FLoxParser::ParseExpression()
{
	TUniquePtr<FLoxExpression> expression = ParseTernaryExpression();

	if (expression.IsNull())
	{
		Synchronize();
	}

	return expression;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseTernaryExpression()
{
	TUniquePtr<FLoxExpression> expr = ParseComparisonExpression();
	if (expr.IsNull())
	{
		return nullptr;
	}

	if (Match(ELoxTokenType::QuestionMark))
	{
		TUniquePtr<FLoxExpression> trueExpr = ParseTernaryExpression();
		if (trueExpr.IsNull())
		{
			return nullptr;
		}

		if (Consume(ELoxTokenType::Colon, "Expected \":\" in ternary expression"_sv) == false)
		{
			return nullptr;
		}

		TUniquePtr<FLoxExpression> falseExpr = ParseTernaryExpression();
		if (falseExpr.IsNull())
		{
			return nullptr;
		}

		return MakeUnique<FLoxTernaryExpression>(MoveTemp(expr), MoveTemp(trueExpr), MoveTemp(falseExpr));
	}

	return expr;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseEqualityExpression()
{
	TUniquePtr<FLoxExpression> expr = ParseComparisonExpression();
	if (expr.IsNull())
	{
		return nullptr;
	}

	while (Match(ELoxTokenType::BangEqual, ELoxTokenType::EqualEqual))
	{
		FLoxToken op = PeekPrevious();
		TUniquePtr<FLoxExpression> right = ParseComparisonExpression();
		if (right.IsNull())
		{
			return nullptr;
		}

		expr = MakeUnique<FLoxBinaryExpression>(MoveTemp(expr), MoveTemp(op), MoveTemp(right));
	}

	return expr;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseComparisonExpression()
{
	TUniquePtr<FLoxExpression> expr = ParseTermExpression();
	if (expr.IsNull())
	{
		return nullptr;
	}

	while (Match(ELoxTokenType::Greater, ELoxTokenType::GreaterEqual, ELoxTokenType::Less, ELoxTokenType::LessEqual))
	{
		FLoxToken op = PeekPrevious();
		TUniquePtr<FLoxExpression> right = ParseTermExpression();
		if (right.IsNull())
		{
			return nullptr;
		}

		expr = MakeUnique<FLoxBinaryExpression>(MoveTemp(expr), MoveTemp(op), MoveTemp(right));
	}

	return expr;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseTermExpression()
{
	TUniquePtr<FLoxExpression> expr = ParseFactorExpression();
	if (expr.IsNull())
	{
		return nullptr;
	}

	while (Match(ELoxTokenType::Minus, ELoxTokenType::Plus))
	{
		FLoxToken op = PeekPrevious();
		TUniquePtr<FLoxExpression> right = ParseFactorExpression();
		if (right.IsNull())
		{
			return nullptr;
		}

		expr = MakeUnique<FLoxBinaryExpression>(MoveTemp(expr), MoveTemp(op), MoveTemp(right));
	}

	return expr;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseFactorExpression()
{
	TUniquePtr<FLoxExpression> expr = ParseUnaryExpression();
	if (expr.IsNull())
	{
		return nullptr;
	}

	while (Match(ELoxTokenType::Asterisk, ELoxTokenType::Slash, ELoxTokenType::Caret))
	{
		FLoxToken op = PeekPrevious();
		TUniquePtr<FLoxExpression> right = ParseUnaryExpression();
		if (right.IsNull())
		{
			return nullptr;
		}

		expr = MakeUnique<FLoxBinaryExpression>(MoveTemp(expr), MoveTemp(op), MoveTemp(right));
	}

	return expr;
}

TUniquePtr<FLoxExpression> FLoxParser::ParseUnaryExpression()
{
	if (Match(ELoxTokenType::Bang, ELoxTokenType::Minus))
	{
		FLoxToken op = PeekPrevious();
		TUniquePtr<FLoxExpression> right = ParseUnaryExpression();
		if (right.IsNull())
		{
			return nullptr;
		}

		return MakeUnique<FLoxUnaryExpression>(MoveTemp(op), MoveTemp(right));
	}

	return ParsePrimaryExpression();
}

TUniquePtr<FLoxExpression> FLoxParser::ParsePrimaryExpression()
{
	if (Match(ELoxTokenType::False, ELoxTokenType::True, ELoxTokenType::Null, ELoxTokenType::Integer, ELoxTokenType::Float, ELoxTokenType::String))
	{
		FLoxToken literal = PeekPrevious();
		return MakeUnique<FLoxLiteralExpression>(MoveTemp(literal));
	}

	if (Match(ELoxTokenType::LeftParen))
	{
		TUniquePtr<FLoxExpression> expr = ParseExpression();
		if (Consume(ELoxTokenType::RightParen, "Expected \")\" after expression"_sv))
		{
			return MakeUnique<FLoxGroupedExpression>(MoveTemp(expr));
		}
		return nullptr;
	}

	if (PeekPrevious().Type == ELoxTokenType::EOF)
	{
		RecordError(Peek().SourceLocation, "Expected an expression or operator before \"{}\""_sv, Peek().Text);
	}
	else
	{
		RecordError(PeekPrevious().SourceLocation, "Expected an expression or operator after \"{}\""_sv, PeekPrevious().Text);
	}

	return nullptr;
}

const FLoxToken& FLoxParser::Peek() const
{
	if (IsAtEnd())
	{
		return FLoxToken::EOF;
	}

	return m_Tokens[m_TokenIndex];
}

const FLoxToken& FLoxParser::PeekNext() const
{
	if (m_TokenIndex + 1 >= m_Tokens.Num())
	{
		return FLoxToken::EOF;
	}

	return m_Tokens[m_TokenIndex + 1];
}

const FLoxToken& FLoxParser::PeekPrevious() const
{
	if (m_Tokens.IsValidIndex(m_TokenIndex - 1) == false)
	{
		return FLoxToken::EOF;
	}

	return m_Tokens[m_TokenIndex - 1];
}

void FLoxParser::RecordError(FLoxSourceLocation location, FString&& message)
{
	(void)m_Errors.Emplace(MoveTemp(location), MoveTemp(message));
}

void FLoxParser::RecordError(FLoxSourceLocation location, FStringView message)
{
	(void)m_Errors.Emplace(MoveTemp(location), MoveTemp(message));
}

void FLoxParser::Synchronize()
{
	AdvanceToken();
	while (IsAtEnd() == false)
	{
		if (PeekPrevious().Type == ELoxTokenType::Semicolon)
		{
			return;
		}

		switch (Peek().Type) {
		case ELoxTokenType::Class:
		case ELoxTokenType::For:
		case ELoxTokenType::Function:
		case ELoxTokenType::If:
		case ELoxTokenType::Return:
		case ELoxTokenType::Let:
		case ELoxTokenType::Const:
		case ELoxTokenType::While:
			return;

		default:
			AdvanceToken();
			break;
		}
	}
}
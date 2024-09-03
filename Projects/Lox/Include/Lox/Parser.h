#pragma once

#include "Containers/Array.h"
#include "Lox/Error.h"
#include "Lox/Statements.h"
#include "Memory/UniquePtr.h"
#include "Templates/VariadicTraits.h"

/**
 * @brief Defines a way to parse Lox tokens into Lox expressions.
 */
class FLoxParser
{
public:

	/**
	 * @brief Gets the collection of errors encountered by this parser.
	 *
	 * @return The collection of errors encountered by this parser.
	 */
	[[nodiscard]] TSpan<const FLoxError> GetErrors() const
	{
		return m_Errors.AsSpan();
	}

	/**
	 * @brief Gets the statements parsed by this parser.
	 *
	 * @return The parsed statements.
	 */
	[[nodiscard]] TSpan<const TUniquePtr<FLoxStatement>> GetStatements() const
	{
		return m_Statements.AsSpan();
	}

	/**
	 * @brief Checks to see if this parser encountered any errors.
	 *
	 * @return True if this parser encountered any errors, otherwise false.
	 */
	[[nodiscard]] bool HasErrors() const
	{
		return m_Errors.Num() > 0;
	}

	/**
	 * @brief Parses the given token collection into statements.
	 *
	 * @param tokens The token collections.
	 */
	void ParseTokens(TSpan<const FLoxToken> tokens);

protected:

	/**
	 * @brief Advances to the next token.
	 */
	[[maybe_unused]] const FLoxToken& AdvanceToken();

	/**
	 * @brief Checks to see if the next token matches the given token type.
	 *
	 * @param type The token type.
	 * @return True if the next token has the given type, otherwise false.
	 */
	[[nodiscard]] bool Check(ELoxTokenType type) const;

	/**
	 * @brief Attempts to consume the given token type. If it was not found, an error message will be recorded.
	 *
	 * After the error message is recorded, the parser will attempt to synchronize to a valid parsing state.
	 *
	 * @param tokenType The token type to match.
	 * @param message The error message.
	 * @return The consumed token.
	 */
	[[nodiscard]] bool Consume(ELoxTokenType tokenType, FStringView message);

	/**
	 * @brief Checks to see if this parser is at the end of the token collection.
	 *
	 * @return True if this parser is at the end of the token collection, otherwise false.
	 */
	[[nodiscard]] bool IsAtEnd() const;

	/**
	 * @brief Checks to see if the next token matches any of the given token types.
	 *
	 * @tparam TokenTypes
	 * @param tokenTypes The token types.
	 * @return True if Check passes for any of \p tokenTypes, otherwise false.
	 */
	template<typename... TokenTypes>
	[[nodiscard]] bool Match(TokenTypes... tokenTypes)
	{
		bool foundMatch = false;
		TVariadicForEach<TokenTypes...>::Visit([&, this](const ELoxTokenType tokenType)
		{
			if (Check(tokenType))
			{
				foundMatch = true;
				AdvanceToken();
				return EIterationDecision::Break;
			}

			return EIterationDecision::Continue;
		}, Forward<TokenTypes>(tokenTypes)...);

		return foundMatch;
	}

	/**
	 * @brief Parses a Lox expression.
	 *
	 * @return The expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseExpression();

	/**
	 * @brief Parses a ternary expression.
	 *
	 * @return The ternary expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseTernaryExpression();

	/**
	 * @brief Parses an equality expression.
	 *
	 * @return The equality expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseEqualityExpression();

	/**
	 * @brief Parses a comparison expression.
	 *
	 * @return The comparison expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseComparisonExpression();

	/**
	 * @brief Parses a term expression.
	 *
	 * @return The term expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseTermExpression();

	/**
	 * @brief Parses a factor expression.
	 *
	 * @return The factor expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseFactorExpression();

	/**
	 * @brief Parses a unary expression.
	 *
	 * @return The unary expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParseUnaryExpression();

	/**
	 * @brief Parses a primary expression.
	 *
	 * @return The primary expression.
	 */
	[[nodiscard]] TUniquePtr<FLoxExpression> ParsePrimaryExpression();

	/**
	 * @brief Peeks at the current token.
	 *
	 * @return The token.
	 */
	[[nodiscard]] const FLoxToken& Peek() const;

	/**
	 * @brief Peeks at the next token.
	 *
	 * @return The next token.
	 */
	[[nodiscard]] const FLoxToken& PeekNext() const;

	/**
	 * @brief Peeks at the previous token.
	 *
	 * @return The previous token.
	 */
	[[nodiscard]] const FLoxToken& PeekPrevious() const;

	/**
	 * @brief Records an error.
	 *
	 * @tparam ArgTypes The types of the message format arguments.
	 * @param location The error location.
	 * @param message The message format.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	void RecordError(FLoxSourceLocation location, const FStringView message, ArgTypes&&... args)
	{
		RecordError(MoveTemp(location), FString::Format(message, Forward<ArgTypes>(args)...));
	}

	/**
	 * @brief Records an error.
	 *
	 * @param location The error location.
	 * @param message The error message.
	 */
	void RecordError(FLoxSourceLocation location, FString&& message);

	/**
	 * @brief Records an error.
	 *
	 * @param location The error location.
	 * @param message The error message.
	 */
	void RecordError(FLoxSourceLocation location, FStringView message);

	/**
	 * @brief Attempts to synchronize this parser to a valid state.
	 */
	void Synchronize();

private:

	TArray<FLoxError> m_Errors;
	TArray<TUniquePtr<FLoxStatement>> m_Statements;
	TSpan<const FLoxToken> m_Tokens;
	int32 m_TokenIndex = 0;
};
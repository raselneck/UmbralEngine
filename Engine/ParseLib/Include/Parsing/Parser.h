#pragma once

#include "Containers/Array.h"
#include "Memory/UniquePtr.h"
#include "Parsing/ParseError.h"
#include "Parsing/Token.h"
#include "Templates/VariadicTraits.h"

/**
 * @brief Defines a helper base for parsing tokens.
 */
class FParser
{
public:

	/**
	 * @brief Destroys this parser.
	 */
	virtual ~FParser() = default;

	/**
	 * @brief Gets the collection of errors encountered by this parser.
	 *
	 * @return The collection of errors encountered by this parser.
	 */
	[[nodiscard]] TSpan<const FParseError> GetErrors() const
	{
		return m_Errors.AsSpan();
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
	 * @brief Parses the given token collection.
	 *
	 * @param tokens The token collection.
	 */
	void ParseTokens(TSpan<const FToken> tokens);

protected:

	/**
	 * @brief Advances to the next token.
	 */
	[[maybe_unused]] const FToken& AdvanceToken();

	/**
	 * @brief Checks to see if the next token matches the given token type.
	 *
	 * @param tokenType The token type.
	 * @return True if the next token has the given type, otherwise false.
	 */
	[[nodiscard]] bool Check(ETokenType tokenType) const;

	/**
	 * @brief Attempts to consume the given token type. If it was not found, an error message will be recorded.
	 *
	 * After the error message is recorded, the parser will attempt to synchronize to a valid parsing state.
	 *
	 * @param tokenType The token type to match.
	 * @param message The error message.
	 * @return The consumed token.
	 */
	[[nodiscard]] bool Consume(ETokenType tokenType, FStringView message);

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
		TVariadicForEach<TokenTypes...>::Visit([&, this](const ETokenType tokenType)
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
	 * @brief Called when parsing is beginning.
	 */
	virtual void OnParseBegin() { }

	/**
	 * @brief Called when parsing has ended.
	 */
	virtual void OnParseEnd() { }

	/**
	 * @brief Called to parse the next item from the current token.
	 *
	 * @return Whether or not to continue parsing.
	 */
	virtual EIterationDecision ParseFromCurrentToken() { return EIterationDecision::Break; }

	/**
	 * @brief Peeks at the current token.
	 *
	 * @return The token.
	 */
	[[nodiscard]] const FToken& Peek() const;

	/**
	 * @brief Peeks at the next token.
	 *
	 * @return The next token.
	 */
	[[nodiscard]] const FToken& PeekNext() const;

	/**
	 * @brief Peeks at the previous token.
	 *
	 * @return The previous token.
	 */
	[[nodiscard]] const FToken& PeekPrevious() const;

	/**
	 * @brief Records an error.
	 *
	 * @tparam ArgTypes The types of the message format arguments.
	 * @param location The error location.
	 * @param message The message format.
	 * @param args The format arguments.
	 */
	template<typename... ArgTypes>
	void RecordError(FSourceLocation location, const FStringView message, ArgTypes&&... args)
	{
		RecordError(MoveTemp(location), FString::Format(message, Forward<ArgTypes>(args)...));
	}

	/**
	 * @brief Records an error.
	 *
	 * @param location The error location.
	 * @param message The error message.
	 */
	void RecordError(FSourceLocation location, FString&& message);

	/**
	 * @brief Records an error.
	 *
	 * @param location The error location.
	 * @param message The error message.
	 */
	void RecordError(FSourceLocation location, FStringView message);

private:

	TArray<FParseError> m_Errors;
	TSpan<const FToken> m_Tokens;
	int32 m_TokenIndex = 0;
};
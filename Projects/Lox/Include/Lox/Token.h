#pragma once

#include "Lox/SourceLocation.h"
#include "Lox/TokenType.h"
#include "Lox/Value.h"

/**
 * @brief Defines a Lox token parsed from source text.
 */
class FLoxToken final
{
public:

	/**
	 * @brief The end-of-file token.
	 */
	static const FLoxToken EOF;

	/** @brief This token's literal value. */
	FLoxValue Value;

	/** @brief This token's text. */
	FString Text;

	/** @brief This token's source location. */
	FLoxSourceLocation SourceLocation;

	/** @brief This token's type. */
	ELoxTokenType Type = ELoxTokenType::Identifier;
};
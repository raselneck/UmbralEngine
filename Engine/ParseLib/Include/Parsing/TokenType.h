#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief An enumeration of possible token types.
 */
enum class ETokenType : int32
{
	Identifier,
	String,
	Integer,
	Float,
	LeftParen,    // (
	RightParent,  // )
	LeftBracket,  // [
	RightBracket, // ]
	LeftBrace,    // {
	RightBrace,   // }
	LessThan,     // <
	GreaterThan,  // >
	Comma,        // ,
	Period        // .
};
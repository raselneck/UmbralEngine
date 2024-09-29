#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief An enumeration of possible token types.
 */
enum class ETokenType : int32
{
	EndOfSource,
	Identifier,   // hello_world
	String,       // "hello world"
	Number,       // 12345
	SingleQuote,  // '
	DoubleQuote,  // "
	LeftParen,    // (
	RightParent,  // )
	LeftBracket,  // [
	RightBracket, // ]
	LeftBrace,    // {
	RightBrace,   // }
	LessThan,     // <
	GreaterThan,  // >
	Underscore,   // _
	Period,       // .
	Comma,        // ,
	Colon,        // :
	Semicolon,    // ;
	Plus,         // +
	Minus,        // -
	Asterisk,     // *
	Slash,        // /
	Equal,        // =
	Caret,        // ^
	Exclamation,  // !
	Question,     // ?
	Ampersand,    // &
	Percent,      // %
	Octothorpe,   // # aka number sign aka pound sign aka hash mark
	Tilde,        // ~
	Backtick,     // `
};
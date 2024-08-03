#pragma once

#include "Engine/IntTypes.h"
#include "Containers/StringView.h"

#ifdef EOF
#	undef EOF
#endif

/**
 * @brief An enumeration of possible Lox token types.
 */
enum class ELoxTokenType : uint8
{
	Identifier,
	Float,
	Integer,
	String,

	True,
	False,
	Null,

	And,
	Or,
	Not,
	If,
	Else,
	Let,
	Const,
	While,
	For,
	Switch,
	Case,
	Function,
	Return,
	Class,
	Interface,
	Super,

	Plus,
	PlusEqual,
	Minus,
	MinusEqual,
	Asterisk,
	AsteriskEqual,
	Slash,
	SlashEqual,
	Caret,
	Equal,
	EqualEqual,
	Bang,
	BangEqual,
	Greater,
	GreaterEqual,
	Less,
	LessEqual,

	LeftParen,
	RightParen,
	LeftBracket,
	RightBracket,
	LeftBrace,
	RightBrace,

	Dot,
	Comma,
	Colon,
	Semicolon,
	QuestionMark,

	Is,
	IntTerm,
	FloatTerm,
	StringTerm,
	BoolTerm,

	EOF
};

/**
 * @brief Gets the text for a token type.
 *
 * @param tokenType The token type.
 * @return The text representing the token type.
 */
[[nodiscard]] constexpr FStringView GetTokenTypeText(const ELoxTokenType tokenType)
{
	switch (tokenType)
	{
	case ELoxTokenType::True:           return "true"_sv;
	case ELoxTokenType::False:          return "false"_sv;
	case ELoxTokenType::Null:           return "null"_sv;

	case ELoxTokenType::And:            return "and"_sv;
	case ELoxTokenType::Or:             return "or"_sv;
	case ELoxTokenType::Not:            return "not"_sv;
	case ELoxTokenType::If:             return "if"_sv;
	case ELoxTokenType::Else:           return "else"_sv;
	case ELoxTokenType::Let:            return "let"_sv;
	case ELoxTokenType::Const:          return "const"_sv;
	case ELoxTokenType::While:          return "while"_sv;
	case ELoxTokenType::For:            return "for"_sv;
	case ELoxTokenType::Switch:         return "switch"_sv;
	case ELoxTokenType::Case:           return "case"_sv;
	case ELoxTokenType::Function:       return "function"_sv;
	case ELoxTokenType::Return:         return "return"_sv;
	case ELoxTokenType::Class:          return "class"_sv;
	case ELoxTokenType::Interface:      return "interface"_sv;
	case ELoxTokenType::Super:          return "super"_sv;

	case ELoxTokenType::Plus:           return "+"_sv;
	case ELoxTokenType::PlusEqual:      return "+="_sv;
	case ELoxTokenType::Minus:          return "-"_sv;
	case ELoxTokenType::MinusEqual:     return "-="_sv;
	case ELoxTokenType::Asterisk:       return "*"_sv;
	case ELoxTokenType::AsteriskEqual:  return "*="_sv;
	case ELoxTokenType::Slash:          return "/"_sv;
	case ELoxTokenType::SlashEqual:     return "/="_sv;
	case ELoxTokenType::Caret:          return "^"_sv;
	case ELoxTokenType::Equal:          return "="_sv;
	case ELoxTokenType::EqualEqual:     return "=="_sv;
	case ELoxTokenType::Bang:           return "!"_sv;
	case ELoxTokenType::BangEqual:      return "!="_sv;
	case ELoxTokenType::Greater:        return ">"_sv;
	case ELoxTokenType::GreaterEqual:   return ">="_sv;
	case ELoxTokenType::Less:           return "<"_sv;
	case ELoxTokenType::LessEqual:      return "<="_sv;

	case ELoxTokenType::LeftParen:      return "("_sv;
	case ELoxTokenType::RightParen:     return ")"_sv;
	case ELoxTokenType::LeftBracket:    return "["_sv;
	case ELoxTokenType::RightBracket:   return "]"_sv;
	case ELoxTokenType::LeftBrace:      return "{"_sv;
	case ELoxTokenType::RightBrace:     return "}"_sv;

	case ELoxTokenType::Dot:            return "."_sv;
	case ELoxTokenType::Comma:          return ","_sv;
	case ELoxTokenType::Colon:          return ":"_sv;
	case ELoxTokenType::Semicolon:      return ";"_sv;
	case ELoxTokenType::QuestionMark:   return "?"_sv;

	case ELoxTokenType::Is:             return "is"_sv;
	case ELoxTokenType::IntTerm:        return "int"_sv;
	case ELoxTokenType::FloatTerm:      return "float"_sv;
	case ELoxTokenType::StringTerm:     return "string"_sv;
	case ELoxTokenType::BoolTerm:       return "bool"_sv;

	default:                            return nullptr;
	}
}
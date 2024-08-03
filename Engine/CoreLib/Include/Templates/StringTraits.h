#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IsChar.h"
#include "Templates/IsSame.h"

template<typename T> struct TIsStringLiteral : FFalseType { };
template<typename T, usize N> struct TIsStringLiteral<T[N]> : TIsChar<T> { };
template<typename T, usize N> struct TIsStringLiteral<T(&)[N]> : TIsChar<T> { };
template<typename T, usize N> struct TIsStringLiteral<const T[N]> : TIsChar<T> { };
template<typename T, usize N> struct TIsStringLiteral<const T(&)[N]> : TIsChar<T> { };

#define MAKE_STRING_LITERAL_TYPE_TRAIT(Name, CharType) \
	template<typename T> struct Name : FFalseType { }; \
	template<typename T, usize N> struct Name<T[N]> : TIsSame<T, CharType> { }; \
	template<typename T, usize N> struct Name<T(&)[N]> : TIsSame<T, CharType> { }; \
	template<typename T, usize N> struct Name<const T[N]> : TIsSame<T, CharType> { }; \
	template<typename T, usize N> struct Name<const T(&)[N]> : TIsSame<T, CharType> { }

MAKE_STRING_LITERAL_TYPE_TRAIT(TIsAnsiStringLiteral, char);
MAKE_STRING_LITERAL_TYPE_TRAIT(TIsUtf8StringLiteral, char8_t);
MAKE_STRING_LITERAL_TYPE_TRAIT(TIsUtf16StringLiteral, char16_t);
MAKE_STRING_LITERAL_TYPE_TRAIT(TIsUtf32StringLiteral, char32_t);

#undef MAKE_STRING_LITERAL_TYPE_TRAIT

/**
 * @brief Defines a way to get traits about string literals.
 *
 * @tparam T The type of each character in the string.
 */
template<typename T>
	requires TIsChar<T>::Value
struct TStringTraits
{
	using CharType = T;
	using SizeType = int32;

	/**
	 * @brief Gets the null-terminated character count of the given raw string literal.
	 *
	 * @param chars The string literal.
	 * @return The null-terminated character count of \p chars.
	 */
	static constexpr SizeType GetNullTerminatedCharCount(const CharType* chars)
	{
		SizeType numChars = 0;
		if (chars == nullptr || *chars == 0)
		{
			return numChars;
		}

		while (*chars)
		{
			++numChars;
			++chars;
		}

		return numChars;
	}
};
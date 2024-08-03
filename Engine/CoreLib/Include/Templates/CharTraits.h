#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IsChar.h"

/**
 * @brief Defines traits and functions for characters.
 */
template<typename T>
	requires TIsChar<T>::Value
class TCharTraits
{
public:

	using CharType = T;
	using SizeType = int32;

	/** @brief The null character. */
	static constexpr CharType NullChar = static_cast<CharType>('\0');

	/**
	 * @brief Gets the null-terminated length of the given raw string literal.
	 * 
	 * @param chars The raw string literal.
	 * @return The null-terminated length of \p chars.
	 */
	static constexpr SizeType GetNullTerminatedLength(const CharType* chars)
	{
		if (chars == nullptr)
		{
			return 0;
		}

		SizeType length = 0;
		while (*chars != NullChar)
		{
			++chars;
			++length;
		}

		return length;
	}
};
#pragma once

#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "Misc/Unicode.h"

namespace Utf8
{
	// See https://en.wikipedia.org/wiki/UTF-8

	struct FCharWidthResult
	{
		uint32 CharWidth = 0;
		uint32 CharInitialMask = 0;
		bool bValid = false;
	};

	struct FDecodeResult
	{
		uint32 CodePoint = 0;
		int32 CodeWidth = 0;
		bool bValid = false;
	};

	struct FEncodeResult
	{
		TStaticArray<uint8, 6> Chars { 0, 0, 0, 0, 0, 0 };
		int32 CharCount = 0;
		bool bValid = false;

		inline TSpan<const char> GetCharSpan() const
		{
			return TSpan<const char> { reinterpret_cast<const char*>(Chars.GetData()), CharCount };
		}

		inline void WriteComponent(const uint8 component)
		{
			UM_ASSERT(CharCount < Chars.Num(), "Attempting to append too many UTF-8 character bytes");

			Chars[CharCount++] = component;
		}
	};

	/**
	 * @brief Determines the width of a UTF-8 encoded Unicode code point.
	 *
	 * @param character The character at the beginning of the Unicode code point.
	 * @return The result from getting the Unicode character width.
	 */
	FCharWidthResult GetCharWidth(uint8 character);

	/**
	 * @brief Decodes a Unicode character from the given stream of UTF-8 characters.
	 *
	 * @param utf8Chars The stream of characters to read from.
	 * @return The decode result.
	 */
	FDecodeResult DecodeChar(const char* utf8Chars);

	/**
	 * @brief Encodes a Unicode code point to UTF-8.
	 *
	 * @param codePoint The code point to encode.
	 * @return The encode result.
	 */
	FEncodeResult EncodeChar(uint32 codePoint);
}

namespace Utf16
{
	// See https://en.wikipedia.org/wiki/UTF-16

	struct FCharWidthResult
	{
		uint32 CharWidth = 0;
		uint32 CharInitialMask = 0;
		bool bValid = false;
	};

	struct FDecodeResult
	{
		uint32 CodePoint = 0;
		int32 CodeWidth = 0;
		bool bValid = false;
	};

	struct FEncodeResult
	{
		TStaticArray<uint16, 2> Chars { 0, 0 };
		int32 CharCount = 0;
		bool bValid = false;

		inline TSpan<const char16_t> GetCharSpan() const
		{
			return TSpan<const char16_t> { reinterpret_cast<const char16_t*>(Chars.GetData()), CharCount };
		}

		inline void WriteComponent(const uint16 component)
		{
			UM_ASSERT(CharCount < Chars.Num(), "Attempting to append too many UTF-16 character bytes");

			Chars[CharCount++] = component;
		}
	};

	/**
	 * @brief Determines the width of a UTF-16 encoded Unicode code point.
	 *
	 * @param Char The char at the beginning of the Unicode code point.
	 * @param OutCharWidth The width of the UTF-16 encoded Unicode code point.
	 * @param Result.bValid Whether or not the encoded code point is valid.
	 */
	FCharWidthResult GetCharWidth(uint16 character);

	/**
	 * @brief Decodes a Unicode character from the given stream of UTF-8 characters.
	 *
	 * @param Chars The stream of characters to read from.
	 * @return The decode result.
	 */
	FDecodeResult DecodeChar(const char16_t* utf16Chars);

	/**
	 * @brief Encodes a Unicode code point to UTF-8.
	 *
	 * @param codePoint The code point to encode.
	 * @return The encode result.
	 */
	FEncodeResult EncodeChar(uint32 codePoint);
}
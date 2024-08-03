#include "Engine/Logging.h"
#include "Misc/Unicode.h"
#include "Misc/InternalUnicode.h"

// https://github.com/ww898/utf-cpp/tree/master/include/ww898

static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "Platform wide character is neither 2 nor 4 bytes wide");
static_assert(sizeof(char) == sizeof(char8_t), "Platform ANSI char and UTF-8 char are not compatible");
static_assert(sizeof(uint8) == sizeof(char8_t), "Platform UTF-8 char and uint8 are not compatible");
static_assert(sizeof(uint16) == sizeof(char16_t), "Platform UTF-16 char and uint16 are not compatible");

namespace Utf8
{
	FCharWidthResult GetCharWidth(const uint8 character)
	{
		FCharWidthResult result;

		if (character < 0b10000000)
		{
			result.CharWidth = 1;
			result.CharInitialMask = 0b01111111;
			result.bValid = true;
		}
		else if (character < 0b11000000)
		{
			// TODO This used to be "found %u (%02X)"
			UM_LOG(Error, "Expected valid UTF-8 code point marker; found {} instead", static_cast<uint32>(character));
		}
		else if (character < 0b11100000)
		{
			result.CharWidth = 2;
			result.CharInitialMask = 0b00011111;
			result.bValid = true;
		}
		else if (character < 0b11110000)
		{
			result.CharWidth = 3;
			result.CharInitialMask = 0b00001111;
			result.bValid = true;
		}
		else if (character < 0b11111000)
		{
			result.CharWidth = 4;
			result.CharInitialMask = 0b00000111;
			result.bValid = true;
		}
		else if (character < 0b11111100)
		{
			result.CharWidth = 5;
			result.CharInitialMask = 0b00000011;
			result.bValid = true;
		}
		else if (character < 0b11111110)
		{
			result.CharWidth = 6;
			result.CharInitialMask = 0b00000001;
			result.bValid = true;
		}
		else
		{
			// TODO This used to be "found %02X instead"
			UM_LOG(Error, "Expected valid UTF-8 code point marker; found {} instead", static_cast<uint32>(character));
		}

		return result;
	}

	FDecodeResult DecodeChar(const char* utf8Chars)
	{
		const FCharWidthResult widthResult = GetCharWidth(*utf8Chars);

		FDecodeResult result;
		if (widthResult.bValid == false)
		{
			return result;
		}

		result.CodePoint = static_cast<uint8>(*utf8Chars) & widthResult.CharInitialMask;
		result.CodeWidth = widthResult.CharWidth;

		uint32 charWidth = widthResult.CharWidth;
		while (--charWidth)
		{
			++utf8Chars;
			if (*utf8Chars == 0)
			{
				break;
			}

			// All bytes after the first must be in the format 0b10xxxxxx
			if ((*utf8Chars & 0b11000000) != 0b10000000)
			{
				// TODO This used to be "continuation byte %c (%02X)"
				UM_LOG(Error, "Found invalid UTF-8 continuation byte {}", static_cast<uint8>(*utf8Chars), static_cast<uint32>(*utf8Chars));
				break;
			}

			result.CodePoint <<= 6;
			result.CodePoint |= static_cast<uint8>(*utf8Chars) & 0b00111111;
		}

		if (charWidth > 0)
		{
			UM_LOG(Error, "Found end of string; expected {} more UTF-8 characters", charWidth);
			return result;
		}

		result.bValid = true;
		return result;
	}

	FEncodeResult EncodeChar(const uint32 codePoint)
	{
		FEncodeResult result;
		if (codePoint >= 0x80000000)
		{
			return result;
		}

		uint32 numExtraChars = 0;
		if (codePoint < 0x80)
		{
			result.WriteComponent(static_cast<uint8>(codePoint));
		}
		else if (codePoint < 0x800)
		{
			result.WriteComponent(static_cast<uint8>(0b11000000 | (codePoint >> 6)));
			numExtraChars = 1;
		}
		else if (codePoint < 0x10000)
		{
			result.WriteComponent(static_cast<uint8>(0b11100000 | (codePoint >> 12)));
			numExtraChars = 2;
		}
		else if (codePoint < 0x200000)
		{
			result.WriteComponent(static_cast<uint8>(0b11111000 | (codePoint >> 18)));
			numExtraChars = 3;
		}
		else if (codePoint < 0x4000000)
		{
			result.WriteComponent(static_cast<uint8>(0b11111100 | (codePoint >> 24)));
			numExtraChars = 4;
		}
		else if (codePoint < 0x80000000)
		{
			result.WriteComponent(static_cast<uint8>(0b11111100 | (codePoint >> 30)));
			numExtraChars = 5;
		}

		while (numExtraChars)
		{
			const uint32 shiftedCodePoint = codePoint >> (6 * (numExtraChars - 1));
			result.WriteComponent(static_cast<uint8>(0b10000000 | (shiftedCodePoint & 0b00111111)));

			--numExtraChars;
		}

		result.bValid = true;
		return result;
	}
}

namespace Utf16
{
	FCharWidthResult GetCharWidth(const uint16 character)
	{
		FCharWidthResult result;

		// 0x0000 - 0xD7FF
		if (character < 0xD800)
		{
			result.CharWidth = 1;
			result.bValid = true;
		}
		// 0xD800 - 0xDBFF
		else if (character < 0xDC00)
		{
			result.CharWidth = 2;
			result.bValid = true;
		}
		// 0xDC00 - 0xDFFF
		else if (character < 0xE000)
		{
			// TODO This used to be "found %04X instead"
			UM_LOG(Error, "Expected high surrogate for UTF-16 pair; found {} instead", static_cast<uint32>(character));
		}
		// 0xE000 - 0xFFFF
		else
		{
			result.CharWidth = 1;
			result.bValid = true;
		}

		return result;
	}

	FDecodeResult DecodeChar(const char16_t* utf16Chars)
	{
		FDecodeResult result;

		const char16_t firstChar = *utf16Chars++;

		// 0x0000 - 0xD7FF, 0xE000 - 0xFFFF
		if (firstChar < 0xD800 || firstChar >= 0xE000)
		{
			result.CodePoint = firstChar;
			result.CodeWidth = 1;
			result.bValid = true;
		}
		// 0xD800 - 0xDBFF (where FirstChar is the high surrogate of the pair)
		else if (firstChar < 0xDC00)
		{
			const char16_t secondChar = *utf16Chars;
			if (secondChar >> 10 != 0b00110111)
			{
				// TODO This used to be "found %d (%04X)"
				UM_LOG(Error, "Expected UTF-16 low surrogate; found {}", static_cast<int32>(secondChar), static_cast<uint32>(secondChar));
				return result;
			}

			result.CodePoint = (firstChar - 0xD800) * 0x400 + (secondChar - 0xDC00) + 0x10000;
			result.CodeWidth = 2;
			result.bValid = true;
		}
		// 0xDC00 - 0xDFFF
		else // if (FirstChar < 0xE000)
		{
			// TODO Apparently tons of encoders / decoders violate this rule... Should we?
			// NOTE violating this rule MAY be necessary to support WTF-16 https://news.ycombinator.com/item?id=18569741
			// TODO This used to be "found %d (%04X)"
			UM_LOG(Error, "Expected UTF-16 high surrogate; found {}", static_cast<int32>(firstChar), static_cast<uint32>(firstChar));
		}

		return result;
	}

	FEncodeResult EncodeChar(const uint32 codePoint)
	{
		FEncodeResult result;

		// 0x0000 - 0xD7FF
		if (codePoint < 0xD800)
		{
			result.WriteComponent(static_cast<uint16>(codePoint));
			result.bValid = true;
		}
		// 0xD800 - 0xDFFF
		else if (codePoint < 0xE000)
		{
			// TODO This used to be "point %d (%04X) is"
			UM_LOG(Error, "Code point {} is reserved by UTF-16", codePoint, codePoint);
		}
		// 0xE000 - 0xFFFF
		else if (codePoint < 0x10000)
		{
			result.WriteComponent(static_cast<uint16>(codePoint));
			result.bValid = true;
		}
		// 0x010000 - 0x10FFFF
		else if (codePoint < 0x110000) // [0xD800...0xDBFF] [0xDC00...0xDFFF]
		{
			const uint16 highSurrogate = static_cast<uint16>(((codePoint - 0x10000) >> 10) + 0xD800);
			const uint16 lowSurrogate = static_cast<uint16>((codePoint & 0x3FF) + 0xDC00);

			result.WriteComponent(highSurrogate);
			result.WriteComponent(lowSurrogate);
			result.bValid = true;
		}
		else
		{
			// TODO This used to be "point %d (%04X) is"
			UM_LOG(Error, "Code point {} is too large for UTF-16", codePoint, codePoint);
		}

		return result;
	}
}
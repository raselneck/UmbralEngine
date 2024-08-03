#include "Engine/Logging.h"
#include "Misc/InternalUnicode.h"
#include "Misc/Unicode.h"
#include "Templates/CharTraits.h"
#include <cstdio>

namespace Unicode
{
	FCountCodePointsResult CountCodePoints(const char* chars)
	{
		const int32 numChars = TCharTraits<char>::GetNullTerminatedLength(chars);
		return CountCodePoints(TSpan<const char> { chars, numChars });
	}

	FCountCodePointsResult CountCodePoints(TSpan<const char> charSpan)
	{
		FCountCodePointsResult result;
		if (charSpan.IsEmpty())
		{
			result.bValid = true;
			return result;
		}

		const char* chars = charSpan.GetData();
		for (int32 idx = 0; idx < charSpan.Num(); /* empty */)
		{
			const Utf8::FDecodeResult decodeResult = Utf8::DecodeChar(chars + idx);
			if (decodeResult.bValid == false)
			{
				return result;
			}

			++result.NumCodePoints;

			idx += decodeResult.CodeWidth;
		}

		result.bValid = true;
		return result;
	}

	FCountCodePointsResult CountCodePoints(const wchar_t* chars)
	{
		const int32 numChars = TCharTraits<wchar_t>::GetNullTerminatedLength(chars);
		return CountCodePoints(TSpan<const wchar_t> { chars, numChars });
	}

	FCountCodePointsResult CountCodePoints(TSpan<const wchar_t> charSpan)
	{
		if constexpr (sizeof(wchar_t) == 2)
		{
			return CountCodePoints(CastSpan<char16_t>(charSpan));
		}
		else
		{
			return CountCodePoints(CastSpan<char32_t>(charSpan));
		}
	}

	FCountCodePointsResult CountCodePoints(const char16_t* chars)
	{
		const int32 numChars = TCharTraits<char16_t>::GetNullTerminatedLength(chars);
		return CountCodePoints(TSpan<const char16_t> { chars, numChars });
	}

	FCountCodePointsResult CountCodePoints(TSpan<const char16_t> charSpan)
	{
		FCountCodePointsResult result;
		if (charSpan.IsEmpty())
		{
			result.bValid = true;
			return result;
		}

		const char16_t* chars = charSpan.GetData();
		int32 numChars = charSpan.Num();
		while (numChars > 0)
		{
			Utf16::FCharWidthResult widthResult = Utf16::GetCharWidth(*chars);
			if (widthResult.bValid == false)
			{
				return result;
			}

			chars += widthResult.CharWidth;
			numChars -= static_cast<int32>(widthResult.CharWidth);

			++result.NumCodePoints;
		}

		result.bValid = true;
		return result;
	}

	FCountCodePointsResult CountCodePoints(const char32_t* chars)
	{
		const int32 numChars = TCharTraits<char32_t>::GetNullTerminatedLength(chars);
		return CountCodePoints(TSpan<const char32_t> { chars, numChars });
	}

	FCountCodePointsResult CountCodePoints(TSpan<const char32_t> charSpan)
	{
		FCountCodePointsResult result;
		if (charSpan.IsEmpty())
		{
			result.bValid = true;
			return result;
		}

		const char32_t* chars = charSpan.GetData();
		int32 numChars = charSpan.Num();
		while (numChars > 0)
		{
			if (*chars >= 0x80000000)
			{
				// TODO This used to be "value %08X is"
				UM_LOG(Error, "UTF-32 char value {} is too large", static_cast<uint32>(*chars));
				return result;
			}

			++result.NumCodePoints;
			++chars;
			--numChars;
		}

		result.bValid = true;
		return result;
	}

	FToUtf8Result ToUtf8(TSpan<const wchar_t> charSpan)
	{
		if constexpr (sizeof(wchar_t) == 2)
		{
			return ToUtf8(CastSpan<char16_t>(charSpan));
		}
		else
		{
			return ToUtf8(CastSpan<char32_t>(charSpan));
		}
	}

	FToUtf8Result ToUtf8(TSpan<const char16_t> charSpan)
	{
		FToUtf8Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		const char16_t* chars = charSpan.GetData();
		for (int32 idx = 0; idx < charSpan.Num(); /* intentionally empty */)
		{
			const Utf16::FDecodeResult decodeResult = Utf16::DecodeChar(chars);
			if (decodeResult.bValid == false)
			{
				return result;
			}

			const Utf8::FEncodeResult encodeResult = Utf8::EncodeChar(decodeResult.CodePoint);
			if (encodeResult.bValid == false)
			{
				return result;
			}

			result.Chars.Append(encodeResult.GetCharSpan());

			chars += decodeResult.CodeWidth;
			idx += decodeResult.CodeWidth;
		}

		result.Chars.Add(TCharTraits<char>::NullChar);
		result.bValid = true;
		return result;
	}

	FToUtf8Result ToUtf8(TSpan<const char32_t> charSpan)
	{
		FToUtf8Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		for (int32 idx = 0; idx < charSpan.Num(); ++idx)
		{
			const uint32 codePoint = static_cast<uint32>(charSpan[idx]);
			const Utf8::FEncodeResult encodeResult = Utf8::EncodeChar(codePoint);
			if (encodeResult.bValid == false)
			{
				return result;
			}

			result.Chars.Append(encodeResult.GetCharSpan());
		}

		result.Chars.Add(TCharTraits<char>::NullChar);
		result.bValid = true;
		return result;
	}

	FToUtf16Result ToUtf16(TSpan<const char> charSpan)
	{
		FToUtf16Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		const char* chars = charSpan.GetData();
		for (int32 idx = 0; idx < charSpan.Num(); /* intentionally empty */)
		{
			const Utf8::FDecodeResult decodeResult = Utf8::DecodeChar(chars);
			if (decodeResult.bValid == false)
			{
				return result;
			}

			const Utf16::FEncodeResult encodeResult = Utf16::EncodeChar(decodeResult.CodePoint);
			if (encodeResult.bValid == false)
			{
				return result;
			}

			result.Chars.Append(encodeResult.GetCharSpan());

			chars += decodeResult.CodeWidth;
			idx += decodeResult.CodeWidth;
		}

		result.Chars.Add(TCharTraits<char16_t>::NullChar);
		result.bValid = true;
		return result;
	}

	FToUtf16Result ToUtf16(TSpan<const wchar_t> charSpan)
	{
		if constexpr (sizeof(wchar_t) == 2)
		{
			UM_ASSERT_NOT_REACHED_MSG("ToUtf16 for wide characters not yet implemented");
		}
		else
		{
			return ToUtf16(CastSpan<char32_t>(charSpan));
		}
	}

	FToUtf16Result ToUtf16(TSpan<const char32_t> charSpan)
	{
		FToUtf16Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		for (int32 idx = 0; idx < charSpan.Num(); ++idx)
		{
			const Utf16::FEncodeResult encodeResult = Utf16::EncodeChar(charSpan[idx]);
			if (encodeResult.bValid == false)
			{
				return result;
			}

			result.Chars.Append(encodeResult.GetCharSpan());
		}

		result.Chars.Add(TCharTraits<char16_t>::NullChar);
		result.bValid = true;
		return result;
	}

	FToUtf32Result ToUtf32(TSpan<const char> charSpan)
	{
		FToUtf32Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		const char* chars = charSpan.GetData();
		for (int32 idx = 0; idx < charSpan.Num(); /* intentionally empty */)
		{
			const Utf8::FDecodeResult decodeResult = Utf8::DecodeChar(chars);
			if (decodeResult.bValid == false)
			{
				return result;
			}

			result.Chars.Add(decodeResult.CodePoint);

			chars += decodeResult.CodeWidth;
			idx += decodeResult.CodeWidth;
		}

		result.Chars.Add(TCharTraits<char32_t>::NullChar);
		result.bValid = true;
		return result;
	}

	FToUtf32Result ToUtf32(TSpan<const wchar_t> charSpan)
	{
		if constexpr (sizeof(wchar_t) == 2)
		{
			return ToUtf32(CastSpan<char16_t>(charSpan));
		}
		else
		{
			UM_ASSERT_NOT_REACHED_MSG("ToUtf32 for wide characters not yet implemented");
			return {};
		}
	}

	FToUtf32Result ToUtf32(TSpan<const char16_t> charSpan)
	{
		FToUtf32Result result;
		if (charSpan.IsEmpty())
		{
			return result;
		}

		const char16_t* chars = charSpan.GetData();
		for (int32 Idx = 0; Idx < charSpan.Num(); /* intentionally empty */)
		{
			const Utf16::FDecodeResult decodeChar = Utf16::DecodeChar(chars);
			if (decodeChar.bValid == false)
			{
				return result;
			}

			result.Chars.Add(decodeChar.CodePoint);

			chars += decodeChar.CodeWidth;
			Idx += decodeChar.CodeWidth;
		}

		result.Chars.Add(TCharTraits<char32_t>::NullChar);
		result.bValid = true;
		return result;
	}
}
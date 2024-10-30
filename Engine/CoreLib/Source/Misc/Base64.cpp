#include "Containers/Pair.h"
#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Misc/Base64.h"
#include "Misc/StringBuilder.h"

// https://en.wikipedia.org/wiki/Base64

namespace Base64
{
	constexpr TStaticArray<char, 64> CharacterTable
	{{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	}};

	FString Encode(TSpan<const uint8> bytes)
	{
		FStringBuilder result;

		// Every three byte octets are encoded as four base-64 sextets
		// TODO If `bytes` is big enough, this will cause an overflow to negative
		result.Reserve(bytes.Num() * 4 / 3);

		// We encode three octets / four sextets at a time
		for (int32 idx = 0; idx < bytes.Num(); idx += 3)
		{
			const char firstChar = [&]()
			{
				const int32 charIndex = static_cast<int32>(bytes[idx] & 0b11111100) >> 2;
				return CharacterTable[charIndex];
			}();
			const char secondChar = [&]()
			{
				int32 charIndex = static_cast<int32>(bytes[idx] & 0b00000011);
				if (bytes.IsValidIndex(idx + 1))
				{
					charIndex <<= 4;
					charIndex  |= static_cast<int32>(bytes[idx + 1] & 0b11110000) >> 4;
				}
				return CharacterTable[charIndex];
			}();
			const char thirdChar = [&]()
			{
				if (bytes.IsValidIndex(idx + 1) == false)
				{
					return '=';
				}

				int32 charIndex = static_cast<int32>(bytes[idx + 1] & 0b00001111);
				if (bytes.IsValidIndex(idx + 2))
				{
					charIndex <<= 2;
					charIndex  |= static_cast<int32>(bytes[idx + 2] & 0b11000000) >> 6;
				}
				return CharacterTable[charIndex];
			}();
			const char fourthChar = [&]()
			{
				if (bytes.IsValidIndex(idx + 2) == false)
				{
					return '=';
				}

				const int32 charIndex = static_cast<int32>(bytes[idx + 2] & 0b00111111);
				return CharacterTable[charIndex];
			}();

			result.Append(firstChar);
			result.Append(secondChar);
			result.Append(thirdChar);
			result.Append(fourthChar);
		}

		return result.ReleaseString();
	}

	TErrorOr<TArray<uint8>> Decode(const FStringView chars)
	{
		TArray<uint8> result;
		result.Reserve(chars.Length());

		// We decode four sextets / three octets at a time
		for (int32 idx = 0; idx < chars.Length(); idx += 4)
		{
			// Verify that the current sextet quad are all valid characters
			for (int32 sextetIdx = 0; sextetIdx < 4 && chars.IsValidIndex(idx + sextetIdx); ++sextetIdx)
			{
				const int32 sextetCharIndex = CharacterTable.IndexOf(chars[idx + sextetIdx]);
				if (sextetCharIndex == INDEX_NONE)
				{
					// '=' is allowed for the third and fourth sextet char
					if (sextetIdx > 1 && chars[idx + sextetIdx] == '=')
					{
						continue;
					}
					return MAKE_ERROR("Invalid base-64 character {} at index {}", chars[idx + sextetIdx], idx + sextetIdx);
				}
			}

			const char firstChar  = chars.At(idx);
			const char secondChar = chars.AtOrDefault(idx + 1);
			const char thirdChar  = chars.AtOrDefault(idx + 2, '=');
			const char fourthChar = chars.AtOrDefault(idx + 3, '=');

			const int32 firstCharIndex  = CharacterTable.IndexOf(firstChar);
			const int32 secondCharIndex = CharacterTable.IndexOf(secondChar);
			const int32 thirdCharIndex  = CharacterTable.IndexOf(thirdChar);
			const int32 fourthCharIndex = CharacterTable.IndexOf(fourthChar);

			// First byte HAS to exist because we're guaranteed at least one sextet
			result.Add([&]()
			{
				// First character in the quartet has six bits of the first byte
				uint8 firstByte = static_cast<uint8>(firstCharIndex & 0b111111);

				// Second character has two bits of the first byte
				if (secondCharIndex != INDEX_NONE)
				{
					firstByte <<= 2;
					firstByte  |= static_cast<uint8>(secondCharIndex & 0b110000) >> 4;
				}

				return firstByte;
			}());

			// The base-64 string would technically be ill-formed if there wasn't a second sextet here, but
			// for the sake of supporting non-conformant generators we'll just break here if it is absent.
			// The third character being '=' also means that the second sextet was only necessary for the
			// final two bits of the first byte
			if (secondCharIndex == INDEX_NONE || thirdChar == '=')
			{
				break;
			}

			// Second character has four bits of the second byte
			uint8 secondByte = static_cast<uint8>(secondCharIndex & 0b001111);

			// Third character has the remaining four bits
			if (thirdCharIndex != INDEX_NONE)
			{
				secondByte <<= 4;
				secondByte  |= static_cast<uint8>(thirdCharIndex & 0b111100) >> 2;
			}
			result.Add(secondByte);

			// Third byte being '=' means that we don't need to include it in any more bytes
			if (thirdCharIndex == INDEX_NONE || fourthChar == '=')
			{
				break;
			}
			// Third character has two bits of the third byte
			uint8 thirdByte = static_cast<uint8>(thirdCharIndex & 0b000011);

			// Fourth character has the remaining six bits
			if (fourthCharIndex != INDEX_NONE)
			{
				thirdByte <<= 6;
				thirdByte  |= static_cast<uint8>(fourthCharIndex & 0b111111);
			}
			result.Add(thirdByte);
		}

		return result;
	}

	bool Decode(const FStringView chars, TArray<uint8>& result)
	{
		TErrorOr<TArray<uint8>> decodeResult = Decode(chars);
		if (decodeResult.IsError())
		{
			UM_LOG(Error, "{}", decodeResult.GetError());
			return false;
		}

		result = decodeResult.ReleaseValue();
		return true;
	}

	bool Decode(const FStringView chars, FString& result)
	{
		TArray<uint8> bytes;
		if (Decode(chars, bytes))
		{
			result = FString::FromByteArray(MoveTemp(bytes));
			return true;
		}
		return false;
	}
}

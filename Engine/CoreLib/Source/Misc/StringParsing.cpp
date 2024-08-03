#include "Math/Math.h"
#include "Misc/StringParsing.h"
#include "Templates/NumericLimits.h"
#include <charconv>
#if __APPLE__
#include <cstdlib>
#endif

/**
 * @brief Checks to see if the given error code is a success error code.
 *
 * @param errorCode The error code.
 * @return True if \p errorCode is a success error code, otherwise false.
 */
static constexpr bool IsSuccessCode(const std::errc errorCode)
{
	constexpr std::errc successErrorCode {};
	return errorCode == successErrorCode;
}

#if UMBRAL_PLATFORM_IS_MAC || UMBRAL_PLATFORM_IS_IOS
template<typename T>
struct TFloatParser;

template<> struct TFloatParser<float>
{
	static float Parse(const char* str, char** str_end)
	{
		return ::strtof(str, str_end);
	}
};

template<> struct TFloatParser<double>
{
	static double Parse(const char* str, char** str_end)
	{
		return ::strtod(str, str_end);
	}
};
#endif

/**
 * @brief Attempts to parse a float from the given string.
 *
 * @param text The text to parse.
 * @param value The parsed value.
 * @param base The integer base to parse.
 * @return True if parsing was successful, otherwise false.
 */
template<typename T>
static bool TryParseInt(const FStringView text, T& value, const ENumericBase base)
{
	const auto result = std::from_chars(text.GetChars(), text.GetChars() + text.Length(), value, static_cast<int32>(base));
	return IsSuccessCode(result.ec);
}

/**
 * @brief Attempts to parse a float from the given string.
 *
 * @param text The text to parse.
 * @param value The parsed value.
 * @return True if parsing was successful, otherwise false.
 */
template<typename T>
static bool TryParseFloat(const FStringView text, T& value)
{
#if UMBRAL_PLATFORM_IS_MAC || UMBRAL_PLATFORM_IS_IOS
	// TODO: This is kinda gross :grimacing:

	constexpr int32 buffSize = 32;
	char buff[buffSize] {};

	::memcpy(buff, text.GetChars(), FMath::Min(text.Length(), buffSize - 1));

	char* buffPtr = buff;
	value = TFloatParser<T>::Parse(buff, &buffPtr);
	return text.GetChars() != buffPtr;
#else
	const auto result = std::from_chars(text.GetChars(), text.GetChars() + text.Length(), value, std::chars_format::general);
	return IsSuccessCode(result.ec);
#endif
}

int8 FStringParser::ParseInt8(const FStringView text, const ENumericBase base, const int8 defaultValue)
{
	if (TOptional<int8> result = TryParseInt8(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

int16 FStringParser::ParseInt16(const FStringView text, const ENumericBase base, const int16 defaultValue)
{
	if (TOptional<int16> result = TryParseInt16(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

int32 FStringParser::ParseInt32(const FStringView text, const ENumericBase base, const int32 defaultValue)
{
	if (TOptional<int32> result = TryParseInt32(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

int64 FStringParser::ParseInt64(const FStringView text, const ENumericBase base, const int64 defaultValue)
{
	if (TOptional<int64> result = TryParseInt64(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

uint8 FStringParser::ParseUint8(const FStringView text, const ENumericBase base, const uint8 defaultValue)
{
	if (TOptional<uint8> result = TryParseUint8(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

uint16 FStringParser::ParseUint16(const FStringView text, const ENumericBase base, const uint16 defaultValue)
{
	if (TOptional<uint16> result = TryParseUint16(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

uint32 FStringParser::ParseUint32(const FStringView text, const ENumericBase base, const uint32 defaultValue)
{
	if (TOptional<uint32> result = TryParseUint32(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

uint64 FStringParser::ParseUint64(const FStringView text, const ENumericBase base, const uint64 defaultValue)
{
	if (TOptional<uint64> result = TryParseUint64(text, base); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

float FStringParser::ParseFloat(const FStringView text, const float defaultValue)
{
	if (TOptional<float> result = TryParseFloat(text); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

double FStringParser::ParseDouble(const FStringView text, const double defaultValue)
{
	if (TOptional<double> result = TryParseDouble(text); result.HasValue())
	{
		return result.GetValue();
	}
	return defaultValue;
}

TOptional<int8> FStringParser::TryParseInt8(const FStringView text, const ENumericBase base)
{
	int8 result = 0;
	if (::TryParseInt<int8>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<int16> FStringParser::TryParseInt16(const FStringView text, const ENumericBase base)
{
	int16 result = 0;
	if (::TryParseInt<int16>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<int32> FStringParser::TryParseInt32(const FStringView text, const ENumericBase base)
{
	int32 result = 0;
	if (::TryParseInt<int32>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<int64> FStringParser::TryParseInt64(const FStringView text, const ENumericBase base)
{
	int64 result = 0;
	if (::TryParseInt<int64>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<uint8> FStringParser::TryParseUint8(const FStringView text, const ENumericBase base)
{
	uint8 result = 0;
	if (::TryParseInt<uint8>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<uint16> FStringParser::TryParseUint16(const FStringView text, const ENumericBase base)
{
	uint16 result = 0;
	if (::TryParseInt<uint16>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<uint32> FStringParser::TryParseUint32(const FStringView text, const ENumericBase base)
{
	uint32 result = 0;
	if (::TryParseInt<uint32>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<uint64> FStringParser::TryParseUint64(const FStringView text, const ENumericBase base)
{
	uint64 result = 0;
	if (::TryParseInt<uint64>(text, result, base))
	{
		return result;
	}
	return nullopt;
}

TOptional<float> FStringParser::TryParseFloat(const FStringView text)
{
	float result = 0.0f;
	if (::TryParseFloat<float>(text, result))
	{
		return result;
	}
	return nullopt;
}

TOptional<double> FStringParser::TryParseDouble(const FStringView text)
{
	double result = 0.0;
	if (::TryParseFloat<double>(text, result))
	{
		return result;
	}
	return nullopt;
}
#pragma once

#include "Containers/Optional.h"
#include "Containers/StringView.h"
#include "Misc/NumericBase.h"
#include "Templates/NumericLimits.h"

/**
 * @brief Defines utility functions for parsing strings into primitives.
 */
class FStringParser final
{
public: // Functions

	/**
	 * @brief Attempts to parse a signed, 8-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static int8 ParseInt8(FStringView text, ENumericBase base = ENumericBase::Decimal, int8 defaultValue = TNumericLimits<int8>::MaxValue);

	/**
	 * @brief Attempts to parse a signed, 16-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static int16 ParseInt16(FStringView text, ENumericBase base = ENumericBase::Decimal, int16 defaultValue = TNumericLimits<int16>::MaxValue);

	/**
	 * @brief Attempts to parse a signed, 32-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static int32 ParseInt32(FStringView text, ENumericBase base = ENumericBase::Decimal, int32 defaultValue = TNumericLimits<int32>::MaxValue);

	/**
	 * @brief Attempts to parse a signed, 64-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static int64 ParseInt64(FStringView text, ENumericBase base = ENumericBase::Decimal, int64 defaultValue = TNumericLimits<int64>::MaxValue);

	/**
	 * @brief Attempts to parse an unsigned, 8-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static uint8 ParseUint8(FStringView text, ENumericBase base = ENumericBase::Decimal, uint8 defaultValue = TNumericLimits<uint8>::MaxValue);

	/**
	 * @brief Attempts to parse an unsigned, 16-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static uint16 ParseUint16(FStringView text, ENumericBase base = ENumericBase::Decimal, uint16 defaultValue = TNumericLimits<uint16>::MaxValue);

	/**
	 * @brief Attempts to parse an unsigned, 32-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static uint32 ParseUint32(FStringView text, ENumericBase base = ENumericBase::Decimal, uint32 defaultValue = TNumericLimits<uint32>::MaxValue);

	/**
	 * @brief Attempts to parse an unsigned, 64-bit integer from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static uint64 ParseUint64(FStringView text, ENumericBase base = ENumericBase::Decimal, uint64 defaultValue = TNumericLimits<uint64>::MaxValue);

	/**
	 * @brief Attempts to parse a 32-bit floating-point number from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static float ParseFloat(FStringView text, float defaultValue = TNumericLimits<float>::MaxValue);

	/**
	 * @brief Attempts to parse a 64-bit floating-point number from a string, providing a default value if one could not be parsed.
	 *
	 * @param text The string to parse.
	 * @param defaultValue The default value to provide.
	 * @return The parsed value, or \p defaultValue if \p text could not be parsed.
	 */
	static double ParseDouble(FStringView text, double defaultValue = TNumericLimits<double>::MaxValue);

	/**
	 * @brief Attempts to parse a signed, 8-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<int8> TryParseInt8(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse a signed, 16-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<int16> TryParseInt16(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse a signed, 32-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<int32> TryParseInt32(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse a signed, 64-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<int64> TryParseInt64(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse an unsigned, 8-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<uint8> TryParseUint8(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse an unsigned, 16-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<uint16> TryParseUint16(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse an unsigned, 32-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<uint32> TryParseUint32(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse an unsigned, 64-bit integer from a string.
	 *
	 * @param text The string to parse.
	 * @param base The expected base the string is in.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<uint64> TryParseUint64(FStringView text, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Attempts to parse a 32-bit floating-point number from a string
	 *
	 * @param text The string to parse.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<float> TryParseFloat(FStringView text);

	/**
	 * @brief Attempts to parse a 64-bit floating-point number from a string
	 *
	 * @param text The string to parse.
	 * @return The parsed value, or nullopt if \p text could not be parsed.
	 */
	static TOptional<double> TryParseDouble(FStringView text);
};
#pragma once

#include "Containers/Optional.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Misc/NumericBase.h"
#include "Misc/StringFormatting.h"

/**
 * @brief Defines a way to build strings.
 */
class FStringBuilder
{
public:

	using CharType = typename FString::CharType;
	using SizeType = typename FString::SizeType;

	/**
	 * @brief Adds a number of zeroed characters.
	 *
	 * @param numChars The number of characters.
	 * @return The pointer to the first character in the added bunch.
	 */
	[[nodiscard]] CharType* AddZeroed(SizeType numChars);

	/**
	 * @brief Appends a string.
	 *
	 * @param string The string.
	 * @return This string builder.
	 */
	FStringBuilder& Append(const FString& string);

	/**
	 * @brief Appends a string view.
	 *
	 * @param stringView The string view.
	 * @return This string builder.
	 */
	FStringBuilder& Append(FStringView stringView);

	/**
	 * @brief Appends a formatted string to this string builder.
	 *
	 * @tparam FirstArgType The type of the first argument.
	 * @tparam ArgTypes The types of the other arguments.
	 * @param formatString The format string.
	 * @param firstArg The first argument to use when formatting.
	 * @param otherArgs The other arguments to use when formatting.
	 * @return This string builder.
	 */
	template<typename FirstArgType, typename... ArgTypes>
	FStringBuilder& Append(const FStringView formatString, const FirstArgType& firstArg, const ArgTypes&... otherArgs)
	{
		TArray<Private::FStringFormatArgument> formatArgs = Private::MakeFormatArgumentArray(firstArg, otherArgs...);
		return AppendFormattedString(formatString, formatArgs.AsSpan());
	}

	/**
	 * @brief Appends characters.
	 *
	 * @param chars The characters.
	 * @param numChars The number of characters.
	 * @return This string builder.
	 */
	FStringBuilder& Append(const CharType* chars, SizeType numChars);

	/**
	 * @brief Appends the given character a number of times.
	 *
	 * @param ch The character to append.
	 * @param numChars The number of times to append \p ch.
	 * @return This string builder.
	 */
	FStringBuilder& Append(CharType ch, SizeType numChars);

	/**
	 * @brief Appends the given character.
	 *
	 * @param ch The character to append.
	 * @return This string builder.
	 */
	FStringBuilder& Append(CharType ch)
	{
		return Append(ch, 1);
	}

	/**
	 * @brief Appends a single precision floating point number to the built string.
	 *
	 * @param value The value to append.
	 * @param numDecimals The number of decimals to append.
	 * @return This string builder.
	 */
	FStringBuilder& Append(float value, TOptional<int32> numDecimals = nullopt);

	/**
	 * @brief Appends a double precision floating point number to the built string.
	 *
	 * @param value The value to append.
	 * @param numDecimals The number of decimals to append.
	 * @return This string builder.
	 */
	FStringBuilder& Append(double value, TOptional<int32> numDecimals = nullopt);

	/**
	 * @brief Appends a signed integer to this string builder.
	 *
	 * @param value The value to append.
	 * @param base The base to append the integer in. Will be ignored if the value is negative.
	 * @return This string builder.
	 */
	FStringBuilder& Append(const int32 value, const ENumericBase base = ENumericBase::Decimal)
	{
		return Append(static_cast<int64>(value), base);
	}

	/**
	 * @brief Appends a signed integer to this string builder.
	 *
	 * @param value The value to append.
	 * @param base The base to append the integer in. Will be ignored if the value is negative.
	 * @return This string builder.
	 */
	FStringBuilder& Append(int64 value, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Appends an unsigned integer to this string builder.
	 *
	 * @param value The value to append.
	 * @param base The base to append the integer in.
	 * @return This string builder.
	 */
	FStringBuilder& Append(const uint32 value, const ENumericBase base = ENumericBase::Decimal)
	{
		return Append(static_cast<uint64>(value), base);
	}

	/**
	 * @brief Appends an unsigned integer to this string builder.
	 *
	 * @param value The value to append.
	 * @param base The base to append the integer in.
	 * @return This string builder.
	 */
	FStringBuilder& Append(uint64 value, ENumericBase base = ENumericBase::Decimal);

	/**
	 * @brief Gets a string view representing the current state of this string builder.
	 *
	 * @return A string view representing the current state of this string builder.
	 */
	[[nodiscard]] FStringView AsStringView() const;

	/**
	 * @brief Gets this string builder's characters.
	 *
	 * @return This string builder's characters.
	 */
	[[nodiscard]] const CharType* GetChars() const
	{
		return m_Chars.GetData();
	}

	/**
	 * @brief Gets this string builder's characters.
	 *
	 * @return This string builder's characters.
	 */
	[[nodiscard]] CharType* GetChars()
	{
		return m_Chars.GetData();
	}

	/**
	 * @brief Gets the last character added to this string builder.
	 *
	 * @return The last character added to this string builder.
	 */
	[[nodiscard]] CharType GetLastChar() const
	{
		return m_Chars.IsEmpty() ? FStringView::CharTraitsType::NullChar : m_Chars.Last();
	}

	/**
	 * @brief Gets the number of characters currently in this string builder.
	 *
	 * @return The number of characters currently in this string builder.
	 */
	[[nodiscard]] SizeType Length() const
	{
		return m_Chars.Num();
	}

	/**
	 * @brief Releases the underlying characters to be used as a string. This effectively resets this string builder.
	 *
	 * @return The string.
	 */
	FString ReleaseString();

	/**
	 * @brief Ensures the underlying character array has at least the given desired capacity.
	 *
	 * @param reserveAmount The number of characters to reserve.
	 */
	FStringBuilder& Reserve(SizeType reserveAmount);

#define DEFINE_INSERTION_OPERATOR(Type) \
	inline FStringBuilder& operator<<(Type value) \
	{ \
		Append(value); \
		return *this; \
	}

	DEFINE_INSERTION_OPERATOR(const FString&);
	DEFINE_INSERTION_OPERATOR(const FStringView);
	DEFINE_INSERTION_OPERATOR(const float);
	DEFINE_INSERTION_OPERATOR(const double);
	DEFINE_INSERTION_OPERATOR(const int64);
	DEFINE_INSERTION_OPERATOR(const uint64);

#undef DEFINE_INSERTION_OPERATOR

private:

	/**
	 * @brief Appends a formatted string to this string builder.
	 *
	 * @param formatString The format string.
	 * @param formatArgs The format arguments.
	 * @return This string builder.
	 */
	FStringBuilder& AppendFormattedString(FStringView formatString, TSpan<Private::FStringFormatArgument> formatArgs);

	TArray<CharType> m_Chars;
};
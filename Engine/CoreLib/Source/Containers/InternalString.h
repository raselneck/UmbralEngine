#pragma once

#include "Containers/Array.h"
#include "Containers/Function.h"
#include "Containers/Optional.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Misc/StringBuilder.h"
#include "Misc/StringFormatting.h"

// TODO Better name than "To Chars Args"

/**
 * @brief Defines arguments used when converting various types to a character array.
 */
struct FToCharsArgs
{
	/** @brief The default "to chars" arguments. */
	static const FToCharsArgs Default;

	TOptional<int32> NumDecimals;
	TOptional<int32> PadLeftAmount;
	TOptional<int32> PadRightAmount;
	char PadLeftChar = ' ';
	char PadRightChar = ' ';
	ENumericBase NumericBase = ENumericBase::Decimal;

	/**
	 * @brief Creates "to chars" arguments that specify padding a value on the left.
	 *
	 * @param padChar The character to use when padding on the left.
	 * @param padAmount The total width of the left padding.
	 * @return The created "to chars" arguments.
	 */
	static FToCharsArgs PadLeft(char padChar, int32 padAmount);

	/**
	 * @brief Creates "to chars" arguments that specify padding a value on the right.
	 *
	 * @param padLeftChar The character to use when padding on the right.
	 * @param padLeftAmount The total width of the right padding.
	 * @return The created "to chars" arguments.
	 */
	static FToCharsArgs PadRight(char padChar, int32 padAmount);

	/**
	 * @brief Parses "to chars" arguments from a format string.
	 *
	 * @param format The format string.
	 * @return The "to chars" arguments.
	 */
	static FToCharsArgs ParseFormat(FStringView format);

	/**
	 * @brief Creates "to chars" arguments that specify a number of decimals.
	 *
	 * @param numDecimals The number of decimals.
	 * @return The arguments.
	 */
	static FToCharsArgs WithDecimals(const int32 numDecimals)
	{
		FToCharsArgs args;
		args.NumDecimals = numDecimals;
		return args;
	}

	/**
	 * @brief Creates "to chars" arguments that specify a number of decimals.
	 *
	 * @param numDecimals The number of decimals.
	 * @return The arguments.
	 */
	static FToCharsArgs WithDecimals(TOptional<int32> numDecimals)
	{
		FToCharsArgs args;
		args.NumDecimals = MoveTemp(numDecimals);
		return args;
	}

	/**
	 * @brief Creates "to chars" arguments that specify a numeric base.
	 *
	 * @param numericBase The numeric base.
	 * @return The arguments.
	 */
	static FToCharsArgs WithNumericBase(const ENumericBase numericBase)
	{
		FToCharsArgs args;
		args.NumericBase = numericBase;
		return args;
	}
};

namespace Private
{
	/**
	 * @brief Appends characters for a double-precision floating-point value to a character array.
	 *
	 * @param builder The string builder.
	 * @param value The value to append the characters for.
	 * @param args The arguments to use for padding the given value.
	 */
	void AppendCharsForDouble(FStringBuilder& builder, double value, const FToCharsArgs& args = FToCharsArgs::Default);

	/**
	 * @brief Appends characters for a single-precision floating-point value to a character array.
	 *
	 * @param builder The string builder.
	 * @param value The value to append the characters for.
	 * @param args The arguments to use for padding the given value.
	 */
	void AppendCharsForFloat(FStringBuilder& builder, float value, const FToCharsArgs& args = FToCharsArgs::Default);

	/**
	 * @brief Appends characters for a pointer value to a character array.
	 *
	 * @param builder The string builder.
	 * @param value The value to append the characters for.
	 * @param args The arguments to use for padding the given value.
	 */
	void AppendCharsForPointer(FStringBuilder& builder, const void* value, const FToCharsArgs& args = FToCharsArgs::Default);

	/**
	 * @brief Appends characters for a signed integer value to a character array.
	 *
	 * @param builder The string builder.
	 * @param value The value to append the characters for.
	 * @param args The arguments to use for padding the given value.
	 */
	void AppendCharsForSignedInt(FStringBuilder& builder, int64 value, const FToCharsArgs& args = FToCharsArgs::Default);

	/**
	 * @brief Appends characters for an unsigned integer value to a character array.
	 *
	 * @param builder The string builder.
	 * @param value The value to append the characters for.
	 * @param args The arguments to use for padding the given value.
	 */
	void AppendCharsForUnsignedInt(FStringBuilder& builder, uint64 value, const FToCharsArgs& args = FToCharsArgs::Default);

	/**
	 * @brief Appends a string to a string builder with arguments.
	 *
	 * @param builder The string builder.
	 * @param value The string to append.
	 * @param args The arguments.
	 */
	void AppendStringWithArguments(FStringBuilder& builder, FStringView value, const FToCharsArgs& args);

	/**
	 * @brief Appends a formatted string to the given string builder.
	 *
	 * @param builder The string builder.
	 * @param format The message format.
	 * @param args The message arguments.
	 */
	void AppendFormattedString(FStringBuilder& builder, FStringView format, TSpan<Private::FStringFormatArgument> args);

	template<typename StringLikeType>
	using TGetNextIndexToSplitOn = TFunction<int32(const StringLikeType& string, const FStringView& data, int32 startIndex, int32& outSplitLength)>;

	template<typename T>
	struct TGetSubstringView;

	template<>
	struct TGetSubstringView<FString>
	{
		static FStringView Get(const FString& value, const int32 index, const int32 length)
		{
			return value.SubstringView(index, length);
		}
	};

	template<>
	struct TGetSubstringView<FStringView>
	{
		static FStringView Get(const FStringView& value, const int32 index, const int32 length)
		{
			return value.Substring(index, length);
		}
	};

	/**
	 * @brief Gets the next index of a substring to split on.
	 *
	 * @param string The string being split.
	 * @param substring The substring to split on.
	 * @param startIndex The start index to search for the \p substring.
	 * @param outSplitLength The length of the split.
	 * @return The next index of the substring to split on.
	 */
	template<typename StringLikeType>
	static int32 GetNextIndexOfSubstringToSplitOn(const StringLikeType& string, const FStringView& substring, const int32 startIndex, int32& outSplitLength)
	{
		outSplitLength = substring.Length();
		return string.IndexOf(substring, startIndex);
	}

	/**
	 * @brief Gets the index of the closest character to split on.
	 *
	 * @param string The string being split.
	 * @param chars The characters to split on.
	 * @param startIndex The start index to search for the \p substring.
	 * @param outSplitLength The length of the split.
	 * @return The next index of the substring to split on.
	 */
	template<typename StringLikeType>
	static int32 GetNextIndexOfClosestCharToSplitOn(const StringLikeType& string, const FStringView& chars, const int32 startIndex, int32& outSplitLength)
	{
		outSplitLength = 1;

		int32 closestIndex = string.IndexOf(chars[0], startIndex);
		for (int32 idx = 1; idx < chars.Length(); ++idx)
		{
			int32 currentIndex = string.IndexOf(chars[idx], startIndex);
			if (closestIndex == INDEX_NONE || (currentIndex != INDEX_NONE && currentIndex < closestIndex))
			{
				closestIndex = currentIndex;
			}
		}

		return closestIndex;
	}

	/**
	 * @brief Splits a string.
	 *
	 * @tparam CustomDataType The type of custom data supplied to \p getNextIndexToSplitOn.
	 * @tparam StringLikeType The resulting string-like type.
	 * @param string The string being split.
	 * @param options The split options.
	 * @param customData The custom data to supply to \p getNextIndexToSplitOn.
	 * @param result The result array to append to.
	 * @param getNextIndexToSplitOn The callback to use when determining the index of the next split.
	 */
	template<typename StringLikeType, typename ResultType>
	static void SplitString(const StringLikeType& string,
	                        const EStringSplitOptions options,
	                        const FStringView& customData,
	                        TArray<ResultType>& result,
	                        TGetNextIndexToSplitOn<StringLikeType> getNextIndexToSplitOn)
	{
		const int32 length = string.Length();
		int32 substringStartIndex = 0;
		int32 splitLength = 0;
		int32 splitIndex = getNextIndexToSplitOn(string, customData, 0, splitLength);
		while (splitIndex != INDEX_NONE)
		{
			const FStringView line = TGetSubstringView<StringLikeType>::Get(string, substringStartIndex, splitIndex - substringStartIndex);

			substringStartIndex = splitIndex + splitLength;
			splitIndex = getNextIndexToSplitOn(string, customData, substringStartIndex, splitLength);

			// Ignore empty entries if we were told to
			if (line.IsEmpty() && HasFlag(options, EStringSplitOptions::IgnoreEmptyEntries))
			{
				continue;
			}

			(void)result.Emplace(line);
		}

		// Add the last line
		if (length > substringStartIndex)
		{
			const FStringView lastLine = TGetSubstringView<StringLikeType>::Get(string, substringStartIndex, length - substringStartIndex);
			if (lastLine.IsEmpty() && HasFlag(options, EStringSplitOptions::IgnoreEmptyEntries))
			{
				return;
			}
			(void)result.Emplace(lastLine);
		}
	}
}
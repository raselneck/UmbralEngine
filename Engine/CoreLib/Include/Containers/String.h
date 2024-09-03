#pragma once

#include "Containers/Array.h"
#include "Containers/Optional.h"
#include "Containers/StringView.h"
#include "Containers/Variant.h"
#include "Engine/Hashing.h"
#include "Meta/TypeInfo.h"
#include "Misc/Badge.h"
#include "Misc/StringFormatting.h"
#include "Templates/CharTraits.h"
#include "Templates/IsSame.h"
#include "Templates/IsZeroConstructible.h"
#include <cstdarg>

class FStringBuilder;

namespace Private
{
	/**
	 * @brief Defines data for a long string.
	 */
	struct FLongStringData
	{
		using CharType = FStringView::CharType;
		using SizeType = FStringView::SizeType;

		/** @brief The dynamic array containing the characters. */
		TArray<CharType> Chars;

		/**
		 * @brief Gets the length of the string defined by this data.
		 *
		 * @return The length of the string defined by this data.
		 */
		[[nodiscard]] SizeType Length() const;
	};

	/**
	 * @brief Defines data for a short string.
	 */
	struct FShortStringData
	{
		using CharType = FStringView::CharType;
		using SizeType = FStringView::SizeType;

		/**
		 * @brief The maximum number of characters allowed for a short string.
		 */
		static constexpr int32 MaxNumChars = sizeof(FLongStringData) - sizeof(uint8); // Subtract sizeof(uint8) for our Length

		/**
		 * @brief The maximum length for a short string before it needs to be converted to an array string.
		 */
		static constexpr int32 MaxLength = MaxNumChars - 1;

		/** @brief This short string data's characters. */
		CharType Chars[MaxNumChars];

		/** @brief This short string data's length. */
		uint8 Length = 0;

		/**
		 * @brief Sets default values for this short-string's properties.
		 */
		FShortStringData();

		// Copy or move construct this short string data
		FShortStringData(const FShortStringData&);
		FShortStringData(FShortStringData&&) noexcept;

		// Copy or move assign to this short string data
		FShortStringData& operator=(const FShortStringData&);
		FShortStringData& operator=(FShortStringData&&) noexcept;

		/**
		 * @brief Gets this short string data's characters as a span.
		 *
		 * @return This short string data's characters as a span.
		 */
		[[nodiscard]] TSpan<const CharType> AsSpan() const
		{
			return TSpan<const CharType> { Chars, static_cast<SizeType>(Length) };
		}
	};
}

template<>
struct TIsZeroConstructible<Private::FShortStringData> : FTrueType
{
};

/**
 * @brief Defines a UTF-8 encoded, dynamically resizable string.
 */
class FString final
{
	friend class FStringBuilder;

public:

	using CharType = FStringView::CharType;
	using SizeType = FStringView::SizeType;
	using IteratorType = typename TAddPointer<CharType>::Type;
	using ConstIteratorType = typename TAddPointer<const CharType>::Type;
	using TraitsType = TCharTraits<CharType>;
	using SpanType = TSpan<CharType>;
	using ConstSpanType = TSpan<const CharType>;
	using CharDataType = TVariant<Private::FShortStringData, Private::FLongStringData>;

	/**
	 * @brief Sets default values for this string's properties.
	 */
	FString() = default;

	/**
	 * @brief Creates a string from the given characters.
	 *
	 * @param chars The characters.
	 */
	explicit FString(const CharType* chars);

	/**
	 * @brief Creates a string from the given characters.
	 *
	 * @param chars The characters.
	 * @param numChars The number of characters.
	 */
	FString(const CharType* chars, SizeType numChars);

	/**
	 * @brief Creates a string from a string view.
	 *
	 * @param chars The string view.
	 */
	explicit FString(FStringView chars);

	/**
	 * @brief Appends the given characters to this string.
	 *
	 * @param chars The characters to append.
	 * @param numChars The number of characters to append.
	 */
	void Append(const CharType* chars, SizeType numChars);

	/**
	 * @brief Appends the given string view to this string.
	 *
	 * @param value The value to append.
	 */
	void Append(FStringView value);

	/**
	 * @brief Gets this string as its lowercase variant.
	 *
	 * @return This string as its lowercase variant.
	 */
	[[nodiscard]] FString AsLower() const;

	/**
	 * @brief Converts this string to a span.
	 *
	 * @return The span.
	 */
	[[nodiscard]] ConstSpanType AsSpan() const
	{
		return ConstSpanType { GetChars(), Length() };
	}

	/**
	 * @brief Converts this string to a mutable span.
	 *
	 * @return The mutable span.
	 */
	[[nodiscard]] SpanType AsSpan()
	{
		return SpanType { GetChars(), Length() };
	}

	// TODO Assign ?

	/**
	 * @brief Converts this string to a string view.
	 *
	 * @return This string as a string view.
	 */
	[[nodiscard]] FStringView AsStringView() const &
	{
		return FStringView { GetChars(), Length() };
	}

	/** r-value strings should not be converted to string views. */
	[[nodiscard]] FStringView AsStringView() const && = delete;

	/**
	 * @brief Gets this string as its uppercase variant.
	 *
	 * @return This string as its uppercase variant.
	 */
	[[nodiscard]] FString AsUpper() const;

	// TODO Clear

	// TODO Overloads for functions that take EStringComparison instead of EIgnoreCase

	/**
	 * @brief Compares this string to another string.
	 *
	 * @param other The other string.
	 * @param ignoreCase Whether case should be ignored when comparing the string views.
	 * @return The result of the comparison.
	 */
	[[nodiscard]] ECompareResult Compare(const FStringView other, const EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return AsStringView().Compare(other, ignoreCase);
	}

	/**
	 * @brief Compares this string view to another string view.
	 *
	 * @param other The other string view.
	 * @param comparison The string comparison type.
	 * @return The result of the comparison.
	 */
	[[nodiscard]] ECompareResult Compare(const FStringView other, const EStringComparison comparison) const
	{
		return AsStringView().Compare(other, comparison);
	}

	/**
	 * @brief Checks to see if this string ends with the given other string view.
	 *
	 * @param other The other string.
	 * @return True if this string ends with \p other, otherwise false.
	 */
	[[nodiscard]] bool EndsWith(FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const;

	/**
	 * @brief Checks to see if this string ends with the given character.
	 *
	 * @param character The character.
	 * @return True if this string ends with \p other, otherwise false.
	 */
	[[nodiscard]] bool EndsWith(const CharType character, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return EndsWith(FStringView { &character, 1 }, ignoreCase);
	}

	/**
	 * @brief Checks to see if this string equals another.
	 *
	 * @param other The other string.
	 * @param ignoreCase Whether to ignore casing when checking for equality.
	 * @return True if this string equals \p other, otherwise false.
	 */
	[[nodiscard]] bool Equals(const FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return Compare(other, ignoreCase) == ECompareResult::Equals;
	}

	/**
	 * @brief Creates a formatted string.
	 *
	 * @tparam ArgTypes The types of the arguments to supply when formatting.
	 * @param format The format for the string.
	 * @param args The arguments to apply to the format.
	 * @return The formatted string.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] static FString Format(const FStringView format, ArgTypes&&... args)
	{
		if constexpr (sizeof...(args) == 0)
		{
			return FString { format };
		}

		// TODO Somehow use GetExpectedNumFormatArgs to determine if formatting will work at compile time

		if (format.IsEmpty())
		{
			return { };
		}

		TArray<Private::FStringFormatArgument> formatArgs = Private::MakeFormatArgumentArray(Forward<ArgTypes>(args)...);
		return MakeFormattedString(format, formatArgs.AsSpan());
	}

	/**
	 * @brief Creates a formatted string using C-style (printf) semantics.
	 *
	 * @param format The format string.
	 * @return The formatted string.
	 */
	[[nodiscard]] static FString FormatCStyle(FStringView format, ...);

	/**
	 * @brief Creates a formatted string using a C-style va_list.
	 *
	 * @param format The string format.
	 * @param args The arg list.
	 * @return The formatted string.
	 */
	[[nodiscard]] static FString FormatVarArgList(FStringView format, va_list args);

	// TODO FormatC (to use FormatVarArgList)

	// TODO FromChar

	// TODO FromCodepoint

	// TODO FromDouble

	// TODO FromInt

	// TODO FromFloat

	// TODO FromPointer

	// TODO FromUint

	/**
	 * @brief Creates a string from UTF-16 characters.
	 *
	 * @param chars The UTF-16 characters.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromUtf16(const char16_t* chars);

	/**
	 * @brief Creates a string from UTF-16 characters.
	 *
	 * @param charSPan The UTF-16 character span.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromUtf16(TSpan<const char16_t> charSpan);

	/**
	 * @brief Creates a string from UTF-32 characters.
	 *
	 * @param chars The UTF-32 characters.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromUtf32(const char32_t* chars);

	/**
	 * @brief Creates a string from UTF-32 characters.
	 *
	 * @param charSpan The UTF-32 character span.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromUtf32(TSpan<const char32_t> charSpan);

	/**
	 * @brief Creates a string from wide characters.
	 *
	 * @param chars The wide characters.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromWide(const wchar_t* chars);

	/**
	 * @brief Creates a string from wide characters.
	 *
	 * @param charSpan The wide character span.
	 * @return The string.
	 */
	[[nodiscard]] static FString FromWide(TSpan<const wchar_t> charSpan);

	/**
	 * @brief Gets this string's characters.
	 *
	 * @return This string's characters.
	 */
	[[nodiscard]] const CharType* GetChars() const;

	/**
	 * @brief Gets this string's characters.
	 *
	 * @return This string's characters.
	 */
	[[nodiscard]] CharType* GetChars();

	/**
	 * @brief Gets the index of a character in this string.
	 *
	 * @param value The character to find.
	 * @return The starting index of the character, or INDEX_NONE if the character was not found.
	 */
	[[nodiscard]] SizeType IndexOf(const CharType value) const
	{
		return AsStringView().IndexOf(value);
	}

	/**
	 * @brief Gets the index of a character in this string.
	 *
	 * @param value The character to find.
	 * @param startIndex The index to start searching at.
	 * @param ignoreCase Whether to ignore casing when searching for the character.
	 * @return The starting index of the character, or INDEX_NONE if the substring was not found.
	 */
	[[nodiscard]] SizeType IndexOf(const CharType value, const SizeType startIndex) const
	{
		return AsStringView().IndexOf(value, startIndex);
	}

	/**
	 * @brief Gets the index of a substring in this string.
	 *
	 * @param value The substring to find.
	 * @param ignoreCase Whether to ignore casing when searching for the substring.
	 * @return The starting index of the substring, or INDEX_NONE if the substring was not found.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return AsStringView().IndexOf(value, ToStringComparison(ignoreCase));
	}

	/**
	 * @brief Gets the index of a substring in this string.
	 *
	 * @param value The substring to find.
	 * @param startIndex The index to start searching at.
	 * @param ignoreCase Whether to ignore casing when searching for the substring.
	 * @return The starting index of the substring, or INDEX_NONE if the substring was not found.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const SizeType startIndex, const EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return AsStringView().IndexOf(value, startIndex, ToStringComparison(ignoreCase));
	}

	/**
	 * @brief Checks to see if this string is empty.
	 *
	 * @return True if this string is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const;

	/**
	 * @brief Checks to see if the given index is valid for this string.
	 *
	 * @param index The index.
	 * @return True if \p index is a valid index for this string, otherwise false.
	 */
	[[nodiscard]] bool IsValidIndex(SizeType index) const;

	/**
	 * @brief Gets the last character in this string.
	 *
	 * @return The last character in this string.
	 */
	[[nodiscard]] TOptional<CharType> Last() const;

	// TODO Left

	/**
	 * @brief Gets the number of characters in this string, not including the null terminator.
	 *
	 * @return The number of characters in this string, not including the null terminator.
	 */
	[[nodiscard]] SizeType Length() const;

	// TODO PadLeft

	// TODO PadRight

	/**
	 * @brief Prepends the given characters to this string.
	 *
	 * @param chars The characters to prepend.
	 * @param numChars The number of characters to prepend.
	 */
	void Prepend(const CharType* chars, SizeType numChars);

	/**
	 * @brief Prepends the given string view to this string.
	 *
	 * @param value The value to prepend.
	 */
	void Prepend(FStringView value);

	/**
	 * @brief Reserves memory in the underlying array for the given number of characters.
	 *
	 * This function expects that the caller is accounting for the eventually necessary null terminator.
	 * If the caller does not account for this, then an allocation may occur when populating this string
	 * with the desired characters.
	 *
	 * @param desiredCapacity The total desired number of characters to reserve memory for.
	 */
	void Reserve(SizeType desiredCapacity);

	/**
	 * @brief Clears this string without freeing any potentially allocated memory.
	 */
	void Reset();

	// TODO Right

	/**
	 * @brief Splits this string by a collection of characters into an array of strings.
	 *
	 * This string will be split by each character in \p chars, such that none of
	 * the resulting strings in \p result will contain any character in \p chars.
	 *
	 * @param chars The character collection.
	 * @param options The split options.
	 * @param result The string array to append to.
	 */
	void SplitByChars(FStringView chars, EStringSplitOptions options, TArray<FString>& result) const;

	/**
	 * @brief Splits this string by a collection of characters into an array of strings.
	 *
	 * This string will be split by each character in \p chars, such that none of
	 * the resulting strings in \p result will contain any character in \p chars.
	 *
	 * @param chars The character collection.
	 * @param options The split options.
	 * @return The split strings.
	 */
	[[nodiscard]] TArray<FString> SplitByChars(FStringView chars, EStringSplitOptions options = EStringSplitOptions::None) const
	{
		TArray<FString> result;
		SplitByChars(chars, options, result);
		return result;
	}

	/**
	 * @brief Splits this string by a collection of characters into an array of string views.
	 *
	 * This string will be split by each character in \p chars, such that none of
	 * the resulting strings in \p result will contain any character in \p chars.
	 *
	 * @param chars The character collection.
	 * @param options The split options.
	 * @param result The string view array to append to.
	 */
	void SplitByCharsIntoViews(FStringView chars, EStringSplitOptions options, TArray<FStringView>& result) const;

	/**
	 * @brief Splits this string by a collection of characters into an array of string views.
	 *
	 * This string will be split by each character in \p chars, such that none of
	 * the resulting strings in \p result will contain any character in \p chars.
	 *
	 * @param chars The character collection.
	 * @param options The split options.
	 * @return The split string views.
	 */
	[[nodiscard]] TArray<FStringView> SplitByCharsIntoViews(FStringView chars, EStringSplitOptions options = EStringSplitOptions::None) const
	{
		TArray<FStringView> result;
		SplitByCharsIntoViews(chars, options, result);
		return result;
	}

	/**
	 * @brief Splits this string by a substring into an array of strings.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @param result The string array to append to.
	 */
	void SplitByString(FStringView substring, EStringSplitOptions options, TArray<FString>& result) const;

	/**
	 * @brief Splits this string by a substring into an array of strings.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @return The split strings.
	 */
	[[nodiscard]] TArray<FString> SplitByString(const FStringView substring, const EStringSplitOptions options = EStringSplitOptions::None) const
	{
		TArray<FString> result;
		SplitByString(substring, options, result);
		return result;
	}

	/**
	 * @brief Splits this string by a substring into an array of string views.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @param result The result string view array.
	 */
	void SplitByStringIntoViews(FStringView substring, EStringSplitOptions options, TArray<FStringView>& result) const;

	/**
	 * @brief Splits this string by a substring into an array of strings.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @return The split string views.
	 */
	[[nodiscard]] TArray<FStringView> SplitByStringIntoViews(const FStringView substring, const EStringSplitOptions options = EStringSplitOptions::None) const
	{
		TArray<FStringView> result;
		SplitByStringIntoViews(substring, options, result);
		return result;
	}

	/**
	 * @brief Checks to see if this string starts with the given other string.
	 *
	 * @param other The other string.
	 * @return True if this string starts with \p other, otherwise false.
	 */
	[[nodiscard]] bool StartsWith(FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const;

	/**
	 * @brief Checks to see if this string starts with the given character.
	 *
	 * @param character The character.
	 * @return True if this string starts with \p other, otherwise false.
	 */
	[[nodiscard]] bool StartsWith(const CharType character, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return StartsWith(FStringView { &character, 1 }, ignoreCase);
	}

	/**
	 * @brief Gets a substring of this string.
	 *
	 * @param index The substring start index.
	 * @param length The substring length.
	 * @return The substring of this string.
	 */
	[[nodiscard]] FString Substring(SizeType index, SizeType length) const;

	/**
	 * @brief Gets a substring view of this string.
	 *
	 * @param index The substring start index.
	 * @param length The substring length.
	 * @return The substring view of this string.
	 */
	[[nodiscard]] FStringView SubstringView(SizeType index, SizeType length) const;

	/**
	 * @brief Converts this string to its lowercase variant in-line.
	 */
	void ToLower();

	/**
	 * @brief Converts this string to its uppercase variant in-line.
	 */
	void ToUpper();

	/**
	 * @brief Converts this string to an array of UTF-16 characters.
	 *
	 * @return The array of UTF-16 characters.
	 */
	[[nodiscard]] TArray<char16_t> ToUtf16Chars() const;

	/**
	 * @brief Converts this string to an array of UTF-32 characters.
	 *
	 * @return The array of UTF-32 characters.
	 */
	[[nodiscard]] TArray<char32_t> ToUtf32Chars() const;

	/**
	 * @brief Converts this string to an array of wide characters.
	 *
	 * @return The array of wide characters.
	 */
	[[nodiscard]] TArray<wchar_t> ToWideChars() const;

	/**
	 * @brief Gets the character at the given index.
	 *
	 * @param index The index of the character to get.
	 * @return The character at \p index.
	 */
	[[nodiscard]] const CharType& operator[](const SizeType index) const
	{
		UM_ASSERT(IsValidIndex(index), "Given invalid index for string");
		return GetChars()[index];
	}

	/**
	 * @brief Gets the character at the given index.
	 *
	 * @param index The index of the character to get.
	 * @return The character at \p index.
	 */
	[[nodiscard]] CharType& operator[](const SizeType index)
	{
		UM_ASSERT(IsValidIndex(index), "Given invalid index for string");
		return GetChars()[index];
	}

	/**
	 * @brief Implicitly converts this string to a string view.
	 *
	 * @return A string view representing this string.
	 */
	[[nodiscard]] operator FStringView() const &
	{
		return AsStringView();
	}

	/** r-value strings should not be converted to string views. */
	[[nodiscard]] operator FStringView() const && = delete;

	/**
	 * @brief Implicitly converts this string to a span.
	 *
	 * @return This string as a span.
	 */
	[[nodiscard]] explicit operator ConstSpanType() const
	{
		return AsSpan();
	}

	/**
	 * @brief Implicitly converts this string to a span.
	 *
	 * @return This string as a span.
	 */
	[[nodiscard]] explicit operator SpanType()
	{
		return AsSpan();
	}

	/**
	 * @brief Appends a string view to this string.
	 *
	 * @param other The string view.
	 * @return This string.
	 */
	FString& operator+=(const FStringView stringView)
	{
		Append(stringView);
		return *this;
	}

	// TODO For the allocating functions below, use GetConcatenatedCharCount

	// Will allocate new string as neither params can be modified
	friend FString operator+(const FString& left, const FString& right)
	{
		FString result { left };
		result.Append(right);
		return result;
	}

	// Will append `right` to `left then return `left`
	friend FString operator+(FString&& left, const FString& right)
	{
		left.Append(right);
		return MoveTemp(left);
	}

	// Will prepend `left` to `right` then return `right`
	friend FString operator+(const FString& left, FString&& right)
	{
		right.Prepend(left);
		return MoveTemp(right);
	}

	// Will append `right` to `left` then return `left`
	friend FString operator+(FString&& left, FString&& right)
	{
		left.Append(right.AsStringView());
		return MoveTemp(left);
	}

	// Will allocate new string as neither params can support the resulting concatenation
	friend FString operator+(FStringView left, const FString& right)
	{
		FString result { left };
		result.Append(right);
		return result;
	}

	// Will prepend `left` to `right` then return `right`
	friend FString operator+(FStringView left, FString&& right)
	{
		right.Prepend(left);
		return MoveTemp(right);
	}

	// Will allocate new string as neither params can support the resulting concatenation
	friend FString operator+(const FString& left, const FStringView right)
	{
		FString result { left };
		result.Append(right);
		return result;
	}

	// Will append `right` to `left` then return `left`
	friend FString operator+(FString&& left, const FStringView right)
	{
		left.Append(right);
		return MoveTemp(left);
	}

	/**
	 * @brief Writes this string to the given stream.
	 *
	 * @tparam StreamType The stream's type.
	 * @param stream The stream.
	 * @param value The string value to write.
	 * @return The stream.
	 */
	template<typename StreamType>
	friend StreamType& operator<<(StreamType& stream, const FString& value)
	{
		stream.write(value.GetChars(), value.Length());
		return stream;
	}

	// STL compatibility BEGIN
	[[nodiscard]] IteratorType      begin()        { return GetChars(); }
	[[nodiscard]] ConstIteratorType begin()  const { return GetChars(); }
	[[nodiscard]] ConstIteratorType cbegin() const { return begin(); }
	[[nodiscard]] IteratorType      end()          { return GetChars() + Length(); }
	[[nodiscard]] ConstIteratorType end()    const { return GetChars() + Length(); }
	[[nodiscard]] ConstIteratorType cend()   const { return end(); }
	[[nodiscard]] bool operator< (const FStringView other) const { return Compare(other, EStringComparison::Ordinal) == ECompareResult::LessThan; }
	[[nodiscard]] bool operator<=(const FStringView other) const { return Compare(other, EStringComparison::Ordinal) != ECompareResult::GreaterThan; }
	[[nodiscard]] bool operator> (const FStringView other) const { return Compare(other, EStringComparison::Ordinal) == ECompareResult::GreaterThan; }
	[[nodiscard]] bool operator>=(const FStringView other) const { return Compare(other, EStringComparison::Ordinal) != ECompareResult::LessThan; }
	[[nodiscard]] bool operator==(const FStringView other) const { return Compare(other, EStringComparison::Ordinal) == ECompareResult::Equals; }
	[[nodiscard]] bool operator!=(const FStringView other) const { return Compare(other, EStringComparison::Ordinal) != ECompareResult::Equals; }
	// STL compatibility END

private:

	/**
	 * @brief Creates a string directly from a character array.
	 *
	 * @param chars The character array.
	 */
	FString(TBadge<FStringBuilder>, TArray<CharType> chars);

	/**
	 * @brief Converts this string to a use long string data, but only if it is currently using
	 *        short string data and only if necessary for the number of characters being appended.
	 *
	 * @param numCharsBeingAppended The number of characters being appended.
	 */
	void ConvertToLongStringDataIfNecessary(SizeType numCharsBeingAppended);

	/**
	 * @brief Converts this string to a use long string data.
	 *
	 * @param numAdditionalChars The number of additional characters being added.
	 */
	void ConvertToLongStringData(SizeType numAdditionalChars = 0);

	/**
	 * @brief Gets the expected number of format arguments for a format string.
	 *
	 * @param format The format string.
	 * @return The expected number of format arguments.
	 */
	[[maybe_unused]] static constexpr int32 GetExpectedNumFormatArgs(const FStringView format)
	{
		int32 expectedNum = 0;
		for (int32 idx = 0; idx < format.Length(); ++idx)
		{
			const CharType ch = format[idx];
			if (ch != '{')
			{
				continue;
			}

			if (idx > 0 && format[idx - 1] == '\\')
			{
				continue;
			}

			while (format.IsValidIndex(idx) && format[idx] != '}')
			{
				++idx;
			}

			if (format.IsValidIndex(idx) == false)
			{
				throw std::runtime_error("Found unterminated argument entry in format string");
			}

			// TODO Verify format[idx] == '}'
		}
		return expectedNum;
	}

	/**
	 * @brief Gets the total number of characters necessary, including null terminator, to allocate for a concatenation.
	 *
	 * @param firstLength The length of the first string.
	 * @param secondLength The length of the second string.
	 * @return The number of concatenated characters to reserve memory for.
	 */
	[[maybe_unused]] static constexpr SizeType GetConcatenatedCharCount(const SizeType firstLength, const SizeType secondLength)
	{
		const SizeType charCount = firstLength + secondLength;
		return charCount == 0 ? 0 : charCount + 1;
	}

	/**
	 * @brief Checks to see if this string is currently using long string data.
	 *
	 * @return True if this string is currently using long string data, otherwise false.
	 */
	[[nodiscard]] bool IsUsingLongStringData() const;

	/**
	 * @brief Checks to see if this string is currently using short string data.
	 *
	 * @return True if this string is currently using short string data, otherwise false.
	 */
	[[nodiscard]] bool IsUsingShortStringData() const;

	/**
	 * @brief Makes a formatted string.
	 *
	 * @param format The format string.
	 * @param args The format arguments.
	 * @return The formatted string.
	 */
	[[nodiscard]] static FString MakeFormattedString(FStringView format, TSpan<Private::FStringFormatArgument> args);

	/**
	 * @brief Converts a EIgnoreCase value to a EStringComparison value.
	 *
	 * @param ignoreCase THe EIgnoreCase value.
	 * @return The EStringComparison value.
	 */
	[[nodiscard]] static constexpr EStringComparison ToStringComparison(const EIgnoreCase ignoreCase)
	{
		return ignoreCase == EIgnoreCase::Yes ? EStringComparison::OrdinalIgnoreCase : EStringComparison::Ordinal;
	}

	CharDataType m_CharData;
};

template<>
struct TIsZeroConstructible<FString> : TIsZeroConstructible<FString::CharDataType>
{
};

template<>
class TComparisonTraits<FString>
{
public:

	using Type = FString;

	static ECompareResult Compare(const FString& left, const FString& right)
	{
		return left.Compare(right);
	}

	static bool Equals(const FString& first, const FString& second)
	{
		return first.Equals(second);
	}
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FString);

/**
 * @brief Gets the hash code for the given string.
 *
 * @param value The string.
 * @return The hash code.
 */
inline uint64 GetHashCode(const FString& value)
{
	return GetHashCode(value.AsSpan());
}

/**
 * @brief Creates a string from a raw string literal.
 *
 * @param chars The raw string literal to create the string from.
 * @return The string.
 */
inline FString operator""_s(const FStringView::CharType* chars)
{
	const int32 numChars = FStringView::StringTraitsType::GetNullTerminatedCharCount(chars);
	return FString { chars, numChars };
}

/**
 * @brief Creates a string from a raw string literal.
 *
 * @param chars The raw string literal to create the string from.
 * @param numChars The number of characters in the string literal.
 * @return The string.
 */
inline FString operator""_s(const FStringView::CharType* chars, const size_t numChars)
{
	constexpr size_t MaximumCharCount = TNumericLimits<FStringView::SizeType>::MaxValue;
	UM_CONSTEXPR_ASSERT(numChars <= MaximumCharCount, "Too many characters for string view literal");
	return FString { chars, static_cast<FStringView::SizeType>(numChars) };
}
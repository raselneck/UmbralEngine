#pragma once

#include "Containers/Span.h"
#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Misc/EnumMacros.h"
#include "Templates/CharTraits.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/IsSame.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/Move.h"
#include "Templates/NumericLimits.h"
#include "Templates/StringTraits.h"
#include <stdexcept>

class FString;

template<typename T>
class TArray;

/**
 * @brief An enumeration of "ignore case" values.
 */
enum class EIgnoreCase : bool
{
	No = false,
	Yes = true
};

/**
 * @brief Specifies the culture, case, and sort rules to be used for string comparison.
 */
enum class EStringComparison : int32
{
	/**
	 * @brief Compare strings using culture-sensitive sort rules and the current culture.
	 */
	CurrentCulture,

	/**
	 * @brief Compare strings using culture-sensitive sort rules, the current culture, and ignoring the case of the strings being compared.
	 */
	CurrentCultureIgnoreCase,

	/**
	 * @brief Compare strings using culture-sensitive sort rules and the invariant culture.
	 */
	InvariantCulture,

	/**
	 * @brief Compare strings using culture-sensitive sort rules, the invariant culture, and ignoring the case of the strings being compared.
	 */
	InvariantCultureIgnoreCase,

	/**
	 * @brief Compare strings using ordinal (binary) sort rules.
	 */
	Ordinal,

	/**
	 * @brief Compare strings using ordinal (binary) sort rules and ignoring the case of the strings being compared.
	 */
	OrdinalIgnoreCase
};

/**
 * @brief Defines an enumeration of string split options
 */
enum class EStringSplitOptions : int32
{
	None               = 0,
	IgnoreEmptyEntries = (1 << 0),
	IgnoreCase         = (1 << 1)
};

ENUM_FLAG_OPERATORS(EStringSplitOptions)

namespace Private
{
	/**
	 * @brief Performs a naive, constexpr "to upper" operation on a character.
	 *
	 * @param ch The character.
	 * @return The character in uppercase form.
	 */
	[[nodiscard]] static constexpr char CharToUpper(const char ch)
	{
		if (ch >= 'a' && ch <= 'z')
		{
			return static_cast<char>('A' + (ch - 'a'));
		}

		return ch;
	}

	/**
	 * @brief Performs an ordinal comparison of character spans.
	 *
	 * @param left The left character span.
	 * @param right The right character span.
	 * @param ignoreCase Whether or not to ignore string casing.
	 * @return The compare result.
	 */
	[[nodiscard]] static constexpr ECompareResult OrdinalCompareCharSpans(const TSpan<const char> left, const TSpan<const char> right, const EIgnoreCase ignoreCase)
	{
		using CharType = char;
		using SizeType = typename TSpan<const CharType>::SizeType;

		const SizeType leftLength = left.Num();
		const SizeType rightLength = right.Num();
		const SizeType lengthToCompare = leftLength < rightLength ? leftLength : rightLength;

		for (SizeType idx = 0; idx < lengthToCompare; ++idx)
		{
			CharType leftChar = left[idx];
			CharType rightChar = right[idx];

			if (ignoreCase == EIgnoreCase::Yes)
			{
				leftChar = CharToUpper(leftChar);
				rightChar = CharToUpper(rightChar);
			}

			if (leftChar == rightChar)
			{
				continue;
			}
			else if (leftChar < rightChar)
			{
				return ECompareResult::LessThan;
			}
			else
			{
				return ECompareResult::GreaterThan;
			}
		}

		if (leftLength < rightLength)
		{
			return ECompareResult::LessThan;
		}
		else if (leftLength > rightLength)
		{
			return ECompareResult::GreaterThan;
		}

		return ECompareResult::Equals;
	}
}

/**
 * @brief Defines a read-only view into a string.
 */
class [[nodiscard]] FStringView final
{
public:

	using CharType = char;
	using CharTraitsType = TCharTraits<CharType>;
	using SizeType = int32;
	using StringTraitsType = TStringTraits<CharType>;
	using SpanType = TSpan<const CharType>;
	using IteratorType = const CharType*;

	/**
	 * @brief Sets default values for this string view's properties.
	 */
	constexpr FStringView() = default;

	/**
	 * @brief Sets default values for this string view's properties.
	 */
	constexpr FStringView(std::nullptr_t) { }

	/**
	 * @brief Sets the character array that this string view should represent.
	 *
	 * @param chars The null-terminated character array.
	 */
	constexpr explicit FStringView(const CharType* chars)
		: m_CharSpan { GetSpanFromChars(chars, StringTraitsType::GetNullTerminatedCharCount(chars)) }
	{
	}

	/**
	 * @brief Sets this string view's properties.
	 *
	 * @param chars The character array.
	 * @param numChars The number of characters.
	 */
	constexpr FStringView(const CharType* chars, const SizeType numChars)
		: m_CharSpan { GetSpanFromChars(chars, numChars) }
	{
	}

	/**
	 * @brief Sets this string view's properties.
	 *
	 * @param chars The span of characters.
	 */
	constexpr explicit FStringView(const TSpan<const CharType> chars)
		: m_CharSpan { chars }
	{
	}

	/**
	 * @brief Gets this string view as a span.
	 *
	 * @return This string view as a span.
	 */
	[[nodiscard]] constexpr SpanType AsSpan() const
	{
		return SpanType { GetChars(), Length() };
	}

	/**
	 * @brief Gets the character at the given index.
	 *
	 * @param index The index of the character to get.
	 * @return The character at the given index.
	 */
	[[nodiscard]] constexpr CharType At(const SizeType index) const
	{
		return m_CharSpan.At(index);
	}

	/**
	 * @brief Compares this string view to another string view.
	 *
	 * @param other The other string view.
	 * @param ignoreCase Whether case should be ignored when comparing the string views.
	 * @return The result of the comparison.
	 */
	[[nodiscard]] constexpr ECompareResult Compare(const FStringView other, const EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		if (std::is_constant_evaluated())
		{
			return Private::OrdinalCompareCharSpans(AsSpan(), other.AsSpan(), ignoreCase);
		}
		else
		{
			// We default to ordinal string comparison because it is faster, and culture-sensitive comparisons should be opt-in
			const EStringComparison comparison = ignoreCase == EIgnoreCase::No ? EStringComparison::Ordinal : EStringComparison::OrdinalIgnoreCase;
			return Compare(other, comparison);
		}
	}

	/**
	 * @brief Compares this string view to another string view.
	 *
	 * @param other The other string view.
	 * @param comparison The string comparison type.
	 * @return The result of the comparison.
	 */
	[[nodiscard]] ECompareResult Compare(FStringView other, EStringComparison comparison) const;

	// TODO Overloads for other functions that take EStringComparison instead of EIgnoreCase

	/**
	 * @brief Checks to see if this string view contains the given character.
	 *
	 * @param value The character.
	 * @return True if this string view contains the given character, otherwise false.
	 */
	[[nodiscard]] bool Contains(const CharType value) const
	{
		return IndexOf(value) != INDEX_NONE;
	}

	/**
	 * @brief Checks to see if this string view contains the given substring.
	 *
	 * @param value The substring.
	 * @param ignoreCase Whether or not to ignore the string's casing.
	 * @return True if this string view contains the given character, otherwise false.
	 */
	[[nodiscard]] bool Contains(const FStringView value, const EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return IndexOf(value, ignoreCase == EIgnoreCase::Yes ? EStringComparison::OrdinalIgnoreCase : EStringComparison::Ordinal) != INDEX_NONE;
	}

	/**
	 * @brief Checks to see if this string view ends with the given other string view.
	 *
	 * @param other The other string view.
	 * @return True if this string view ends with \p other, otherwise false.
	 */
	[[nodiscard]] bool EndsWith(FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const;

	/**
	 * @brief Checks to see if this string view ends with the given character.
	 *
	 * @param character The character.
	 * @return True if this string view ends with \p other, otherwise false.
	 */
	[[nodiscard]] bool EndsWith(const CharType character, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return EndsWith(FStringView { &character, 1 }, ignoreCase);
	}

	/**
	 * @brief Checks to see if this string view equals another.
	 *
	 * @param other The other string view.
	 * @param ignoreCase Whether to ignore casing when checking for equality.
	 * @return True if this string view equals \p other, otherwise false.
	 */
	[[nodiscard]] bool Equals(const FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return Compare(other, ignoreCase) == ECompareResult::Equals;
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given character.
	 *
	 * @param value The character to find.
	 * @return The zero-based index of the first occurrence of the given character.
	 */
	[[nodiscard]] SizeType IndexOf(const CharType value) const
	{
		constexpr SizeType startIndex = 0;
		const SizeType count = Length();
		return IndexOf(value, startIndex, count);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given character.
	 *
	 * @param value The character to find.
	 * @param startIndex The index to start searching at.
	 * @return The zero-based index of the first occurrence of the given character.
	 */
	[[nodiscard]] SizeType IndexOf(const CharType value, const SizeType startIndex) const
	{
		const SizeType count = Length() - startIndex;
		return IndexOf(value, startIndex, count);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given character.
	 *
	 * @param value The character to find.
	 * @param startIndex The index to start searching at.
	 * @param count The number of character positions to examine.
	 * @return The zero-based index of the first occurrence of the given character.
	 */
	[[nodiscard]] SizeType IndexOf(CharType value, SizeType startIndex, SizeType count) const;

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value) const
	{
		constexpr SizeType startIndex = 0;
		const SizeType count = Length();
		constexpr EStringComparison comparison = EStringComparison::Ordinal;
		return IndexOf(value, startIndex, count, comparison);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @param startIndex The index to start searching at.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const SizeType startIndex) const
	{
		const SizeType count = Length() - startIndex;
		constexpr EStringComparison comparison = EStringComparison::Ordinal;
		return IndexOf(value, startIndex, count, comparison);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @param comparison The string comparison rule to use during search.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const EStringComparison comparison) const
	{
		constexpr SizeType startIndex = 0;
		const SizeType count = Length();
		return IndexOf(value, startIndex, count, comparison);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @param startIndex The index to start searching at.
	 * @param count The number of character positions to examine.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const SizeType startIndex, const SizeType count) const
	{
		constexpr EStringComparison comparison = EStringComparison::Ordinal;
		return IndexOf(value, startIndex, count, comparison);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @param startIndex The index to start searching at.
	 * @param comparison The string comparison rule to use during search.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(const FStringView value, const SizeType startIndex, const EStringComparison comparison) const
	{
		const SizeType count = Length() - startIndex;
		return IndexOf(value, startIndex, count, comparison);
	}

	/**
	 * @brief Gets the zero-based index of the first occurrence of the given string.
	 *
	 * @param value The string to find.
	 * @param startIndex The index to start searching at.
	 * @param count The number of character positions to examine.
	 * @param comparison The string comparison rule to use during search.
	 * @return The zero-based index of the first occurrence of the given string.
	 */
	[[nodiscard]] SizeType IndexOf(FStringView value, SizeType startIndex, SizeType count, EStringComparison comparison) const;

	/**
	 * @brief Checks to see if this string view is empty.
	 *
	 * @return True if this string view is empty, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsEmpty() const
	{
		return m_CharSpan.IsEmpty();
	}

	/**
	 * @brief Checks to see if a given index is valid for this string view.
	 *
	 * @param index The index to check for.
	 * @return True if \p index is a valid index, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsValidIndex(const SizeType index) const
	{
		return m_CharSpan.IsValidIndex(index);
	}

	/**
	 * @brief Gets this string view's characters.
	 *
	 * @return This string view's characters.
	 */
	[[nodiscard]] constexpr const CharType* GetChars() const
	{
		return m_CharSpan.GetData();
	}

	/**
	 * @brief Gets the index of the last time the given character appears in this string view.
	 *
	 * @param character The character to search for.
	 * @return The index of the last time the given character appears in this string view, or INDEX_NONE if the character was not found.
	 */
	[[nodiscard]] SizeType LastIndexOf(const CharType character) const
	{
		return m_CharSpan.LastIndexOf(character);
	}

	/**
	 * @brief Gets the last index of a character based on the given predicate.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate.
	 * @return The last index of a character matching \p predicate, or INDEX_NONE if none were found.
	 */
	template<typename PredicateType>
	[[nodiscard]] SizeType LastIndexOfByPredicate(PredicateType predicate) const
	{
		return m_CharSpan.template LastIndexOfByPredicate<PredicateType>(MoveTemp(predicate));
	}

	/**
	 * @brief Gets a string view of a number of the leftmost characters.
	 *
	 * @param numChars The number of characters to get.
	 * @return A string view of the leftmost \p numChars characters.
	 */
	[[nodiscard]] constexpr FStringView Left(const SizeType numChars) const
	{
		return Substring(0, numChars);
	}

	/**
	 * @brief Gets the number of characters in this string view.
	 *
	 * @return The number of characters in this string view.
	 */
	[[nodiscard]] constexpr SizeType Length() const
	{
		return m_CharSpan.Num();
	}

	/**
	 * @brief Returns this string view with the leftmost \p numChars characters removed.
	 *
	 * @param numChars The number of characters to remove.
	 * @return This string view with the leftmost \p numChars characters removed.
	 */
	[[nodiscard]] constexpr FStringView RemoveLeft(const SizeType numChars) const
	{
		if (numChars < 0 || numChars >= Length())
		{
			return {};
		}

		return FStringView { GetChars() + numChars, Length() - numChars };
	}

	/**
	 * @brief Returns this string view with the rightmost \p numChars characters removed.
	 *
	 * @param numChars The number of characters to remove.
	 * @return This string view with the rightmost \p numChars characters removed.
	 */
	[[nodiscard]] constexpr FStringView RemoveRight(const SizeType numChars) const
	{
		if (numChars < 0 || numChars >= Length())
		{
			return {};
		}

		return FStringView { GetChars(), Length() - numChars };
	}

	/**
	 * @brief Gets a string view of a number of the rightmost characters.
	 *
	 * @param numChars The number of characters to get.
	 * @return A string view of the rightmost \p numChars characters.
	 */
	[[nodiscard]] constexpr FStringView Right(const SizeType numChars) const
	{
		const SizeType index = Length() - numChars;
		return Substring(index, numChars);
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
	void SplitByChars(FStringView chars, EStringSplitOptions options, TArray<FStringView>& result) const;

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
	[[nodiscard]] TArray<FStringView> SplitByChars(FStringView chars, EStringSplitOptions options = EStringSplitOptions::None) const;

	/**
	 * @brief Splits this string by a substring into an array of string views.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @param result The result string view array.
	 */
	void SplitByString(FStringView substring, EStringSplitOptions options, TArray<FStringView>& result) const;

	/**
	 * @brief Splits this string by a substring into an array of strings.
	 *
	 * @param substring The substring to search for when splitting.
	 * @param options The split options.
	 * @return The split string views.
	 */
	[[nodiscard]] TArray<FStringView> SplitByString(FStringView substring, EStringSplitOptions options = EStringSplitOptions::None) const;

	/**
	 * @brief Checks to see if this string view starts with the given other string view.
	 *
	 * @param other The other string view.
	 * @return True if this string view starts with \p other, otherwise false.
	 */
	[[nodiscard]] bool StartsWith(FStringView other, EIgnoreCase ignoreCase = EIgnoreCase::No) const;

	/**
	 * @brief Checks to see if this string view starts with the given character.
	 *
	 * @param character The character.
	 * @return True if this string view starts with \p other, otherwise false.
	 */
	[[nodiscard]] bool StartsWith(const CharType character, EIgnoreCase ignoreCase = EIgnoreCase::No) const
	{
		return StartsWith(FStringView { &character, 1 }, ignoreCase);
	}

	/**
	 * @brief Gets a substring of this string view.
	 *
	 * @param index The substring start index.
	 * @param length The substring length.
	 * @return A substring of this string view.
	 */
	[[nodiscard]] constexpr FStringView Substring(const SizeType index, const SizeType length) const
	{
		UM_CONSTEXPR_ASSERT(index >= 0, "Substring start index must be positive");
		UM_CONSTEXPR_ASSERT(index < Length(), "Substring start index must be less than this string view's length");
		UM_CONSTEXPR_ASSERT(length >= 0, "Substring length must be positive");
		UM_CONSTEXPR_ASSERT(index <= Length() - length, "Substring length is out of bounds");

		if (length == 0)
		{
			return {};
		}
		if (index == 0 && length == Length())
		{
			return *this;
		}

		return FStringView { GetChars() + index, length };
	}

	/**
	 * @brief Gets the character at the given index.
	 *
	 * @param index The index of the character to get.
	 * @return The character at \p index.
	 */
	[[nodiscard]] constexpr CharType operator[](const SizeType index) const
	{
		return m_CharSpan[index];
	}

	/**
	 * @brief Gets the characters pointed to by this string view.
	 *
	 * @return The characters pointed to by this string view.
	 */
	[[nodiscard]] constexpr const CharType* operator*() const
	{
		return GetChars();
	}

	/**
	 * @brief Implicitly converts this string view to a character span.
	 *
	 * @return This string view as a span.
	 */
	[[nodiscard]] constexpr operator SpanType() const
	{
		return AsSpan();
	}

	/**
	 * @brief Writes this string view to the given stream.
	 *
	 * @tparam StreamType The stream's type.
	 * @param stream The stream.
	 * @param value The string view value to write.
	 * @return The stream.
	 */
	template<typename StreamType>
	friend StreamType& operator<<(StreamType& stream, const FStringView& value)
	{
		stream.write(value.GetChars(), value.Length());
		return stream;
	}

	// BEGIN STD COMPATIBILITY
	[[nodiscard]] IteratorType begin()  const { return m_CharSpan.begin(); }
	[[nodiscard]] IteratorType cbegin() const { return m_CharSpan.begin(); }
	[[nodiscard]] IteratorType end()    const { return m_CharSpan.end(); }
	[[nodiscard]] IteratorType cend()   const { return m_CharSpan.end(); }
	[[nodiscard]] constexpr bool operator< (const FStringView other) const { return Compare(other) == ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator<=(const FStringView other) const { return Compare(other) != ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator> (const FStringView other) const { return Compare(other) == ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator>=(const FStringView other) const { return Compare(other) != ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator==(const FStringView other) const { return Compare(other) == ECompareResult::Equals; }
	[[nodiscard]] constexpr bool operator!=(const FStringView other) const { return Compare(other) != ECompareResult::Equals; }
	// END STD COMPATIBILITY

private:

	/**
	 * @brief Gets a character span from the given raw character pointer.
	 *
	 * @param chars The character pointer.
	 * @param numChars The number of characters.
	 * @return The character span.
	 */
	static constexpr SpanType GetSpanFromChars(const CharType* chars, const SizeType numChars)
	{
		UM_CONSTEXPR_ASSERT(numChars >= 0, "Attempting to create string view from negative number of characters");

		if (chars == nullptr || numChars == 0)
		{
			return SpanType {};
		}

		return SpanType { chars, numChars };
	}

	SpanType m_CharSpan;
};

template<>
struct TIsZeroConstructible<FStringView> : FTrueType
{
};

template<>
class TComparisonTraits<FStringView>
{
public:

	using Type = FStringView;

	static ECompareResult Compare(const FStringView& left, const FStringView& right)
	{
		return left.Compare(right);
	}

	static bool Equals(const FStringView& first, const FStringView& second)
	{
		return first.Equals(second);
	}
};

/**
 * @brief Gets the hash code for the given string.
 *
 * @param value The string.
 * @return The hash code.
 */
inline uint64 GetHashCode(const FStringView& value)
{
	return GetHashCode(value.AsSpan());
}

/**
 * @brief Creates a string view from a raw string literal.
 *
 * @param chars The raw string literal to create the string view from.
 * @return The string view.
 */
constexpr FStringView operator""_sv(const FStringView::CharType* chars)
{
	const int32 numChars = FStringView::StringTraitsType::GetNullTerminatedCharCount(chars);
	return FStringView { chars, numChars };
}

/**
 * @brief Creates a string view from a raw string literal.
 *
 * @param chars The raw string literal to create the string view from.
 * @param numChars The number of characters in the string literal.
 * @return The string view.
 */
constexpr FStringView operator""_sv(const FStringView::CharType* chars, const size_t numChars)
{
	constexpr size_t MaximumCharCount = TNumericLimits<FStringView::SizeType>::MaxValue;
	UM_CONSTEXPR_ASSERT(numChars <= MaximumCharCount, "Too many characters for string view literal");
	return FStringView { chars, static_cast<FStringView::SizeType>(numChars) };
}
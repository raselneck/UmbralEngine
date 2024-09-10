#pragma once

#include "Memory/UniquePtr.h"
#include "Templates/CharTraits.h"
#include "Templates/ComparisonTraits.h"

/**
 * @brief Defines a platform-agnostic way to access C-related string functions.
 */
class FCString final
{
	struct FCStringDeleter
	{
		void Delete(char* chars);
	};

public:

	using CharType = char;
	using SizeType = int32;
	using CharTraits = TCharTraits<CharType>;
	using StringCompareFunction = ECompareResult(*)(const CharType*, const CharType*, SizeType);

	/**
	 * @brief Sets default values for this C-string's properties.
	 */
	constexpr FCString() = default;

	/**
	 * @brief Copies a raw C string.
	 *
	 * @param chars The raw C string to copy.
	 */
	explicit FCString(const char* chars);

	/**
	 * @brief Copies another C string.
	 *
	 * @param other The C string to copy.
	 */
	FCString(const FCString& other);

	/**
	 * @brief Assumes ownership of another C string's resources.
	 *
	 * @param other The other C string.
	 */
	FCString(FCString&& other) noexcept;

	/**
	 * @brief Frees up any memory used by this C string.
	 */
	~FCString() = default;

	/**
	 * @brief Gets this string's characters.
	 *
	 * @return This string's characters.
	 */
	[[nodiscard]] const char* GetChars() const
	{
		return m_Chars.Get();
	}

	/**
	 * @brief Gets this string's characters.
	 *
	 * @return This string's characters.
	 */
	[[nodiscard]] char* GetChars()
	{
		return m_Chars.Get();
	}

	/**
	 * @brief Gets a value indicating whether a character is an alphabetic character.
	 *
	 * @param ch The character.
	 * @return True if \p ch is an alphabetic character, otherwise false.
	 */
	[[nodiscard]] static bool IsAlpha(CharType ch);

	/**
	 * @brief Gets a value indicating whether a character is an alphabetic or numeric character.
	 *
	 * @param ch The character.
	 * @return True if \p ch is an alphabetic or numeric character, otherwise false.
	 */
	[[nodiscard]] static bool IsAlphaNumeric(CharType ch);

	/**
	 * @brief Gets a value indicating whether a character is a numeric character.
	 *
	 * @param ch The character.
	 * @return True if \p ch is a numeric character, otherwise false.
	 */
	[[nodiscard]] static bool IsNumeric(CharType ch);

	/**
	 * @brief Gets the length of this C string.
	 *
	 * @return The length of this C string.
	 */
	[[nodiscard]] SizeType Length() const
	{
		return CharTraits::GetNullTerminatedLength(m_Chars.Get());
	}

	/**
	 * @brief Compares two strings, ignoring case.
	 *
	 * @param first The first string.
	 * @param second The second string.
	 * @param numChars The number of characters.
	 * @return The comparison result.
	 */
	static ECompareResult StrCaseCmp(const CharType* first, const CharType* second, SizeType numChars);

	/**
	 * @brief Compares two strings.
	 *
	 * @param first The first string.
	 * @param second The second string.
	 * @param numChars The number of characters.
	 * @return The comparison result.
	 */
	static ECompareResult StrCmp(const CharType* first, const CharType* second, SizeType numChars);

	/**
	 * @brief Finds the index of a character in a string, ignoring case.
	 *
	 * @param haystack The string.
	 * @param haystackLength The length of the string being searched.
	 * @param needle The character.
	 * @return The index of the character in the string, or INDEX_NONE if it was not found.
	 */
	static SizeType StrCaseChr(const CharType* haystack, SizeType haystackLength, const CharType needle);

	/**
	 * @brief Finds the index of a character in a string.
	 *
	 * @param haystack The string.
	 * @param haystackLength The length of the string being searched.
	 * @param needle The character.
	 * @return The index of the character in the string, or INDEX_NONE if it was not found.
	 */
	static SizeType StrChr(const CharType* haystack, SizeType haystackLength, const CharType needle);

	/**
	 * @brief Finds the index of a substring in a string, ignoring case.
	 *
	 * @param haystack The string being searched.
	 * @param haystackLength The length of the string being searched.
	 * @param needle The string to search for.
	 * @param needleLength The length of the string to search for.
	 * @return The index of the character in the string, or INDEX_NONE if it was not found.
	 */
	static SizeType StrCaseStr(const CharType* haystack, SizeType haystackLength, const CharType* needle, SizeType needleLength);

	/**
	 * @brief Checks to see if two strings are considered equal, ignoring case.
	 *
	 * @param first The first string.
	 * @param second The second string.
	 * @param numChars The number of characters to compare.
	 * @return True if the strings are equal, otherwise false.
	 */
	static bool StrCaseEqual(const CharType* first, const CharType* second, const SizeType numChars)
	{
		return StrCaseCmp(first, second, numChars) == ECompareResult::Equals;
	}

	/**
	 * @brief Checks to see if two strings are considered equal.
	 *
	 * @param first The first string.
	 * @param second The second string.
	 * @param numChars The number of characters to compare.
	 * @return True if the strings are equal, otherwise false.
	 */
	static bool StrEqual(const CharType* first, const CharType* second, const SizeType numChars)
	{
		return StrCmp(first, second, numChars) == ECompareResult::Equals;
	}

	/**
	 * @brief Finds the index of a substring in a string.
	 *
	 * @param haystack The string being searched.
	 * @param haystackLength The length of the string being searched.
	 * @param needle The string to search for.
	 * @param needleLength The length of the string to search for.
	 * @return The index of the character in the string, or INDEX_NONE if it was not found.
	 */
	static SizeType StrStr(const CharType* haystack, SizeType haystackLength, const CharType* needle, SizeType needleLength);

	/**
	 * @brief Converts the given character to its lowercase variant.
	 *
	 * @param ch The character.
	 * @return \p ch as its lowercase variant.
	 */
	static CharType ToLower(CharType ch);

	/**
	 * @brief Converts the given character to its uppercase variant.
	 *
	 * @param ch The character.
	 * @return \p ch as its uppercase variant.
	 */
	static CharType ToUpper(CharType ch);

	/**
	 * @brief Copies another C string.
	 *
	 * @param other The C string to copy.
	 * @return This C string.
	 */
	FCString& operator=(const FCString& other);

	/**
	 * @brief Assumes ownership of another C string's resources.
	 *
	 * @param other The other C string.
	 * @return This C string.
	 */
	FCString& operator=(FCString&& other) noexcept;

private:

	/**
	 * @brief Copies a character array.
	 *
	 * @param chars The character array to copy.
	 * @return The copied character array.
	 */
	static TUniquePtr<char, FCStringDeleter> CopyCharArray(const char* chars);

	TUniquePtr<char, FCStringDeleter> m_Chars;
};

static_assert(sizeof(FCString) == sizeof(char*), "CString must be the same size as a C string!");
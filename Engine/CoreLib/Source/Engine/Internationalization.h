#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "Misc/Badge.h"

/**
 * @brief Defines an interface for internationalization capabilities.
 */
class FInternationalization
{
public:

	using CharType = typename FStringView::CharType;
	using CharSpanType = TSpan<const CharType>;

	/**
	 * @brief Compares two strings.
	 *
	 * @param first The first string.
	 * @param second The second string.
	 * @param comparison The string comparison type.
	 * @return The comparison result for the two strings.
	 */
	static TErrorOr<ECompareResult> CompareStrings(CharSpanType first, CharSpanType second, EStringComparison comparison);

	/**
	 * @brief Converts a string to its lowercase equivalent using the current system locale.
	 *
	 * @param string The string to convert.
	 * @return The array of characters representing the new string.
	 */
	static TArray<char> ConvertStringToLower(CharSpanType string);

	/**
	 * @brief Converts a string to its uppercase equivalent using the current system locale.
	 *
	 * @param string The string to convert.
	 * @return The array of characters representing the new string.
	 */
	static TArray<char> ConvertStringToUpper(CharSpanType string);

	/**
	 * @brief Gets the current locale.
	 *
	 * @return The current locale.
	 */
	static FStringView GetLocale();

	/**
	 * @brief Attempts to initialize the internationalization subsystem.
	 *
	 * @return True if the initialization was successful, otherwise false.
	 */
	static TErrorOr<void> Initialize();

	/**
	 * @brief Sets the current locale.
	 *
	 * @param locale The new locale.
	 */
	static TErrorOr<void> SetLocale(FStringView locale);

	/**
	 * @brief Shuts down the internationalization subsystem.
	 */
	static void Shutdown();
};
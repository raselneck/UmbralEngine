#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"

/**
 * @brief Defines a way to match regular expression patterns.
 */
class FRegex final
{
public:

	/**
	 * @brief Gets the first substring of text matching a given regex pattern.
	 *
	 * @param pattern The regex pattern.
	 * @param text The text to match in.
	 * @return The first substring of \p text matching \p pattern. Will return an empty string view if there is no match.
	 */
	static FStringView Match(const FString& pattern, const FString& text);

	/**
	 * @brief Gets all matching substrings of text matching a given regex pattern.
	 *
	 * @param pattern The regex pattern.
	 * @param text The text to match in.
	 * @return All matching substrings of \p text matching \p pattern.
	 */
	static TArray<FStringView> MatchAll(const FString& pattern, const FString& text);

	/**
	 * @brief Checks to see if a regex pattern matches a string.
	 *
	 * @param pattern The regex pattern.
	 * @param text The text to match.
	 * @return True if \p pattern matches \p text.
	 */
	static bool Matches(const FString& pattern, const FString& text);
};
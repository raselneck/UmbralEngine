#pragma once

#include "Engine/Error.h"
#include "JSON/JsonValue.h"

namespace JSON
{
	/**
	 * @brief Attempts to parse a JSON value from a file.
	 *
	 * @param filePath The path to the file.
	 * @return The parsed JSON value, or the error that was encountered.
	 */
	TErrorOr<FJsonValue> ParseFile(FStringView filePath);

	/**
	 * @brief Attempts to parse a JSON value from a string.
	 *
	 * @param text The string to parse.
	 * @return The parsed JSON value, or the error that was encountered.
	 */
	TErrorOr<FJsonValue> ParseString(FStringView text);

	/**
	 * @brief Attempts to parse a JSON value from a string.
	 *
	 * @param text The string to parse.
	 * @return The parsed JSON value, or the error that was encountered.
	 */
	TErrorOr<FJsonValue> ParseString(const FString& text);
}
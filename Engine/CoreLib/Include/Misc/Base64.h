#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Engine/Error.h"

namespace Base64
{
	/**
	 * @brief Encodes the given span of bytes into base-64.
	 *
	 * @param bytes The bytes to encode.
	 * @return \p bytes encoded as a base-64 string.
	 */
	[[nodiscard]] FString Encode(TSpan<const uint8> bytes);

	/**
	 * @brief Decodes a base-64 string into an array of bytes.
	 *
	 * @param chars The base-64 string to decode.
	 * @return \p chars decoded into an array of bytes.
	 */
	[[nodiscard]] TErrorOr<TArray<uint8>> Decode(FStringView chars);

	/**
	 * @brief Decodes a base-64 string into an array of bytes.
	 *
	 * @param chars The base-64 string to decode.
	 * @param result The array to populate with the decoded bytes.
	 * @return True if \p chars was successfully decoded, otherwise false.
	 */
	[[nodiscard]] bool Decode(FStringView chars, TArray<uint8>& result);

	/**
	 * @brief Decodes a base-64 string into an array of bytes.
	 *
	 * @param chars The base-64 string to decode.
	 * @param result The string to populate with the decoded result.
	 * @return True if \p chars was successfully decoded, otherwise false.
	 */
	[[nodiscard]] bool Decode(FStringView chars, FString& result);
}

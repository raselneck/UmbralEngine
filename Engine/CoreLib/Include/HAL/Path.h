#pragma once

#include "Containers/Array.h"
#include "Containers/StaticArray.h"
#include "Containers/Span.h"
#include "Containers/String.h"
#include "Containers/StringOrStringView.h"
#include "Containers/StringView.h"

/**
 * @brief Defines a way to interact with and manipulate paths.
 */
class FPath
{
public:

	/**
	 * @brief Gets the absolute path to the given path.
	 *
	 * @param path The path.
	 * @return The absolute path.
	 */
	[[nodiscard]] static FString GetAbsolutePath(FStringView path);

	/**
	 * @brief Gets the absolute path to the given path.
	 *
	 * @param path The path.
	 * @return The absolute path.
	 */
	[[nodiscard]] static FString GetAbsolutePath(const FString& path);

	/**
	 * @brief Gets the name of the file at the given path, without the extension.
	 *
	 * @param path The path.
	 * @returns The file name, without the extension.
	 */
	[[nodiscard]] static FString GetBaseFileName(FStringView path);

	/**
	 * @brief Gets the name of the file at the given path, without the extension.
	 *
	 * @param path The path.
	 * @returns The file name, without the extension.
	 */
	[[nodiscard]] static FStringView GetBaseFileNameAsView(FStringView path);

	/**
	 * @brief Gets the name of the directory that the given path resides in.
	 *
	 * @param path The path.
	 * @returns The parent directory.
	 */
	[[nodiscard]] static FString GetDirectoryName(FStringView path);

	/**
	 * @brief Gets the name of the directory that the given path resides in.
	 *
	 * @param path The path.
	 * @returns The parent directory.
	 */
	[[nodiscard]] static FStringView GetDirectoryNameAsView(FStringView path);

	/**
	 * @brief Gets the extension for the given file path without the dot.
	 *
	 * @param path The file path.
	 * @return The file's extension without the dot, or an empty string if one could not be determined.
	 */
	[[nodiscard]] static FStringView GetExtension(FStringView path);

	/**
	 * @brief Gets the current executable's name.
	 *
	 * @return The current executable's name.
	 */
	[[nodiscard]] static FString GetExecutableName();

	/**
	 * @brief Gets the name of the file at the given path, with the extension.
	 *
	 * @param path The path.
	 * @returns The file name, with the extension.
	 */
	[[nodiscard]] static FString GetFileName(FStringView path);

	/**
	 * @brief Gets the name of the file at the given path, with the extension.
	 *
	 * @param path The path.
	 * @returns The file name, with the extension.
	 */
	[[nodiscard]] static FStringView GetFileNameAsView(FStringView path);

	/**
	 * @brief Checks to see if the given path contains any illegal characters.
	 *
	 * @param path The path to check.
	 * @return True if \p path contains illegal characters, otherwise false.
	 */
	[[nodiscard]] static bool HasIllegalCharacter(FStringView path);

	/**
	 * @brief Checks to see if the given path is absolute.
	 *
	 * @param path The path.
	 * @return True if \p path is an absolute path, otherwise false.
	 */
	[[nodiscard]] static bool IsAbsolute(FStringView path);

	/**
	 * @brief Checks to see if the file indicated by \p path is a directory.
	 *
	 * @param path The path to the file.
	 * @return True if the file indicated by \p path is a directory, otherwise false.
	 */
	[[nodiscard]] static bool IsDirectory(FStringView path);

	/**
	 * @brief Checks to see if the file indicated by \p path is a directory.
	 *
	 * @param path The path to the file.
	 * @return True if the file indicated by \p path is a directory, otherwise false.
	 */
	[[nodiscard]] static bool IsDirectory(const FString& path);

	/**
	 * @brief Checks to see if a character is a directory separator.
	 *
	 * @param ch The character.
	 * @return True if \p ch is a directory separator, otherwise false.
	 */
	static constexpr bool IsDirectorySeparator(const char ch)
	{
		return ch == '\\' || ch == '/';
	}

	/**
	 * @brief Checks to see if the file indicated by \p path is a file.
	 *
	 * @param path The path to the file.
	 * @return True if the file indicated by \p path is a file, otherwise false.
	 */
	[[nodiscard]] static bool IsFile(FStringView path);

	/**
	 * @brief Checks to see if the file indicated by \p path is a file.
	 *
	 * @param path The path to the file.
	 * @return True if the file indicated by \p path is a file, otherwise false.
	 */
	[[nodiscard]] static bool IsFile(const FString& path);

	/**
	 * @brief Checks to see if a character is an illegal path character.
	 *
	 * @param ch The character.
	 * @return True if \p ch is an illegal path character, otherwise false.
	 */
	static constexpr bool IsIllegalPathCharacter(const char ch)
	{
		if (static_cast<uint8>(ch) <= 31)
		{
			return true;
		}

		return ch == '\"' || ch == '<' || ch == '>' || ch == '|';
	}

	/**
	 * @brief Checks to see if the given path is relative.
	 *
	 * @param path The path.
	 * @return True if \p path is a relative path, otherwise false.
	 */
	[[nodiscard]] static bool IsRelative(FStringView path);

	/**
	 * @brief Joins path parts together.
	 *
	 * @param pathParts The path parts.
	 * @returns The joined path.
	 */
	[[nodiscard]] static FString Join(TSpan<const FStringView> pathParts);

	/**
	 * @brief Joins path parts together.
	 *
	 * @param pathParts The path parts.
	 * @returns The joined path.
	 */
	[[nodiscard]] static FString Join(TSpan<const FString> pathParts);

	/**
	 * @brief Joins path parts together.
	 *
	 * @param pathParts The path parts.
	 * @returns The joined path.
	 */
	[[nodiscard]] static FString Join(TSpan<const FStringOrStringView> pathParts);

	/**
	 * @brief Joins two paths together.
	 *
	 * @param first The first path.
	 * @param second The second path.
	 * @returns The joined path.
	 */
	[[nodiscard]] static FString Join(FStringView first, FStringView second)
	{
		TStaticArray<FStringView, 2> pathParts {{ first, second }};
		return Join(pathParts.AsSpan());
	}

	/**
	 * @brief Joins three paths together.
	 *
	 * @param first The first path.
	 * @param second The second path.
	 * @param third The third path.
	 * @returns The joined path.
	 */
	[[nodiscard]] static FString Join(FStringView first, FStringView second, FStringView third)
	{
		TStaticArray<FStringView, 3> pathParts {{ first, second, third }};
		return Join(pathParts.AsSpan());
	}

	/**
	 * @brief Joins path parts together.
	 *
	 * @tparam ArgTypes The argument types. Must all be string-like.
	 * @param firstPath The first path part.
	 * @param otherParts The remaining path parts.
	 * @return The joined path.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] static FString Join(FStringView firstPath, ArgTypes... otherParts)
	{
		TArray<FStringOrStringView> pathParts;
		pathParts.Reserve(1 + sizeof...(ArgTypes));

		(void)pathParts.Emplace(firstPath);

		TVariadicForEach<ArgTypes...>::Visit([&](auto value)
		{
			(void)pathParts.Emplace(value);
			return EIterationDecision::Continue;
		}, Forward<ArgTypes>(otherParts)...);

		return Join(pathParts.AsSpan());
	}

	/**
	 * @brief Normalizes the separators in the given path.
	 *
	 * @param path The path.
	 * @return The normalized path.
	 */
	[[nodiscard]] static FString Normalize(FStringView path);

	/**
	 * @brief Normalizes the separators in the given path.
	 *
	 * @param path The path.
	 * @return The normalized path.
	 */
	[[nodiscard]] static FString Normalize(const FString& path);

	/**
	 * @brief Normalizes the separators in the given path.
	 *
	 * @param path The path.
	 * @return The normalized path.
	 */
	[[maybe_unused]] static FString& Normalize(FString& path);

	/**
	 * @brief Resolves a path. This will clear out any relative parts, such as "..".
	 *
	 * @param path The path to resolve.
	 * @returns The resolved path.
	 */
	[[nodiscard]] static FString ResolvePath(FStringView path);
};
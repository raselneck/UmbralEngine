#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "HAL/Path.h"

/**
 * @brief Defines a way to interact with a system's directories.
 */
class FDirectory final
{
public:

	/**
	 * @brief Checks to see if the given directory exists.
	 *
	 * @param path The path to the directory.
	 * @return True if the directory exists, otherwise false.
	 */
	[[nodiscard]] static bool Exists(FStringView path);

	/**
	 * @brief Checks to see if the given directory exists.
	 *
	 * @param path The path to the directory.
	 * @return True if the directory exists, otherwise false.
	 */
	[[nodiscard]] static bool Exists(const FString& path);

	/**
	 * @brief Gets the absolute path to the content directory.
	 *
	 * @return The absolute path to the content directory.
	 */
	[[nodiscard]] static FString GetContentDir();

	/**
	 * @brief Gets the path to a content file.
	 *
	 * @param fileName The file name.
	 * @return The path to the file.
	 */
	[[nodiscard]] static FString GetContentFilePath(FStringView fileName);

	/**
	 * @brief Gets the path to a content file.
	 *
	 * @param fileName The file name.
	 * @return The path to the file.
	 */
	[[nodiscard]] static FString GetContentFilePath(const FString& fileName);

	/**
	 * @brief Gets the path to a content file.
	 *
	 * @param fileName The file name.
	 * @return The path to the file.
	 */
	[[nodiscard]] static FString GetContentFilePath(FString&& fileName);

	/**
	 * @brief Gets the path to a content file after joining together multiple path parts together.
	 *
	 * @tparam StringLikeTypes The string-like types to join.
	 * @param fileNameParts The file name parts.
	 * @return The path to the content file.
	 */
	template<typename... StringLikeTypes>
	[[nodiscard]] static FString GetContentFilePath(StringLikeTypes&&... fileNameParts)
	{
		static_assert(sizeof...(fileNameParts) > 1, "Must provide at least two file name parts to be joined");
		return GetContentFilePath(FPath::Join(Forward<StringLikeTypes>(fileNameParts)...));
	}

	/**
	 * @brief Gets the directory where the executable is located.
	 *
	 * @return The directory where the executable is located.
	 */
	[[nodiscard]] static FString GetExecutableDir();

	/**
	 * @brief Attempts to get all of the files in a directory.
	 *
	 * @param path The path to the directory to query.
	 * @param recursive True to search for files recursively, false to only search the top-level directory.
	 * @param files The file array to populate.
	 * @return An error if one was encountered.
	 */
	[[nodiscard]] static TErrorOr<void> GetFiles(FStringView path, bool recursive, TArray<FString>& files);

	/**
	 * @brief Attempts to get all of the files in a directory.
	 *
	 * @param path The path to the directory to query.
	 * @param recursive True to search for files recursively, false to only search the top-level directory.
	 * @param files The file array to populate.
	 * @return An error if one was encountered.
	 */
	[[nodiscard]] static TErrorOr<void> GetFiles(const FString& path, bool recursive, TArray<FString>& files);

	/**
	 * @brief Gets the absolute path to the current working directory.
	 *
	 * @return The absolute path to the current working directory.
	 */
	[[nodiscard]] static FString GetWorkingDir();
};
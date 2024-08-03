#pragma once

#include "HAL/File.h"

/**
 * @brief Defines native implementations for a Linux file system.
 */
class FLinuxFileSystem
{
public:

	/**
	 * @brief Attempts to delete the file pointed to the given path.
	 *
	 * @param filePath The path to the file.
	 * @return The error that was encountered, if any.
	 */
	[[nodiscard]] static TErrorOr<void> DeleteFile(const FString& filePath);

	/**
	 * @brief Gets the absolute path to the given path.
	 *
	 * @param path The path.
	 * @return The absolute path.
	 */
	[[nodiscard]] static FString GetAbsolutePath(const FString& path);

	// TODO GetDirectories

	/**
	 * @brief Gets the absolute path to the current executable.
	 *
	 * @return The absolute path to the current executable.
	 */
	[[nodiscard]] static FString GetExecutablePath();

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
	 * @brief Gets the last system error.
	 *
	 * @return The last system error.
	 */
	[[nodiscard]] static FStringView GetLastError();

	/**
	 * @brief Gets the last system error as an error.
	 *
	 * @return The last system error as an error.
	 */
	[[nodiscard]] static TErrorOr<void> GetLastErrorAsError();

	/**
	 * @brief Similar to GetFiles, but will attempt to match file names based on a pattern.
	 *
	 * @param path The base directory to begin finding files in.
	 * @param pattern The pattern to match file names against.
	 * @param recursive True to search for files recursively, false to only search the top-level directory.
	 * @param files The file array to populate.
	 * @return An error if one was encountered.
	 */
	[[nodiscard]] static TErrorOr<void> FindFiles(const FString& path, const FString& pattern, bool recursive, TArray<FString>& files);

	/**
	 * @brief Gets the absolute path to the current working directory.
	 *
	 * @return The absolute path to the current working directory.
	 */
	[[nodiscard]] static FString GetWorkingDir();

	/**
	 * @brief Attempts to stat a file.
	 *
	 * @param fileName The name of the file.
	 * @param stats The file stats.
	 */
	static void StatFile(const FString& fileName, FFileStats& stats);
};

using FNativeDirectory = FLinuxFileSystem;
using FNativeFile = FLinuxFileSystem;
using FNativeFileSystem = FLinuxFileSystem;
using FNativePath = FLinuxFileSystem;
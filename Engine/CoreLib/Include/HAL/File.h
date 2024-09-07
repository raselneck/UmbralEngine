#pragma once

#include "Containers/Array.h"
#include "Containers/Function.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "HAL/DateTime.h"
#include "Engine/Error.h"
#include "HAL/FileStream.h"

class FEventLoop;

/**
 * @brief Defines file stats.
 */
struct FFileStats
{
	/**
	 * @brief The file's size, in bytes.
	 */
	int64 Size = 0;

	/**
	 * @brief The last time the file was modified.
	 */
	FDateTime ModifiedTime;

	/**
	 * @brief The time the file was created.
	 *
	 * @remark Note that this may be inaccurate as some file systems, like Linux, don't store this.
	 */
	FDateTime CreationTime;

	/**
	 * @brief The last time the file was accessed.
	 */
	FDateTime LastAccessTime;

	/**
	 * @brief Whether or not the file is a directory.
	 */
	bool IsDirectory = false;

	/**
	 * @brief Whether or not the file is read-only.
	 */
	bool IsReadOnly = false;

	/**
	 * @brief Whether or not the file exists.
	 */
	bool Exists = false;
};

/**
 * @brief Defines a static way to interact with files.
 */
class FFile
{
public:

	using FErrorCallback = TFunction<void(FError)>;
	using FReadBytesCallback = TFunction<void(TArray<uint8>)>;
	using FReadTextCallback = TFunction<void(FString)>;
	using FStatCallback = TFunction<void(FFileStats)>;

	/**
	 * @brief Attempts to delete the file pointed to the given path.
	 *
	 * @param filePath The path to the file.
	 * @return The error that was encountered, if any.
	 */
	[[nodiscard]] static TErrorOr<void> Delete(FStringView filePath);

	/**
	 * @brief Attempts to delete the file pointed to the given path.
	 *
	 * @param filePath The path to the file.
	 * @return The error that was encountered, if any.
	 */
	[[nodiscard]] static TErrorOr<void> Delete(const FString& filePath);

	/**
	 * @brief Checks to see if the given file exists.
	 *
	 * @param fileName The name of the file.
	 * @returns True if the file exists, otherwise false.
	 */
	[[nodiscard]] static bool Exists(FStringView fileName);

	/**
	 * @brief Checks to see if the given file exists.
	 *
	 * @param fileName The name of the file.
	 * @returns True if the file exists, otherwise false.
	 */
	[[nodiscard]] static bool Exists(const FString& fileName);

	/**
	 * @brief Attempts to read all bytes from a file.
	 *
	 * @param fileName The name of the file.
	 * @param bytes The output bytes.
	 * @returns True if the file was read successfully, otherwise false.
	 */
	static bool ReadBytes(FStringView fileName, TArray<uint8>& bytes);

	/**
	 * @brief Attempts to read all bytes from a file.
	 *
	 * @param fileName The name of the file.
	 * @returns The array of bytes representing the file, or an error if one occurred.
	 */
	static TErrorOr<TArray<uint8>> ReadBytes(FStringView fileName);

	/**
	 * @brief Reads a file as an array of bytes asynchronously.
	 *
	 * @param filePath The path of the file to read.
	 * @param eventLoop The event loop to queue the read operation to.
	 * @param callback The function to call once reading is complete.
	 * @param errorCallback The function to call if an error occurs.
	 */
	static void ReadBytesAsync(FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadBytesCallback callback, FErrorCallback errorCallback);

	/**
	 * @brief Attempts to read all lines of text from a file.
	 *
	 * @param fileName The name of the file.
	 * @param lines The output lines.
	 * @returns True if the file was read successfully, otherwise false.
	 */
	static bool ReadLines(FStringView fileName, TArray<FString>& lines);

	/**
	 * @brief Attempts to read all text from a file.
	 *
	 * @param fileName The name of the file.
	 * @param text The output string.
	 * @returns True if the file was read successfully, otherwise false.
	 */
	static bool ReadText(FStringView fileName, FString& text);

	/**
	 * @brief Attempts to read all bytes from a file.
	 *
	 * @param fileName The name of the file.
	 * @returns The array of bytes representing the file, or an error if one occurred.
	 */
	static TErrorOr<FString> ReadText(FStringView fileName);

	/**
	 * @brief Reads a file as a string asynchronously.
	 *
	 * @param filePath The path of the file to read.
	 * @param eventLoop The event loop to queue the read operation to.
	 * @param callback The function to call once reading is complete.
	 * @param errorCallback The function to call if an error occurs.
	 */
	static void ReadTextAsync(FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadTextCallback callback, FErrorCallback errorCallback);

	/**
	 * @brief Attempts to stat a file.
	 *
	 * @param fileName The name of the file.
	 * @param stats The file stats.
	 */
	static void Stat(FStringView fileName, FFileStats& stats);

	/**
	 * @brief Attempts to stat a file.
	 *
	 * @param fileName The name of the file.
	 * @param stats The file stats.
	 */
	static void Stat(const FString& fileName, FFileStats& stats);

	/**
	 * @brief Attempts to stat a file asynchronously.
	 *
	 * @param filePath The path to the file.
	 * @param eventLoop The event loop to queue the stat operation to.
	 * @param callback The function to call once stat-ing is complete.
	 * @param errorCallback The function to call if an error occurs.
	 */
	static void StatAsync(FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FStatCallback callback, FErrorCallback errorCallback);

	/**
	 * @brief Attempts to stat a file.
	 *
	 * @param fileName The name of the file.
	 * @returns The file stats.
	 */
	static FFileStats Stat(FStringView fileName)
	{
		FFileStats result;
		Stat(fileName, result);
		return result;
	}

	/**
	 * @brief Attempts to stat a file.
	 *
	 * @param fileName The name of the file.
	 * @returns The file stats.
	 */
	static FFileStats Stat(const FString& fileName)
	{
		FFileStats result;
		Stat(fileName, result);
		return result;
	}
};
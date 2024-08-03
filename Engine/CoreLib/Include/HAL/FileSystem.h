#pragma once

#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "HAL/FileStream.h"
#include "Memory/SharedPtr.h"
#include "Misc/Badge.h"

// TODO More concise API name for files being accessed anywhere... Like IsSiloed or IsLimited or something?

/**
 * @brief Defines a static way to safely access the file system.
 */
class FFileSystem final
{
	friend class UEngineLoop;

public:

	/**
	 * @brief Gets a value indicating whether or not files from anywhere can be accessed.
	 *
	 * @return True if files located anywhere can be accessed.
	 */
	[[nodiscard]] static bool CanAccessFilesAnywhere();

	/**
	 * @brief Gets a message for the last error that the file system encountered.
	 *
	 * @return A message for the last error that the file system encountered.
	 */
	[[nodiscard]] static FStringView GetLastError();

	/**
	 * @brief Attempts to mount the given directory to the root of the file system.
	 *
	 * @param directory The directory to mount.
	 * @returns True if the directory was mounted, otherwise false.
	 */
	[[maybe_unused]] static bool Mount(FStringView directory);

	/**
	 * @brief Attempts to mount the given directory to the file system.
	 *
	 * @param directory The directory to mount.
	 * @param mountPoint The name of the directory's mount point.
	 * @returns True if the directory was mounted, otherwise false.
	 */
	[[maybe_unused]] static bool Mount(FStringView directory, FStringView mountPoint);

	/**
	 * @brief Attempts to open a file for reading.
	 *
	 * @param path The path to the file.
	 * @return The file stream, or nullptr if the file could not be opened.
	 */
	[[nodiscard]] static TSharedPtr<IFileStream> OpenRead(FStringView path)
	{
		return Open(path, EFileMode::Open, EFileAccess::Read);
	}

	/**
	 * @brief Attempts to open a file for writing.
	 *
	 * @param path The path to the file.
	 * @return The file stream, or nullptr if the file could not be opened.
	 */
	[[nodiscard]] static TSharedPtr<IFileStream> OpenWrite(FStringView path)
	{
		return Open(path, EFileMode::Create, EFileAccess::Write);
	}

	/**
	 * @brief Sets whether or not files anywhere can be accessed.
	 *
	 * Note that this function will only be respected when called the first time!
	 *
	 * @param canAccessFilesAnywhere True to allow files anywhere to be accessed, otherwise false.
	 */
	static void SetCanAccessFilesAnywhere(bool canAccessFilesAnywhere);

private:

	/**
	 * @brief Attempts to open a file stream.
	 *
	 * @param path The path to the file.
	 * @param mode The file open mode.
	 * @param access The file access mode.
	 * @return The file stream, or nullptr if the file could not be opened.
	 */
	[[nodiscard]] static TSharedPtr<IFileStream> Open(FStringView path, EFileMode mode, EFileAccess access);
};
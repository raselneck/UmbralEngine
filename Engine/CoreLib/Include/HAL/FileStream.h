#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"

// TODO Split readable and writable streams into separate types

/**
 * @brief An enumeration of file access modes.
 */
enum class EFileAccess
{
	Read,
	Write
};

/**
 * @brief An enumeration of file open modes.
 */
enum class EFileMode
{
	/** @brief Specifies that a new file should be created. Will fail if one already exists. */
	CreateNew,
	/** @brief Specifies that a new file should be created. Will overwrite any existing file. */
	Create,
	/** @brief Specifies that an existing file should be opened. */
	Open,
	/** @brief Specifies that an existing file should be opened. Will create a new file if one does not exist. */
	OpenOrCreate,
	/** @brief Specifies that an existing file should be opened and have its length truncated to zero. */
	Truncate,
	/** @brief Specifies that an existing file should be opened and then seek to the end of the file. */
	Append
};

/**
 * @brief An enumeration of possible seek origins.
 */
enum class ESeekOrigin
{
	Beginning,
	Current,
	End
};

/**
 * @brief Defines the interface for file streams.
 */
class IFileStream
{
	friend class FMemory;

public:

	/**
	 * @brief Returns a value indicating whether or not this stream can read from the opened file.
	 *
	 * @return True if this stream can be read from, otherwise false.
	 */
	[[nodiscard]] bool CanRead() const;

	/**
	 * @brief Returns a value indicating whether or not this stream can write to the opened file.
	 *
	 * @return True if this stream can be written to, otherwise false.
	 */
	[[nodiscard]] bool CanWrite() const;

	/**
	 * @brief Closes this file stream.
	 */
	virtual void Close() = 0;

	/**
	 * @brief Flushes this file stream.
	 */
	virtual void Flush() = 0;

	/**
	 * @brief Gets the total length, in bytes, of the opened file.
	 *
	 * @return The total length, in bytes, of the opened file.
	 */
	[[nodiscard]] virtual int64 GetLength() const = 0;

	/**
	 * @brief Gets the path to the underlying file.
	 *
	 * @return The path to the underlying file.
	 */
	[[nodiscard]] FStringView GetPath() const;

	/**
	 * @brief Gets a value indicating whether or not this stream is at the end.
	 *
	 * @return True if this stream is at the end, otherwise false.
	 */
	[[nodiscard]] virtual bool IsAtEnd() const = 0;

	/**
	 * @brief Gets a value indicating whether or not this stream has an opened file.
	 *
	 * @return True if this stream has an opened file, otherwise false.
	 */
	[[nodiscard]] virtual bool IsOpen() const = 0;

	/**
	 * @brief Attempts to read \p dataSize bytes from the stream into \p data.
	 *
	 * @param data The target data buffer to read into.
	 * @param dataSize The number of bytes to read into \p data.
	 */
	virtual void Read(void* data, uint64 dataSize) = 0;

	/**
	 * @brief Seeks the current position this stream is at.
	 *
	 * @param origin The seek origin.
	 * @param offset The offset from the seek origin.
	 */
	virtual void Seek(ESeekOrigin origin, int64 offset) = 0;

	/**
	 * @brief Gets this stream's current position.
	 *
	 * @return This stream's current position.
	 */
	[[nodiscard]] virtual int64 Tell() const = 0;

	/**
	 * @brief Writes the given data to this stream.
	 *
	 * @param data The data.
	 * @param dataSize The size, in bytes, of \p data.
	 */
	virtual void Write(const void* data, uint64 dataSize) = 0;

	/**
	 * @brief Writes a byte span to this stream.
	 *
	 * @param bytes The byte span.
	 */
	void Write(TSpan<const uint8> bytes);

	/**
	 * @brief Writes a string to this stream.
	 * .
	 * @param string The string.
	 */
	void Write(FStringView string);

protected:

	/**
	 * @brief Sets default values for this file stream's properties.
	 *
	 * @param path The file's path.
	 * @param accessMode The file's access mode.
	 * @param openMode The file's open mode.
	 */
	IFileStream(const FString& path, EFileAccess accessMode, EFileMode openMode);

	/**
	 * @brief Sets default values for this file stream's properties.
	 *
	 * @param path The file's path.
	 * @param accessMode The file's access mode.
	 * @param openMode The file's open mode.
	 */
	IFileStream(FString&& path, EFileAccess accessMode, EFileMode openMode);

	/**
	 * @brief Destroys this file stream.
	 */
	virtual ~IFileStream() = default;

private:

	FString m_Path;
	EFileAccess m_AccessMode;
	EFileMode m_OpenMode;
};
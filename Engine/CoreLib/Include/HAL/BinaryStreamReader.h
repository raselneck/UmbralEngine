#pragma once

#include "Containers/String.h"
#include "Engine/Platform.h"
#include "HAL/FileStream.h"
#include "Memory/WeakPtr.h"

/**
 * @brief Defines a helper reading binary streams.
 */
class FBinaryStreamReader
{
public:

	/**
	 * @brief Gets the endianness that this binary reader is using when reading.
	 *
	 * If this differs from the current system's endianness, then the bytes read
	 * for multi-byte types will be reversed to produce correct values.
	 *
	 * @return The endianness used by this stream reader.
	 */
	[[nodiscard]] EEndianness GetEndianness() const;

	/**
	 * @brief Reads a single character from the stream.
	 *
	 * @return The read character.
	 */
	[[nodiscard]] char ReadChar();

	/**
	 * @brief Reads a 64-bit floating point number from the stream.
	 *
	 * @return The read floating point number.
	 */
	[[nodiscard]] double ReadDouble();

	/**
	 * @brief Reads a 32-bit floating point number from the stream.
	 *
	 * @return The read floating point number.
	 */
	[[nodiscard]] float ReadFloat();

	/**
	 * @brief Reads an 8-bit signed integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] int8 ReadInt8();

	/**
	 * @brief Reads a 16-bit signed integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] int16 ReadInt16();

	/**
	 * @brief Reads a 32-bit signed integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] int32 ReadInt32();

	/**
	 * @brief Reads a 64-bit signed integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] int64 ReadInt64();

	/**
	 * @brief Reads an 8-bit unsigned integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] uint8 ReadUint8();

	/**
	 * @brief Reads a 16-bit unsigned integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] uint16 ReadUint16();

	/**
	 * @brief Reads a 32-bit unsigned integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] uint32 ReadUint32();

	/**
	 * @brief Reads a 64-bit unsigned integer from the stream.
	 *
	 * @return The read integer.
	 */
	[[nodiscard]] uint64 ReadUint64();

	/**
	 * @brief Reads a string from the stream.
	 *
	 * @param numBytes The number of bytes to read for the string.
	 * @return The read string.
	 */
	[[nodiscard]] FString ReadString(FString::SizeType numBytes);

	/**
	 * @brief Sets the endianness that this binary reader will use.
	 *
	 * @param endianness The new endianness value.
	 */
	void SetEndianness(EEndianness endianness);

	/**
	 * @brief Sets the file stream to be used by this binary reader.
	 *
	 * @param fileStream The underlying file stream.
	 */
	void SetFileStream(TSharedPtr<IFileStream> fileStream);

private:

	/**
	 * @brief Gets a value indicating whether or not this binary reader can read from its referenced stream.
	 *
	 * @return True if this binary reader can read from the referenced stream, otherwise false.
	 */
	[[nodiscard]] bool CanReadFromStream();

	TWeakPtr<IFileStream> m_FileStream;
	EEndianness m_Endianness = EEndianness::Little;
};
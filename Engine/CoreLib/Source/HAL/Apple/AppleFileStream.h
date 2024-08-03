#pragma once

#include "HAL/FileStream.h"
#include "Memory/SharedPtr.h"

/**
 * @brief Defines an Apple file stream.
 */
class FAppleFileStream final : public IFileStream
{
	friend class FMemory;

public:

	/**
	 * @brief Sets default values for this Apple file stream's properties.
	 *
	 * @param descriptor The file descriptor.
	 * @param path The file's path.
	 * @param accessMode The file access mode.
	 * @param openMode The file's open mode.
	 */
	FAppleFileStream(int32 descriptor, FString path, EFileAccess accessMode, EFileMode openMode);

	/**
	 * @brief Destroys this Apple file stream.
	 */
	virtual ~FAppleFileStream() override;

	/** @copydoc IFileStream::Close */
	virtual void Close() override;

	/** @copydoc IFileStream::Flush */
	virtual void Flush() override;

	/** @copydoc IFileStream::GetLength */
	virtual int64 GetLength() const override;

	/** @copydoc IFileStream::IsAtEnd */
	virtual bool IsAtEnd() const override;

	/** @copydoc IFileStream::IsOpen */
	virtual bool IsOpen() const override;

	/**
	 * @brief Attempts to open a POSIX file stream.
	 *
	 * @param path The path to the file.
	 * @param mode The file open mode.
	 * @param access The file access mode.
	 * @return The file stream, or nullptr if the file could not be opened.
	 */
	[[nodiscard]] static TSharedPtr<FAppleFileStream> Open(FStringView path, EFileMode mode, EFileAccess access);

	/** @copydoc IFileStream::Read */
	virtual void Read(void* data, uint64 dataSize) override;

	/** @copydoc IFileStream::Seek */
	virtual void Seek(ESeekOrigin origin, int64 offset) override;

	/** @copydoc IFileStream::Tell */
	virtual int64 Tell() const override;

	/** @copydoc IFileStream::Write */
	virtual void Write(const void* data, uint64 dataSize) override;

private:

	int64 m_Length = 0;
	int32 m_Descriptor = INDEX_NONE;
};

using FNativeFileStream = FAppleFileStream;
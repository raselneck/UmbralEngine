#pragma once

#include "Engine/Logging/LogListener.h"
#include "HAL/FileStream.h"
#include "Memory/SharedPtr.h"

/**
 * @brief Defines a log listener that writes to a file.
 */
class FFileLogListener final : public ILogListener
{
	FFileLogListener(const FFileLogListener&) = delete;
	FFileLogListener& operator=(const FFileLogListener&) = delete;

public:

	UM_DEFAULT_MOVE(FFileLogListener);

	/**
	 * @brief Sets default values for this log listener's properties.
	 */
	FFileLogListener() = default;

	/**
	 * @brief Destroys this file log listener.
	 */
	virtual ~FFileLogListener() override;

	/**
	 * @brief Closes the underlying file, if it is open.
	 */
	void Close();

	/**
	 * @brief Flushes the underlying file.
	 */
	virtual void Flush() override;

	/**
	 * @brief Attempts to open a new file to write to.
	 *
	 * @param fileName The name of the file to write to.
	 * @return True if the file was opened for writing, otherwise false.
	 */
	[[nodiscard]] bool Open(FStringView fileName);

	/**
	 * @brief Writes a string to the underlying file.
	 *
	 * @param logLevel The log level.
	 * @param string The string to write.
	 */
	virtual void Write(ELogLevel logLevel, FStringView string) const override;

private:

	TSharedPtr<IFileStream> m_File;
};
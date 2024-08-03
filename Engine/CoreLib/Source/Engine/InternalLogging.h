#pragma once

#include "Containers/String.h"
#include "Engine/Error.h"
#include "Engine/Logging.h"
#include "Misc/StringFormatting.h"

namespace Private
{
	/**
	 * @brief Creates a formatted string for logging.
	 *
	 * @param logLevel The log level.
	 * @param message The log message.
	 * @return The log string.
	 */
	FString CreateLogString(ELogLevel logLevel, FStringView message);

	/**
	 * @brief Creates a formatted string for logging.
	 *
	 * @param logLevel The log level.
	 * @param message The log message.
	 * @param messageArgs The arguments to use when formatting the message.
	 * @return The log string.
	 */
	FString CreateLogString(ELogLevel logLevel, FStringView message, TSpan<Private::FStringFormatArgument> messageArgs);

	/**
	 * @brief Attempts to initialize the logging system.
	 *
	 * @return Nothing if logging was initialized successfully, otherwise the error that was encountered.
	 */
	void InitializeLogging();

	/**
	 * @brief Shuts down the logging system.
	 */
	void ShutdownLogging();
}
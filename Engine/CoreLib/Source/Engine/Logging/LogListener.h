#pragma once

#include "Engine/Logging.h"

/**
 * @brief Defines a log listener.
 */
class ILogListener
{
public:

	/**
	 * @brief Destroys this log listener.
	 */
	virtual ~ILogListener() = default;

	/**
	 * @brief Flushes this log listener.
	 */
	virtual void Flush() = 0;

	/**
	 * @brief Writes a string to the log.
	 *
	 * The string will already have been formatted.
	 *
	 * @param logLevel The log level.
	 * @param string The string.
	 */
	virtual void Write(ELogLevel logLevel, FStringView string) const = 0;
};
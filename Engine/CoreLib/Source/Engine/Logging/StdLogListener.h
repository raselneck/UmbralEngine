#pragma once

#include "Engine/Logging/LogListener.h"

/**
 * @brief Defines a log listener that writes to stdout and stderr.
 */
class FStdLogListener final : public ILogListener
{
public:

	/**
	 * @brief Flushes this log listener.
	 */
	virtual void Flush() override;

	/**
	 * @brief Writes a string to this log listener.
	 *
	 * @param logLevel The log level.
	 * @param string The string.
	 */
	virtual void Write(ELogLevel logLevel, FStringView string) const override;
};
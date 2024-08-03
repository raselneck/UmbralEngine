#pragma once

#include "Engine/IntTypes.h"
#include "HAL/DateTime.h"

/**
 * @brief Defines Windows-specific time functions.
 */
class FWindowsTime final
{
public:

	/**
	 * @brief Gets the current system's local time.
	 *
	 * @return The current system's local time.
	 */
	[[nodiscard]] static FDateTime GetLocalTime();

	/**
	 * @brief Gets the current system time in ticks.
	 *
	 * @return The current system time in ticks.
	 */
	[[nodiscard]] static int64 GetSystemTimeInTicks();

	/**
	 * @brief Gets the current UTC time.
	 *
	 * @return The current UTC time.
	 */
	[[nodiscard]] static FDateTime GetUtcTime();
};

using FPlatformTime = FWindowsTime;
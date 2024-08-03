#pragma once

#include "HAL/TimeSpan.h"

/**
 * @brief Defines Windows-specific threading implementations.
 */
class FWindowsThread
{
public:

	/**
	 * @brief Sleeps the calling thread for the given duration.
	 *
	 * @param duration The duration to sleep for.
	 */
	static void Sleep(FTimeSpan duration);
};

using FPlatformThread = FWindowsThread;
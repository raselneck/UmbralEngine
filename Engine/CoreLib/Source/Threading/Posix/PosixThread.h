#pragma once

#include "HAL/TimeSpan.h"

/**
 * @brief Defines POSIX-specific threading implementations.
 */
class FPosixThread
{
public:

	/**
	 * @brief Sleeps the calling thread for the given duration.
	 *
	 * @param duration The duration to sleep for.
	 */
	static void Sleep(FTimeSpan duration);
};

using FPlatformThread = FPosixThread;
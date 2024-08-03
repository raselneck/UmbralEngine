#pragma once

#include "Containers/Function.h"
#include "HAL/TimeSpan.h"
#include "Memory/UniquePtr.h"

/** The function type for running a thread without any arguments. */
using FThreadFunction = TFunction<void()>;

/** The function type for running a thread with a user-defined argument. */
using FParameterizedThreadFunction = TFunction<void(void*)>;

/**
 * @brief Defines a way to launch threads or manipulate the calling thread.
 */
class FThread
{
public:

	/**
	 * @brief Sleeps the calling thread for the given duration.
	 *
	 * @param duration The duration to sleep for.
	 */
	static void Sleep(FTimeSpan duration);
};
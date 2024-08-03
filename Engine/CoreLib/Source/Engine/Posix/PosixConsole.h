#pragma once

#include "Containers/String.h"

/**
 * @brief Defines POSIX-specific console functionality.
 */
class FPosixConsole
{
public:

	/**
	 * @brief Reads a line of text as input from the console.
	 *
	 * @return The input string.
	 */
	static FString ReadLine();
};

using FPlatformConsole = FPosixConsole;
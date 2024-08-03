#pragma once

#include "Containers/String.h"

/**
 * @brief Defines Windows-specific console functionality.
 */
class FWindowsConsole
{
public:

	/**
	 * @brief Reads a line of text as input from the console.
	 *
	 * @return The input string.
	 */
	static FString ReadLine();
};

using FPlatformConsole = FWindowsConsole;
#pragma once

#include "Containers/Span.h"
#include "Containers/StringView.h"
#include "Misc/Badge.h"

/**
 * @brief Defines a way to interact with the command line arguments.
 */
class FCommandLine final
{
public:

	/**
	 * @brief Gets the raw argument count.
	 *
	 * @return The raw argument count.
	 */
	static int32 GetArgc();

	/**
	 * @brief Gets the raw argument values.
	 *
	 * @return The raw argument values.
	 */
	static const char* const* GetArgv();

	/**
	 * @brief Gets all of the command line arguments.
	 *
	 * @return All of the command line arguments.
	 */
	static TSpan<FStringView> GetArguments();

	/**
	 * @brief Gets the argument at the given index.
	 *
	 * @param index The argument index.
	 * @return The argument at \p index.
	 */
	static FStringView GetArgument(int32 index);

	/**
	 * @brief Initializes the argument count and values. Will ignore future calls after the first.
	 *
	 * @param argc The command line argument count.
	 * @param argv The command line argument values.
	 */
	static void Initialize(int32 argc, char** argv);
};
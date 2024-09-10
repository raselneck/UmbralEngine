#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Misc/Badge.h"
#include "Misc/CString.h"

/**
 * @brief Defines a memory-safe wrapper for a cover of argc and argv.
 */
class FCommandLineArguments
{
public:

	/**
	 * @brief Sets default values for these command line arguments.
	 *
	 * @param arguments The arguments.
	 */
	explicit FCommandLineArguments(TArray<FCString> arguments);

	/**
	 * @brief Gets the argument count.
	 *
	 * @return The argument count.
	 */
	[[nodiscard]] int32 GetArgc() const
	{
		return m_Arguments.Num();
	}

	/**
	 * @brief Gets the argument values.
	 *
	 * @return The argument values.
	 */
	[[nodiscard]] char** GetArgv()
	{
		return m_MutableArguments.GetData();
	}

private:

	// NOTE: MutableArguments is necessary for something like Google Test that actually modifies the argv array

	TArray<FCString> m_Arguments;
	TArray<char*> m_MutableArguments;
};

/**
 * @brief Defines a way to interact with the command line arguments.
 */
class FCommandLine final
{
public:

	/**
	 * @brief Gets the total number of command line arguments.
	 *
	 * @return The total number of command line arguments.
	 */
	static int32 GetArgc();

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
	 * @brief Gets a mutable copy of the command line arguments.
	 *
	 * @return The command line arguments.
	 */
	static FCommandLineArguments GetMutableArguments();

	/**
	 * @brief Initializes the argument count and values. Will ignore future calls after the first.
	 *
	 * @param argc The command line argument count.
	 * @param argv The command line argument values.
	 */
	static void Initialize(int32 argc, char** argv);
};
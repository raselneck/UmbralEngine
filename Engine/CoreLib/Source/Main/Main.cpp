#define __STDC_WANT_LIB_EXT1__ 1
#include <cstdlib>

#include "Engine/Console.h"
#include "Engine/CommandLine.h"
#include "Engine/InternalLogging.h"
#include "Engine/Internationalization.h"
#include "HAL/Directory.h"
#include "HAL/FileSystem.h"
#include "Main/Main.h"
#include "Misc/AtExit.h"
#include <cerrno>
#include <mimalloc.h>

#ifdef __STDC_LIB_EXT1__
static void HandleConstraintViolation(const char* msg, void* ptr, errno_t error)
{
	// Leave this alone for now until we get a compile error about unused parameters :^)
}
#endif

int UMBRAL_EXPORT main(int argc, char** argv)
{
#ifdef __STDC_LIB_EXT1__
	set_constraint_handler_s(HandleConstraintViolation);
#endif

	mi_register_output([](const char* message, void* /*userData*/)
	{
		// Sometimes registering the output function will just try to print "\n"
		const int32 messageLength = TStringTraits<char>::GetNullTerminatedCharCount(message);
		if (messageLength < 2)
		{
			return;
		}

		const FStringView messageView { message, messageLength };
		UM_LOG(Debug, "Memory Output: {}", messageView);
	}, nullptr);

	mi_register_error([](int errorCode, void* /*userData*/)
	{
		constexpr auto GetMemoryErrorMessage = [](const int32 errorCode) -> FStringView
		{
			switch (errorCode)
			{
			case EAGAIN:    return "Double free was detected"_sv;
			case EFAULT:    return "Corrupted free list or meta-data was detected"_sv;
			case ENOMEM:    return "Not enough memory available to satisfy the request"_sv;
			case EOVERFLOW: return "Attempting to allocate too much memory"_sv;
			case EINVAL:    return "Trying to free or re-allocate an invalid pointer"_sv;
			default:        return "Unknown error detected"_sv;
			}
		};

		const FStringView errorMessage = GetMemoryErrorMessage(errorCode);
		UM_LOG(Error, "Memory Error: {}", errorMessage);
	}, nullptr);

	FCommandLine::Initialize(argc, argv);

	// Logging cannot fail to initialize because at worst we will have console logging
	// Logging also is not dependent on any of our subsystems that need to be initialized
	Private::InitializeLogging();

	ON_EXIT_SCOPE()
	{
		Private::ShutdownLogging();
	};

	// We absolutely need to initialize the internationalization system for proper string comparison
	// TODO Fall back to byte-by-byte comparison without this?
	if (TErrorOr<void> result = FInternationalization::Initialize();
	    result.IsError())
	{
		UM_LOG(Error, "{}", result.ReleaseError());
		return EXIT_FAILURE;
	}

	ON_EXIT_SCOPE()
	{
		FInternationalization::Shutdown();
	};

	// Make sure we can find relative files
	const FString workingDirectory = FDirectory::GetWorkingDir();
	FFileSystem::Mount(workingDirectory);

	return UmbralMain();
}
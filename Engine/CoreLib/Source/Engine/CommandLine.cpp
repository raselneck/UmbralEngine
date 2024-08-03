#include "Containers/Array.h"
#include "Engine/CommandLine.h"

static int32 GArgCount = 0;
static char** GArgValues = nullptr;
static TArray<FStringView> GArguments;

int32 FCommandLine::GetArgc()
{
	return GArgCount;
}

const char* const* FCommandLine::GetArgv()
{
	return const_cast<const char* const*>(GArgValues);
}

TSpan<FStringView> FCommandLine::GetArguments()
{
	return GArguments.AsSpan();
}

FStringView FCommandLine::GetArgument(int32 index)
{
	if (GArguments.IsValidIndex(index))
	{
		return GArguments[index];
	}
	return {};
}

void FCommandLine::Initialize(int32 argc, char** argv)
{
	if (GArgValues != nullptr)
	{
		return;
	}

	GArgCount = argc;
	GArgValues = argv;
	GArguments.Reserve(argc);

	while (argc > 0)
	{
		const int32 argLength = TCharTraits<char>::GetNullTerminatedLength(*argv);
		(void)GArguments.Emplace(*argv, argLength);

		++argv;
		--argc;
	}
}
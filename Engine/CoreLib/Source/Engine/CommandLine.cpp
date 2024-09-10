#include "Containers/Array.h"
#include "Engine/CommandLine.h"

static int32 GArgCount = 0;
static char** GArgValues = nullptr;
static TArray<FStringView> GArguments;

FCommandLineArguments::FCommandLineArguments(TArray<FCString> arguments)
	: m_Arguments { MoveTemp(arguments) }
{
	m_MutableArguments.Reserve(m_Arguments.Num());
	for (FCString& arg : m_Arguments)
	{
		m_MutableArguments.Add(arg.GetChars());
	}
}

int32 FCommandLine::GetArgc()
{
	return GArgCount;
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

FCommandLineArguments FCommandLine::GetMutableArguments()
{
	TArray<FCString> arguments;
	for (int32 idx = 0; idx < GArgCount; ++idx)
	{
		(void)arguments.Emplace(GArgValues[idx]);
	}
	return FCommandLineArguments { MoveTemp(arguments) };
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
#include "Engine/CommandLine.h"
#include "Main/Main.h"
#include <gtest/gtest.h>

extern "C" int32 UmbralMain()
{
	FCommandLineArguments arguments = FCommandLine::GetMutableArguments();
	int32 argc = arguments.GetArgc();
	char** argv = arguments.GetArgv();

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
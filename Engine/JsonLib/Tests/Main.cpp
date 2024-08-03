#include "Engine/CommandLine.h"
#include "Main/Main.h"
#include <gtest/gtest.h>

extern "C" int32 UmbralMain()
{
	int32 argc = FCommandLine::GetArgc();
	char** argv = const_cast<char**>(FCommandLine::GetArgv());

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
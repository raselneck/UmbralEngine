#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include "HAL/File.h"
#include "HAL/TimePoint.h"
#include <gtest/gtest.h>

TEST(FileTests, ReadTextAsync)
{
	TSharedPtr<FEventLoop> eventLoop = FEventLoop::Create();
	bool readFile = false;
	bool encounteredError = false;

	const FString filePath { UMBRAL_FILE_AS_VIEW };
	const FTimePoint fileReadStart = FTimePoint::Now();
	FFile::ReadTextAsync(filePath, eventLoop,
		[&](FString content)
		{
			UM_LOG(Info, "File contents:\n{}", content.AsStringView());
			readFile = true;
		},
		[&](FError error)
		{
			UM_LOG(Info, "Read file error: {}", error.GetMessage());
			encounteredError = true;
		}
	);

	while (eventLoop->NumTasks() > 0)
	{
		eventLoop->PollTasks();
	}

	const FTimePoint fileReadEnd = FTimePoint::Now();
	const FTimeSpan fileReadDuration = fileReadEnd - fileReadStart;
	UM_LOG(Info, "Took {} ms to read file \"{}\"", fileReadDuration.GetTotalMilliseconds(), filePath.AsStringView());
}
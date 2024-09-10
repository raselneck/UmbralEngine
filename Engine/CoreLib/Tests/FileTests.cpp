#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include "HAL/File.h"
#include "HAL/TimePoint.h"
#include "HAL/Timer.h"
#include <gtest/gtest.h>

// TODO Read text from a file that does not exist

TEST(FileTests, ReadTextAsync)
{
	TSharedPtr<FEventLoop> eventLoop = FEventLoop::Create();

	const FString filePath { UMBRAL_FILE_AS_VIEW };
	const FTimePoint fileReadStart = FTimePoint::Now();
	FFile::ReadTextAsync(filePath, eventLoop,
		[=](const FString content)
		{
			UM_LOG(Info, "Read {} characters from file \"{}\"", content.Length(), filePath);
		},
		[](const FError error)
		{
			UM_LOG(Info, "Read file error: {}", error.GetMessage());
		}
	);

	while (eventLoop->IsRunning())
	{
		eventLoop->PollTasks();
	}

	const FTimePoint fileReadEnd = FTimePoint::Now();
	const FTimeSpan fileReadDuration = fileReadEnd - fileReadStart;
	UM_LOG(Info, "Took {} ms to async read file \"{}\"", fileReadDuration.GetTotalMilliseconds(), filePath.AsStringView());
}

// TODO Stat a file that does not exist
// TODO Stat a directory
// TODO Stat a directory that does not exist

TEST(FileTests, StatFileAsync)
{
	TSharedPtr<FEventLoop> eventLoop = FEventLoop::Create();

	const FString filePath { UMBRAL_FILE_AS_VIEW };
	FFileStats fileStats;
	FTimer statTimer = FTimer::Start();
	FFile::StatAsync(filePath, eventLoop,
		[&](FFileStats stats)
		{
			fileStats = MoveTemp(stats);
		},
		[](const FError error)
		{
			UM_LOG(Info, "Stat file error: {}", error.GetMessage());
		}
	);

	while (eventLoop->IsRunning())
	{
		eventLoop->PollTasks();
	}

	const FTimeSpan statDuration = statTimer.Stop();
	UM_LOG(Info, "Took {} ms to stat file \"{}\"", statDuration.GetTotalMilliseconds(), filePath.AsStringView());

	EXPECT_TRUE(fileStats.Exists);
	EXPECT_FALSE(fileStats.IsDirectory);
	EXPECT_FALSE(fileStats.IsReadOnly);
}
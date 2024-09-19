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

TEST(FileTests, WriteLinesAsync)
{
	constexpr FStringView fileName = "WriteLinesAsync.txt"_sv;

	const TArray<FString> lines
	{{
		"Lorem ipsum odor amet, consectetuer adipiscing elit"_s,
		"Rhoncus pulvinar montes fringilla per imperdiet neque velit blandit"_s,
		"Mollis nascetur mauris sollicitudin mi sociosqu donec platea"_s,
		"Ultrices natoque curae lacus sapien sagittis hendrerit felis volutpat"_s,
		"Sed ut senectus conubia condimentum etiam"_s,
		"Morbi vestibulum netus mus ad egestas sit curae"_s,
		"Pharetra aptent sem arcu molestie nullam dictum facilisis curae"_s,
		"Torquent ac taciti faucibus; nascetur nibh nec placerat"_s,
		"Ridiculus vehicula hac bibendum posuere suspendisse"_s,
		"Semper porta at lobortis sapien; mus habitasse fermentum"_s,
	}};

	TSharedPtr<FEventLoop> eventLoop = FEventLoop::Create();
	bool wroteToFile = true;
	FTimer writeTimer = FTimer::Start();
	FFile::WriteLinesAsync(fileName, lines, eventLoop,
		[&](TErrorOr<void> result)
		{
			if (result.IsError())
			{
				UM_LOG(Error, "Failed to write lines to \"{}\". Reason: {}", fileName, result.GetError().GetMessage());
				wroteToFile = false;
			}
		}
	);

	while (eventLoop->IsRunning())
	{
		eventLoop->PollTasks();
	}

	EXPECT_TRUE(wroteToFile);
	if (wroteToFile == false)
	{
		return;
	}

	const FTimeSpan writeDuration = writeTimer.Stop();
	UM_LOG(Info, "Took {} ms to async write lines to \"{}\"", writeDuration.GetTotalMilliseconds(), fileName);

	TArray<FString> fileLines;
	bool openedFile = false;
	EXPECT_TRUE(openedFile = FFile::ReadLines(fileName, fileLines));

	if (openedFile == false)
	{
		return;
	}

	EXPECT_EQ(lines.Num(), fileLines.Num());
	for (int32 idx = 0; idx < lines.Num(); ++idx)
	{
		EXPECT_EQ(lines[idx], fileLines[idx]);
	}
}
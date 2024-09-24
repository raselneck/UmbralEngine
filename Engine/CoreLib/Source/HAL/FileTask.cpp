#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "HAL/FileTask.h"
#include "HAL/InternalTime.h"
#include <sys/stat.h>

uv_loop_t* FFileTask::GetLoop() const
{
	// Prefer our local reference to the loop since that one would most likely
	// be set when treating the task as a synchronous task
	if (m_Loop)
	{
		return m_Loop;
	}

	if (TSharedPtr<FEventLoop> eventLoop = GetEventLoop())
	{
		return eventLoop->GetLoop();
	}

	UM_ASSERT_NOT_REACHED_MSG("Failed to find associated libuv loop for file task");
}

void FFileTask::CloseFile()
{
	UM_ASSERT(m_CloseRequest.IsNull(), "Attempting to close file while a close request is pending");
	UM_ASSERT(m_FileHandle != INDEX_NONE, "Attempting to close file when it is not yet opened");

	m_CloseRequest = MakeRequest();

	uv_loop_t* loop = GetLoop();
	uv_fs_cb callback = DispatchRequestCallback<&FFileTask::HandleFileClosed>;
	uv_fs_close(loop, m_CloseRequest.Get(), m_FileHandle, callback);
}

FFileTask::FRequestHandle FFileTask::MakeRequest() const
{
	FRequestHandle result { FMemory::AllocateObject<uv_fs_t>() };
	FMemory::ZeroOut(result.Get(), sizeof(uv_fs_t));

	result->data = const_cast<FFileTask*>(this);

	return result;
}

void FFileTask::OpenFile(const FStringView filePath, const int32 openFlags, const int32 openMode)
{
	UM_ASSERT(m_OpenRequest.IsNull(), "Attempting to open file while an open request is pending");
	UM_ASSERT(m_FileHandle == INDEX_NONE, "Attempting to open file while another is already open");

	m_OpenRequest = MakeRequest();
	m_FilePath = FString { filePath };

	uv_loop_t* loop = GetLoop();
	uv_fs_cb callback = DispatchRequestCallback<&FFileTask::HandleFileOpened>;
	uv_fs_open(loop, m_OpenRequest.Get(), m_FilePath.GetChars(), openFlags, openMode, callback);
}

void FFileTask::StatFile(const FStringView filePath)
{
	UM_ASSERT(m_StatRequest.IsNull(), "Attempting to stat file while a stat request is pending");
	UM_ASSERT(m_FilePath.IsEmpty(), "Attempting to stat file while working with another file");

	m_StatRequest = MakeRequest();
	m_FilePath = FString { filePath };

	uv_loop_t* loop = GetLoop();
	uv_fs_cb callback = DispatchRequestCallback<&FFileTask::HandleFileStat>;
	uv_fs_stat(loop, m_StatRequest.Get(), m_FilePath.GetChars(), callback);
}

void FFileTask::HandleFileClosed()
{
	// TODO Should really check for errors here

	m_CloseRequest.Reset();
	m_FileHandle = INDEX_NONE;

	OnFileClosed();

	m_FilePath.Reset();
	m_Loop = nullptr;
}

void FFileTask::HandleFileOpened()
{
	const ssize_t openResult = m_OpenRequest->result;
	m_OpenRequest.Reset();

	if (openResult < 0)
	{
		const FStringView errorString { uv_strerror(static_cast<int32>(openResult)) };
		OnError(MAKE_ERROR("Failed to open file \"{}\". Reason: {}", m_FilePath, errorString));
	}
	else
	{
		m_FileHandle = static_cast<uv_file>(openResult);
		OnFileOpened();
	}
}

/**
 * @brief Converts a libuv timespec to a date-time.
 *
 * @param time The libuv time.
 * @return The date-time.
 */
static FDateTime ConvertFileTime(const uv_timespec_t& time)
{
	const FTimeSpan timeSinceEpoch = FTimeSpan::FromSeconds(static_cast<double>(time.tv_sec));
	const FTimeSpan additionalNanos = Time::NanosecondsToTimeSpan(time.tv_nsec);
	return FDateTime::Epoch + timeSinceEpoch + additionalNanos;
}

void FFileTask::HandleFileStat()
{
	FFileStats stats;
	stats.Exists = m_StatRequest->result >= 0; // TODO May not be accurate

	if (stats.Exists)
	{
		const uv_stat_t& statbuf = m_StatRequest->statbuf;
		stats.Size = static_cast<int64>(statbuf.st_size);
		stats.ModifiedTime = ConvertFileTime(statbuf.st_mtim);
		stats.LastAccessTime = ConvertFileTime(statbuf.st_atim);
		stats.CreationTime = ConvertFileTime(statbuf.st_ctim);
		stats.IsDirectory = HasFlag(statbuf.st_mode, S_IFDIR);
		stats.IsReadOnly = HasFlag(statbuf.st_mode, S_IWUSR) == false;
	}

	m_StatRequest.Reset();

	OnFileStat(MoveTemp(stats));

	m_FilePath.Reset();
}
#include "Containers/StaticArray.h"
#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include "HAL/File.h"
#include "HAL/FileSystem.h"
#include "HAL/InternalTime.h"
#include "Misc/StringBuilder.h"
#include "Templates/NumericLimits.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsFileSystem.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleFileSystem.h"
#else
#	include "HAL/Linux/LinuxFileSystem.h"
#endif
#include <sys/stat.h>
#include <uv.h>

// Thanks, libuv, for including Windows.h :^)
#ifdef DeleteFile
#undef DeleteFile
#endif

/**
 * @brief The maximum length for a file that we can read.
 *
 * TODO This could be increased to int64 max minus one if containers size types were 64-bit
 * TODO Maybe because of the above, containers should use ssize for their size type?
 */
static constexpr int64 GMaxFileLength = TNumericLimits<int32>::MaxValue - 1;

[[maybe_unused]] static void DebugPrintFileStats(const FStringView fileName, const FFileStats& stats)
{
	UM_LOG(Info, "Stats for `{}`:", fileName);
	UM_LOG(Info, "~~ Size           = {}", stats.Size);
	UM_LOG(Info, "~~ ModifiedTime   = {}", stats.ModifiedTime);
	UM_LOG(Info, "~~ CreationTime   = {}", stats.CreationTime);
	UM_LOG(Info, "~~ LastAccessTime = {}", stats.LastAccessTime);
	UM_LOG(Info, "~~ IsDirectory    = {}", stats.IsDirectory);
	UM_LOG(Info, "~~ IsReadOnly     = {}", stats.IsReadOnly);
}

TErrorOr<void> FFile::Delete(FStringView filePathAsView)
{
	const FString filePath { filePathAsView };
	return Delete(filePath);
}

TErrorOr<void> FFile::Delete(const FString& filePath)
{
	return FNativeFile::DeleteFile(filePath);
}

bool FFile::Exists(const FStringView fileNameAsView)
{
	const FString fileName { fileNameAsView };
	return Exists(fileName);
}

bool FFile::Exists(const FString& fileName)
{
	FFileStats stats;
	FFile::Stat(fileName, stats);

	return stats.Exists && stats.IsDirectory == false;
}

bool FFile::ReadBytes(const FStringView fileName, TArray<uint8>& bytes)
{
	TErrorOr<TArray<uint8>> result = ReadBytes(fileName);
	if (result.IsError())
	{
		UM_LOG(Error, "{}", result.GetError());
		return false;
	}

	bytes = result.ReleaseValue();

	return true;
}

TErrorOr<TArray<uint8>> FFile::ReadBytes(const FStringView fileName)
{
	// Attempt to open the file
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenRead(fileName);
	if (fileStream.IsNull())
	{
		return MAKE_ERROR("Failed to open \"{}\"", fileName);
	}

	// Get the length of the file, and reserve memory for the bytes
	const int64 fileLength = fileStream->GetLength();
	if (fileLength > GMaxFileLength)
	{
		return MAKE_ERROR("Cannot read \"{}\"; file size ({}) is too large (maximum size is {})", fileName, fileLength, GMaxFileLength);
	}

	TArray<uint8> bytes;
	bytes.SetNum(static_cast<int32>(fileLength));

	// Read all the bytes at once (probably better to do this progressively, but BIG SHRUG)
	fileStream->Read(bytes.GetData(), bytes.Num());

	// TODO Re-use when IFileStream::Read returns number of bytes read
	/*const int64 numberOfBytesRead = PHYSFS_readBytes(file, bytes.GetData(), bytes.Num());
	if (numberOfBytesRead != fileLength)
	{
		UM_LOG(Warning, "Expected to read {} bytes for \"{}\", actually read {}", fileLength, fileName, numberOfBytesRead);
	}*/

	return bytes;
}

bool FFile::ReadLines(const FStringView fileName, TArray<FString>& lines)
{
	FString fileText;
	if (ReadText(fileName, fileText) == false)
	{
		return false;
	}

	fileText.SplitByChars("\r\n"_sv, EStringSplitOptions::IgnoreEmptyEntries, lines);

	return true;
}

bool FFile::ReadText(const FStringView fileName, FString& text)
{
	TErrorOr<FString> result = ReadText(fileName);
	if (result.IsError())
	{
		UM_LOG(Error, "{}", result.GetError());
		return false;
	}

	text = result.ReleaseValue();

	return true;
}

TErrorOr<FString> FFile::ReadText(FStringView fileName)
{
	// Attempt to open the file
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenRead(fileName);
	if (fileStream.IsNull())
	{
		return MAKE_ERROR("Failed to open \"{}\"; reason: {}", fileName, FFileSystem::GetLastError());
	}

	// Get the length of the file, and reserve memory for the bytes
	const int64 fileLength = fileStream->GetLength();
	if (fileLength > GMaxFileLength)
	{
		return MAKE_ERROR("Cannot read \"{}\"; file size ({}) is too large (maximum size is {})", fileName, fileLength, GMaxFileLength);
	}

	FStringBuilder textBuilder;
	void* fileBuffer = textBuilder.AddZeroed(static_cast<FStringBuilder::SizeType>(fileLength));

	// Read all the bytes at once (probably better to do this progressively, but BIG SHRUG)
	fileStream->Read(fileBuffer, static_cast<uint64>(fileLength));

	// TODO Re-use when IFileStream::Read returns number of bytes read
	/*const int64 numberOfBytesRead = PHYSFS_readBytes(file, textBuilder.GetChars(), textBuilder.Length());
	if (numberOfBytesRead != fileLength)
	{
		UM_LOG(Warning, "Expected to read {} bytes for \"{}\", actually read {}", fileLength, fileName, numberOfBytesRead);
	}*/

	return textBuilder.ReleaseString();
}

/**
 * @brief Defines a base helper class for file tasks.
 *
 * @tparam T The derived task type.
 */
template<typename T>
class TFileTask : public IEventTask
{
public:

	/**
	 * @brief Starts the task.
	 */
	virtual void StartTask() = 0;

protected:

	/** Destructor type for uv_fs_t */
	struct FRequestDestructor
	{
		void Delete(uv_fs_t* handle)
		{
			uv_fs_req_cleanup(handle);
			FMemory::Free(handle);
		}
	};

	template<int32 N = 4096>
	struct FRequestBuffer
	{
		TStaticArray<char, N> BufferData;
		uv_buf_t Buffer;

		/**
		 * @brief Initializes Buffer to point to BufferData.
		 */
		FRequestBuffer()
			: Buffer { uv_buf_init(reinterpret_cast<char*>(BufferData.GetData()), static_cast<uint32>(BufferData.Num())) }
		{
		}
	};

	/** Memory managed handle to a libuv file request */
	using FRequestHandle = TUniquePtr<uv_fs_t, FRequestDestructor>;

	/** Handle to a buffer suitable for use by libuv */
	template<int32 N>
	using FRequestBufferHandle = TUniquePtr<FRequestBuffer<N>>;

	TFileTask() = default;
	virtual ~TFileTask() override = default;

	/**
	 * @brief Dispatches a libuv request.
	 *
	 * @tparam Callback The member function callback.
	 * @param handle The request handle.
	 */
	template<void(T::*Callback)()>
	static void DispatchRequest(uv_fs_t* handle)
	{
		T* task = reinterpret_cast<T*>(handle->data);
		(task->*Callback)();
	}

	/**
	 * @brief Creates a new libuv file request handle.
	 *
	 * @param ownerTask The owner task.
	 * @return The file request handle.
	 */
	static FRequestHandle MakeRequest(T* ownerTask)
	{
		FRequestHandle result { FMemory::AllocateObject<uv_fs_t>() };
		FMemory::ZeroOut(result.Get(), sizeof(uv_fs_t));

		result->data = ownerTask;

		return result;
	}
};

/**
 * @brief Defines an async task for reading a file as text.
 */
class FReadFileTextTask : public TFileTask<FReadFileTextTask>
{
	// TODO(raselneck) This task seems like it can take a very long time... During the File Tests, I'm getting about 7ms in CLion on Windows,
	//                 23ms from Windows Terminal (both with an M.2 SSD), and ~1.5ms on my M2 MacBook Pro just to read the test file. Need to
	//                 test if that is actually this task taking that long, or if it's a side effect of the EventLoop structure

	using Super = TFileTask<FReadFileTextTask>;

public:

	static constexpr int32 ReadBufferSize = 4096;

	/**
	 * @brief Sets default values for this task's properties.
	 *
	 * @param filePath The path to the file.
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	FReadFileTextTask(const FStringView filePath, FFile::FReadTextCallback callback, FFile::FErrorCallback errorCallback)
		: m_CompleteCallback { MoveTemp(callback) }
		, m_ErrorCallback { MoveTemp(errorCallback) }
		, m_FilePath { filePath }
	{
	}

	/**
	 * @brief Destroys this file read task.
	 */
	virtual ~FReadFileTextTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return m_FilePath.Length() > 0 || m_FileHandle != INDEX_NONE;
	}

	/** @inheritdoc TFileTask::StartTask */
	virtual void StartTask() override
	{
		m_OpenRequest = MakeRequest(this);

		uv_loop_t* loop = GetEventLoop()->GetLoop();
		uv_fs_open(loop, m_OpenRequest.Get(), m_FilePath.GetChars(), UV_FS_O_RDONLY, 0, DispatchRequest<&FReadFileTextTask::OnOpen>);
	}

private:

	/**
	 * @brief Called when the associated file is opened.
	 */
	void OnOpen()
	{
		if (m_OpenRequest->result < 0)
		{
			const FStringView errorString { uv_strerror(static_cast<int32>(m_OpenRequest->result)) };
			if (m_ErrorCallback.IsValid())
			{
				m_ErrorCallback(MAKE_ERROR("{}", errorString));
			}
			else
			{
				UM_LOG(Error, "Failed to open file \"{}\" asynchronously. Reason: {}", m_FilePath, errorString);
			}
		}
		else
		{
			m_FileHandle = static_cast<uv_file>(m_OpenRequest->result);
			m_ReadRequest = MakeRequest(this);
			m_ReadBuffer = MakeUnique<FRequestBuffer<ReadBufferSize>>();

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_read(loop, m_ReadRequest.Get(), m_FileHandle, &m_ReadBuffer->Buffer, 1, -1, DispatchRequest<&FReadFileTextTask::OnRead>);
		}

		m_OpenRequest.Reset();
	}

	/**
	 * @brief Called when the associated file has had some data read.
	 */
	void OnRead()
	{
		uv_fs_req_cleanup(m_ReadRequest.Get());

		if (m_ReadRequest->result < 0)
		{
			const FStringView errorString { uv_strerror(static_cast<int32>(m_ReadRequest->result)) };
			m_ReadRequest.Reset();

			if (m_ErrorCallback.IsValid())
			{
				m_ErrorCallback(MAKE_ERROR("{}", errorString));
			}
			else
			{
				UM_LOG(Error, "Failed to read file \"{}\". Reason: {}", m_FilePath, errorString);
			}
		}
		else if (m_ReadRequest->result == 0)
		{
			m_ReadRequest.Reset();
			m_CloseRequest = MakeRequest(this);

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_close(loop, m_CloseRequest.Get(), m_FileHandle, DispatchRequest<&FReadFileTextTask::OnClose>);
		}
		else
		{
			const int32 numReadChars = static_cast<int32>(m_ReadRequest->result);
			m_Text.Append(m_ReadBuffer->BufferData.GetData(), numReadChars);

			// TODO(FIXME) Kinda gross that we're accessing m_Data on the static array directly...
			FMemory::ZeroOutArray(m_ReadBuffer->BufferData.m_Data);

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_read(loop, m_ReadRequest.Get(), m_FileHandle, &m_ReadBuffer->Buffer, 1, -1, DispatchRequest<&FReadFileTextTask::OnRead>);
		}
	}

	/**
	 * @brief Called when the associated file is closed.
	 */
	void OnClose()
	{
		m_CloseRequest.Reset();
		m_FilePath.Reset();
		m_FileHandle = INDEX_NONE;

		m_CompleteCallback(MoveTemp(m_Text));
	}

	FFile::FReadTextCallback m_CompleteCallback;
	FFile::FErrorCallback m_ErrorCallback;
	FRequestHandle m_OpenRequest;
	FRequestHandle m_ReadRequest;
	FRequestBufferHandle<ReadBufferSize> m_ReadBuffer;
	FRequestHandle m_CloseRequest;
	FString m_Text;
	FString m_FilePath;
	uv_file m_FileHandle = INDEX_NONE;
};

void FFile::ReadTextAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadTextCallback callback, FErrorCallback errorCallback)
{
	if (eventLoop.IsNull())
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null event loop when reading file"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when reading file \"{}\"", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null read callback"));
		}
		else
		{
			UM_LOG(Error, "Given null read callback when reading file \"{}\"", filePath);
		}
		return;
	}

	TSharedPtr<FReadFileTextTask> task = eventLoop->AddTask<FReadFileTextTask>(filePath, MoveTemp(callback), MoveTemp(errorCallback));
	task->StartTask();
}

void FFile::Stat(const FStringView fileNameAsView, FFileStats& stats)
{
	const FString fileName { fileNameAsView };
	Stat(fileName, stats);
}

void FFile::Stat(const FString& fileName, FFileStats& stats)
{
	stats = {};
	FNativeFile::StatFile(fileName, stats);

	//DebugPrintFileStats(fileName, stats);
}

/**
 * @brief Defines an async task for getting file stats.
 */
class FStatFileTask : public TFileTask<FStatFileTask>
{
	using Super = TFileTask<FStatFileTask>;

public:

	/**
	 * @brief Sets default values for this task's properties.
	 *
	 * @param filePath The path to the file.
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	FStatFileTask(const FStringView filePath, FFile::FStatCallback callback, FFile::FErrorCallback errorCallback)
		: m_CompleteCallback { MoveTemp(callback) }
		, m_ErrorCallback { MoveTemp(errorCallback) }
		, m_FilePath { filePath }
	{
	}

	/**
	 * @brief Destroys this file stat task.
	 */
	virtual ~FStatFileTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return m_FilePath.Length() > 0;
	}

	/** @inheritdoc TFileTask::StartTask */
	virtual void StartTask() override
	{
		m_StatRequest = MakeRequest(this);

		uv_loop_t* loop = GetEventLoop()->GetLoop();
		uv_fs_stat(loop, m_StatRequest.Get(), m_FilePath.GetChars(), DispatchRequest<&FStatFileTask::OnStat>);
	}

private:

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

	/**
	 * @brief Converts libuv file stats into our own file stats.
	 *
	 * @param statbuf The stat buffer.
	 * @return The converted file stats.
	 */
	static void ConvertFileStats(const uv_stat_t& statbuf, FFileStats& result)
	{
		result.Size = static_cast<int64>(statbuf.st_size);
		result.ModifiedTime = ConvertFileTime(statbuf.st_mtim);
		result.LastAccessTime = ConvertFileTime(statbuf.st_atim);
		result.CreationTime = ConvertFileTime(statbuf.st_ctim);
		result.IsDirectory = HasFlag(statbuf.st_mode, S_IFDIR);
		//result.IsReadOnly = HasFlag(statbuf.st_mode, S_IWUSR) == false;
		result.IsReadOnly = HasFlag(statbuf.st_mode, S_IWRITE) == false;
	}

	/**
	 * @brief Called once stat-ing the file has completed.
	 */
	void OnStat()
	{
		FFileStats stats;
		stats.Exists = m_StatRequest->result >= 0; // TODO May not be accurate

		if (stats.Exists)
		{
			ConvertFileStats(m_StatRequest->statbuf, stats);
		}

		m_CompleteCallback(MoveTemp(stats));

		m_StatRequest.Reset();
		m_FilePath.Reset();
	}

	FFile::FStatCallback m_CompleteCallback;
	FFile::FErrorCallback m_ErrorCallback;
	FRequestHandle m_StatRequest;
	FString m_FilePath;
};

void FFile::StatAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FStatCallback callback, FErrorCallback errorCallback)
{
	if (eventLoop.IsNull())
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null event loop when stat-ing file"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when stat-ing file \"{}\"", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null stat callback"));
		}
		else
		{
			UM_LOG(Error, "Given null stat callback when reading file \"{}\"", filePath);
		}
		return;
	}

	TSharedPtr<FStatFileTask> task = eventLoop->AddTask<FStatFileTask>(filePath, MoveTemp(callback), MoveTemp(errorCallback));
	task->StartTask();
}
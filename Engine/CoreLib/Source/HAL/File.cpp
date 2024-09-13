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
static constexpr int64 GMaxFileLength = TNumericLimits<FString::SizeType>::MaxValue - 1;

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

/**
 * @brief Defines a base helper class for file tasks.
 *
 * @tparam T The derived task type.
 */
template<typename T>
class TFileTask : public IEventTask
{
	using Super = IEventTask;

public:

	/** Destructor type for uv_fs_t */
	struct FRequestDestructor
	{
		void Delete(uv_fs_t* handle)
		{
			uv_fs_req_cleanup(handle);
			FMemory::Free(handle);
		}
	};

	/** Memory managed handle to a libuv file request */
	using FRequestHandle = TUniquePtr<uv_fs_t, FRequestDestructor>;

	using Super::GetEventLoop;

	/**
	 * @brief Set default values for this file task's properties.
	 */
	TFileTask() = default;

	/**
	 * @brief Destroys this file task.
	 */
	virtual ~TFileTask() override = default;

	/**
	 * @brief Starts the task.
	 */
	virtual void StartTask() = 0;

protected:

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
		using RequestHandleType = typename TFileTask<T>::FRequestHandle;

		RequestHandleType result { FMemory::AllocateObject<uv_fs_t>() };
		FMemory::ZeroOut(result.Get(), sizeof(uv_fs_t));

		result->data = ownerTask;

		return result;
	}
};

/**
 * @brief Defines an async task for reading a file.
 */
template<typename ResultType, typename DataType, typename CallbackType, int32 RequestBufferSize = 4096>
class TReadFileTask : public TFileTask<TReadFileTask<ResultType, DataType, CallbackType>>
{
	// TODO(raselneck) This task seems like it can take a very long time... During the File Tests, I'm getting about 7ms in CLion on Windows,
	//                 23ms from Windows Terminal (both with an M.2 SSD), and ~1.5ms on my M2 MacBook Pro just to read the test file. Need to
	//                 test if that is actually this task taking that long, or if it's a side effect of the EventLoop structure

	using ThisClass = TReadFileTask;
	using Super = TFileTask<TReadFileTask<ResultType, DataType, CallbackType>>;
	using typename Super::FRequestHandle;

public:

	/**
	 * @brief Defines a wrapper for a libuv buffer that writes to a static array.
	 */
	struct FRequestBuffer
	{
		TStaticArray<DataType, RequestBufferSize> BufferData;
		uv_buf_t Buffer;

		/**
		 * @brief Initializes Buffer to point to BufferData.
		 */
		FRequestBuffer()
			: Buffer { uv_buf_init(reinterpret_cast<char*>(BufferData.GetData()), static_cast<uint32>(BufferData.Num())) }
		{
		}
	};

	/** Handle to a buffer suitable for use by libuv */
	using FRequestBufferHandle = TUniquePtr<FRequestBuffer>;

	using Super::GetEventLoop;

	/**
	 * @brief Sets default values for this task's properties.
	 *
	 * @param filePath The path to the file.
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	TReadFileTask(const FStringView filePath, CallbackType callback, FFile::FErrorCallback errorCallback)
		: m_CompleteCallback { MoveTemp(callback) }
		, m_ErrorCallback { MoveTemp(errorCallback) }
		, m_FilePath { filePath }
	{
	}

	/**
	 * @brief Destroys this file read task.
	 */
	virtual ~TReadFileTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return m_FilePath.Length() > 0 || m_FileHandle != INDEX_NONE;
	}

	/** @inheritdoc TFileTask::StartTask */
	virtual void StartTask() override
	{
		m_OpenRequest = Super::MakeRequest(this);

		uv_loop_t* loop = GetEventLoop()->GetLoop();
		uv_fs_cb callback = Super::template DispatchRequest<&ThisClass::OnOpen>;
		uv_fs_open(loop, m_OpenRequest.Get(), m_FilePath.GetChars(), UV_FS_O_RDONLY, 0, callback);
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
			m_ReadRequest = Super::MakeRequest(this);
			m_ReadBuffer = MakeUnique<FRequestBuffer>();

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_cb callback = Super::template DispatchRequest<&ThisClass::OnRead>;
			uv_fs_read(loop, m_ReadRequest.Get(), m_FileHandle, &m_ReadBuffer->Buffer, 1, -1, callback);
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
			m_CloseRequest = Super::MakeRequest(this);

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_cb callback = Super::template DispatchRequest<&ThisClass::OnClose>;
			uv_fs_close(loop, m_CloseRequest.Get(), m_FileHandle, callback);
		}
		else
		{
			const int32 numReadBytes = static_cast<int32>(m_ReadRequest->result);
			m_Result.Append(m_ReadBuffer->BufferData.GetData(), numReadBytes);

			// TODO(FIXME) Kinda gross that we're accessing m_Data on the static array directly...
			FMemory::ZeroOutArray(m_ReadBuffer->BufferData.m_Data);

			uv_loop_t* loop = GetEventLoop()->GetLoop();
			uv_fs_cb callback = Super::template DispatchRequest<&ThisClass::OnRead>;
			uv_fs_read(loop, m_ReadRequest.Get(), m_FileHandle, &m_ReadBuffer->Buffer, 1, -1, callback);
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

		m_CompleteCallback(MoveTemp(m_Result));
	}

	CallbackType m_CompleteCallback;
	FFile::FErrorCallback m_ErrorCallback;
	FRequestHandle m_OpenRequest;
	FRequestHandle m_ReadRequest;
	FRequestBufferHandle m_ReadBuffer;
	FRequestHandle m_CloseRequest;
	ResultType m_Result;
	FString m_FilePath;
	uv_file m_FileHandle = INDEX_NONE;
};

using FReadFileBytesTask = TReadFileTask<TArray<uint8>, uint8, FFile::FReadBytesCallback>;
using FReadFileTextTask = TReadFileTask<FString, char, FFile::FReadTextCallback>;

/**
 * @brief Defines an async task for getting file stats.
 */
class FStatFileTask : public TFileTask<FStatFileTask>
{
	using ThisClass = FStatFileTask;
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
		uv_fs_cb callback = DispatchRequest<&ThisClass::OnStat>;
		uv_fs_stat(loop, m_StatRequest.Get(), m_FilePath.GetChars(), callback);
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

template<typename T>
struct TDataBufferInitializer;

template<>
struct TDataBufferInitializer<FString>
{
	static uv_buf_t Initialize(FString& data)
	{
		return uv_buf_init(data.GetChars(), static_cast<size_t>(data.Length()));
	}
};

template<>
struct TDataBufferInitializer<TArray<uint8>>
{
	static uv_buf_t Initialize(TArray<uint8>& data)
	{
		return uv_buf_init(reinterpret_cast<char*>(data.GetData()), static_cast<size_t>(data.Num()));
	}
};

/**
 * @brief Defines an async task for writing to a file.
 *
 * @tparam DataType The type of the data being written.
 */
template<typename DataType>
class TWriteFileTask : public TFileTask<TWriteFileTask<DataType>>
{
public:

	/**
	 * @brief Sets values for this task's properties.
	 *
	 * @tparam DataInitializerType The type of \p data, used to initialize m_Data.
	 * @param filePath The path to the file.
	 * @param data The data to write.
	 * @param callback The function to call once the task is complete.
	 */
	template<typename DataInitializerType>
	TWriteFileTask(const FStringView filePath, const DataInitializerType data, FFile::FWriteCallback callback)
		: m_Callback { MoveTemp(callback) }
		, m_Data { data }
		, m_FilePath { filePath }
	{
		m_DataBuffer = TDataBufferInitializer<DataType>::Initialize(m_Data);
	}

	/**
	 * @brief Destroys this task.
	 */
	virtual ~TWriteFileTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return m_FilePath.Length() > 0;
	}

	/** @inheritdoc TFileTask::StartTask */
	virtual void StartTask() override
	{
	}

private:

	FFile::FWriteCallback m_Callback;
	DataType m_Data;
	uv_buf_t m_DataBuffer;
	FString m_FilePath;
};

using FWriteFileBytesTask = TWriteFileTask<TArray<uint8>>;
using FWriteFileTextTask = TWriteFileTask<FString>;

// TODO FWriteFileLinesTask, and use a TStaticArray of eight total buffers. Four for lines and four for newline strings, interlaced

class FWriteFileLinesTask : public TFileTask<FWriteFileLinesTask>
{
public:

	FWriteFileLinesTask(const FStringView filePath, TArray<FString> lines, FFile::FWriteCallback callback)
		: m_Lines { MoveTemp(lines) }
		, m_Callback { MoveTemp(callback) }
		, m_FilePath { filePath }
	{
	}

	/**
	 * @brief Destroys this task.
	 */
	virtual ~FWriteFileLinesTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return m_FilePath.Length() > 0;
	}

	/** @inheritdoc TFileTask::StartTask */
	virtual void StartTask() override
	{
	}

private:

	TStaticArray<uv_buf_t, 8> m_DataBuffers;
	TArray<FString> m_Lines;
	FFile::FWriteCallback m_Callback;
	FString m_FilePath;
	int32 m_LineIndex = 0;
};

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

void FFile::ReadBytesAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadBytesCallback callback, FErrorCallback errorCallback)
{
	if (eventLoop.IsNull())
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null event loop when reading file as byte array"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when reading file \"{}\" as byte array", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null callback when reading file \"{}\" as byte array", filePath));
		}
		else
		{
			UM_LOG(Error, "Given null callback when reading file \"{}\" as byte array", filePath);
		}
		return;
	}

	TSharedPtr<FReadFileBytesTask> task = eventLoop->AddTask<FReadFileBytesTask>(filePath, MoveTemp(callback), MoveTemp(errorCallback));
	task->StartTask();
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

void FFile::ReadTextAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadTextCallback callback, FErrorCallback errorCallback)
{
	if (eventLoop.IsNull())
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null event loop when reading file as string"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when reading file \"{}\" as string", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null callback when reading file \"{}\" as string", filePath));
		}
		else
		{
			UM_LOG(Error, "Given null callback when reading file \"{}\" as string", filePath);
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
}

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

TErrorOr<void> FFile::WriteBytes(const FStringView filePath, const TSpan<const uint8> bytes)
{
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenWrite(filePath);
	if (fileStream.IsNull())
	{
		return MAKE_ERROR("Failed to open \"{}\" for writing", filePath);
	}

	fileStream->Write(bytes);

	return {};
}

void FFile::WriteBytesAsync(const FStringView filePath, const TSpan<const uint8> bytes, const TSharedPtr<FEventLoop>& eventLoop, FWriteCallback callback)
{
	if (eventLoop.IsNull())
	{
		if (callback.IsValid())
		{
			callback.Invoke(MAKE_ERROR("Given null event loop when writing bytes to file"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when writing bytes to file \"{}\"", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		callback = [](TErrorOr<void>) {};
	}

	TSharedPtr<FWriteFileBytesTask> task = eventLoop->AddTask<FWriteFileBytesTask>(filePath, bytes, MoveTemp(callback));
	task->StartTask();
}

TErrorOr<void> FFile::WriteLines(const FStringView filePath, const TSpan<const FString> lines)
{
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenWrite(filePath);
	if (fileStream.IsNull())
	{
		return MAKE_ERROR("Failed to open \"{}\" for writing", filePath);
	}

	for (const FStringView line : lines)
	{
		fileStream->Write(line);
		fileStream->Write("\n"_sv);
	}

	return {};
}

void FFile::WriteLinesAsync(FStringView filePath, TArray<FString> lines, const TSharedPtr<FEventLoop>& eventLoop, FWriteCallback callback)
{
	if (eventLoop.IsNull())
	{
		if (callback.IsValid())
		{
			callback.Invoke(MAKE_ERROR("Given null event loop when writing lines to file"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when writing lines to file \"{}\"", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		callback = [](TErrorOr<void>) {};
	}

	TSharedPtr<FWriteFileLinesTask> task = eventLoop->AddTask<FWriteFileLinesTask>(filePath, MoveTemp(lines), MoveTemp(callback));
	task->StartTask();
}

TErrorOr<void> FFile::WriteText(const FStringView filePath, const FStringView text)
{
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenWrite(filePath);
	if (fileStream.IsNull())
	{
		return MAKE_ERROR("Failed to open \"{}\" for writing", filePath);
	}

	fileStream->Write(text);

	return {};
}

void FFile::WriteTextAsync(const FStringView filePath, const FStringView text, const TSharedPtr<FEventLoop>& eventLoop, FWriteCallback callback)
{
	if (eventLoop.IsNull())
	{
		if (callback.IsValid())
		{
			callback.Invoke(MAKE_ERROR("Given null event loop when writing text to file"));
		}
		else
		{
			UM_LOG(Error, "Given null event loop when writing text to file \"{}\"", filePath);
		}
		return;
	}

	if (callback.IsValid() == false)
	{
		callback = [](TErrorOr<void>) {};
	}

	TSharedPtr<FWriteFileTextTask> task = eventLoop->AddTask<FWriteFileTextTask>(filePath, text, MoveTemp(callback));
	task->StartTask();
}
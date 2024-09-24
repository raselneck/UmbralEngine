#include "Containers/StaticArray.h"
#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include "HAL/File.h"
#include "HAL/FileSystem.h"
#include "HAL/FileTask.h"
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
 * @brief Defines an async task for reading a file.
 */
template<typename ResultType, typename DataType, typename CallbackType, int32 RequestBufferSize = 4096>
class TReadFileTask : public FFileTask
{
	// TODO(raselneck) This task seems like it can take a very long time... During the File Tests, I'm getting about 7ms in CLion on Windows,
	//                 23ms from Windows Terminal (both with an M.2 SSD), and ~1.5ms on my M2 MacBook Pro just to read the test file. Need to
	//                 test if that is actually this task taking that long, or if it's a side effect of the EventLoop structure

	using ThisClass = TReadFileTask;
	using Super = FFileTask;

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
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	TReadFileTask(CallbackType callback, FFile::FErrorCallback errorCallback)
		: m_CompleteCallback { MoveTemp(callback) }
		, m_ErrorCallback { MoveTemp(errorCallback) }
	{
	}

	/**
	 * @brief Destroys this file read task.
	 */
	virtual ~TReadFileTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		const FStringView filePath = GetFilePath();
		return filePath.Length() > 0 || GetFileHandle() != INDEX_NONE;
	}

	/**
	 * @brief Begins reading the given file.
	 *
	 * @param filePath The path to the file.
	 */
	void ReadFile(const FStringView filePath)
	{
		OpenFile(filePath, UV_FS_O_RDONLY, 0);
	}

private:

	virtual void OnError(FError error) override
	{
		if (m_ErrorCallback.IsValid())
		{
			m_ErrorCallback(MoveTemp(error));
		}
	}

	virtual void OnFileClosed() override
	{
		m_CompleteCallback(MoveTemp(m_Result));
	}

	virtual void OnFileOpened() override
	{
		m_ReadRequest = MakeRequest();
		m_ReadBuffer = MakeUnique<FRequestBuffer>();

		uv_loop_t* loop = GetLoop();
		uv_fs_cb callback = DispatchRequestCallback<ThisClass, &ThisClass::OnFileRead>;
		uv_fs_read(loop, m_ReadRequest.Get(), GetFileHandle(), &m_ReadBuffer->Buffer, 1, -1, callback);
	}

	/**
	 * @brief Called when the associated file has had some data read.
	 */
	void OnFileRead()
	{
		uv_fs_req_cleanup(m_ReadRequest.Get());

		if (m_ReadRequest->result < 0)
		{
			const FStringView errorString { uv_strerror(static_cast<int32>(m_ReadRequest->result)) };
			m_ReadRequest.Reset();

			OnError(MAKE_ERROR("{}", errorString));

			// TODO Does the file need to be closed?
		}
		else if (m_ReadRequest->result == 0)
		{
			m_ReadRequest.Reset();

			CloseFile();
		}
		else
		{
			const int32 numReadBytes = static_cast<int32>(m_ReadRequest->result);
			m_Result.Append(m_ReadBuffer->BufferData.GetData(), numReadBytes);

			// TODO(FIXME) Kinda gross that we're accessing m_Data on the static array directly...
			FMemory::ZeroOutArray(m_ReadBuffer->BufferData.m_Data);

			uv_loop_t* loop = GetLoop();
			uv_fs_cb callback = DispatchRequestCallback<ThisClass, &ThisClass::OnFileRead>;
			uv_fs_read(loop, m_ReadRequest.Get(), GetFileHandle(), &m_ReadBuffer->Buffer, 1, -1, callback);
		}
	}

	CallbackType m_CompleteCallback;
	FFile::FErrorCallback m_ErrorCallback;
	FRequestHandle m_ReadRequest;
	FRequestBufferHandle m_ReadBuffer;
	ResultType m_Result;
};

using FReadFileBytesTask = TReadFileTask<TArray<uint8>, uint8, FFile::FReadBytesCallback>;
using FReadFileTextTask = TReadFileTask<FString, char, FFile::FReadTextCallback>;

/**
 * @brief Defines an async task for getting file stats.
 */
class FStatFileTask : public FFileTask
{
	using ThisClass = FStatFileTask;
	using Super = FFileTask;

public:

	/**
	 * @brief Sets default values for this task's properties.
	 *
	 * @param filePath The path to the file.
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	FStatFileTask(FFile::FStatCallback callback)
		: m_CompleteCallback { MoveTemp(callback) }
	{
	}

	/**
	 * @brief Destroys this file stat task.
	 */
	virtual ~FStatFileTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return GetFilePath().Length() > 0;
	}

	using Super::StatFile;

private:

	virtual void OnFileStat(FFileStats stats) override
	{
		m_CompleteCallback(MoveTemp(stats));
	}

	FFile::FStatCallback m_CompleteCallback;
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
class TWriteFileTask : public FFileTask
{
	using ThisClass = TWriteFileTask;
	using Super = FFileTask;

public:

	/**
	 * @brief Sets values for this task's properties.
	 *
	 * @tparam DataInitializerType The type of \p data, used to initialize m_Data.
	 * @param data The data to write.
	 * @param callback The function to call once the task is complete.
	 */
	TWriteFileTask(DataType data, FFile::FWriteCallback callback)
		: m_Callback { MoveTemp(callback) }
		, m_Data { MoveTemp(data) }
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
		return GetFilePath().Length() > 0;
	}

	/**
	 * @brief Begins writing data to the given file.
	 *
	 * @param filePath The path to the file.
	 */
	void WriteToFile(const FStringView filePath)
	{
		constexpr int32 openFlags = O_CREAT | O_TRUNC | O_WRONLY;
		constexpr int32 openMode = S_IRUSR | S_IWUSR;
		OpenFile(filePath, openFlags, openMode);
	}

private:

	virtual void OnError(FError error) override
	{
		if (m_Callback.IsValid())
		{
			m_Callback(MoveTemp(error));
		}
		else
		{
			// TODO Need to log some more context here
			UM_LOG(Error, "{}", error.GetMessage());
		}
	}

	virtual void OnFileClosed() override
	{
		// TODO Check for errors
		m_Callback({});
	}

	virtual void OnFileOpened() override
	{
		m_WriteRequest = MakeRequest();

		uv_loop_t* loop = GetLoop();
		uv_fs_cb callback = DispatchRequestCallback<ThisClass, &ThisClass::OnWrite>;
		uv_fs_write(loop, m_WriteRequest.Get(), GetFileHandle(), &m_DataBuffer, 1, 0, callback);
	}

	/**
	 * @brief Called when the associated file has had some lines written to it.
	 */
	void OnWrite()
	{
		const int64 result = m_WriteRequest->result;
		m_WriteRequest.Reset();

		if (result < 0)
		{
			const FStringView errorString { uv_strerror(static_cast<int32>(result)) };
			OnError(MAKE_ERROR("{}", errorString));
		}

		CloseFile();
	}

	FFile::FWriteCallback m_Callback;
	DataType m_Data;
	uv_buf_t m_DataBuffer;
	FRequestHandle m_WriteRequest;
};

using FWriteFileBytesTask = TWriteFileTask<TArray<uint8>>;
using FWriteFileTextTask = TWriteFileTask<FString>;

/**
 * @brief Defines an async task for writing a collection of lines to a file.
 */
class FWriteFileLinesTask : public FFileTask
{
	using ThisClass = FWriteFileLinesTask;
	using Super = FFileTask;

public:

	FWriteFileLinesTask(TArray<FString> lines, FFile::FWriteCallback callback)
		: m_Lines { MoveTemp(lines) }
		, m_Callback { MoveTemp(callback) }
	{
	}

	/**
	 * @brief Destroys this task.
	 */
	virtual ~FWriteFileLinesTask() override = default;

	/** @inheritdoc IEventTask::IsRunning */
	[[nodiscard]] virtual bool IsRunning() const override
	{
		return GetFilePath().Length() > 0;
	}

	/**
	 * @brief Begins writing all of the lines to the given file.
	 *
	 * @param filePath The path to the file.
	 */
	void WriteToFile(const FStringView filePath)
	{
		constexpr int32 openFlags = O_CREAT | O_TRUNC | O_WRONLY;
		constexpr int32 openMode = S_IRUSR | S_IWUSR;
		OpenFile(filePath, openFlags, openMode);
	}

private:

	static inline char NewLineChar = '\n';

	virtual void OnError(FError error) override
	{
		if (m_Callback.IsValid())
		{
			m_Callback(MoveTemp(error));
		}
		else
		{
			// TODO Need to log some more context here
			UM_LOG(Error, "{}", error.GetMessage());
		}
	}

	virtual void OnFileClosed() override
	{
		// TODO Check for errors
		m_Callback({});
	}

	virtual void OnFileOpened() override
	{
		m_WriteRequest = MakeRequest();

		m_LineBuffers.AddZeroed(m_Lines.Num() * 2);
		for (int32 idx = 0; idx < m_Lines.Num(); ++idx)
		{
			m_LineBuffers[idx * 2 + 0] = uv_buf_init(m_Lines[idx].GetChars(), static_cast<uint32>(m_Lines[idx].Length()));
			m_LineBuffers[idx * 2 + 1] = uv_buf_init(&NewLineChar, 1);
		}

		uv_loop_t* loop = GetLoop();
		uv_fs_cb callback = DispatchRequestCallback<ThisClass, &ThisClass::OnWrite>;
		uv_fs_write(loop, m_WriteRequest.Get(), GetFileHandle(), m_LineBuffers.GetData(), static_cast<uint32>(m_LineBuffers.Num()), 0, callback);
	}

	/**
	 * @brief Called when the associated file has had some lines written to it.
	 */
	void OnWrite()
	{
		const int64 result = m_WriteRequest->result;
		m_WriteRequest.Reset();

		if (result < 0)
		{
			const FStringView errorString { uv_strerror(static_cast<int32>(result)) };
			OnError(MAKE_ERROR("{}", errorString));
		}

		CloseFile();
	}

	TArray<FString> m_Lines;
	TArray<uv_buf_t> m_LineBuffers;
	FFile::FWriteCallback m_Callback;
	FRequestHandle m_WriteRequest;
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

	TSharedPtr<FReadFileBytesTask> task = eventLoop->AddTask<FReadFileBytesTask>(MoveTemp(callback), MoveTemp(errorCallback));
	task->ReadFile(filePath);
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

	TSharedPtr<FReadFileTextTask> task = eventLoop->AddTask<FReadFileTextTask>(MoveTemp(callback), MoveTemp(errorCallback));
	task->ReadFile(filePath);
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

void FFile::StatAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FStatCallback callback)
{
	if (eventLoop.IsNull())
	{
		UM_LOG(Error, "Given null event loop when stat-ing file \"{}\"", filePath);
		return;
	}

	if (callback.IsValid() == false)
	{
		UM_LOG(Error, "Given null stat callback when reading file \"{}\"", filePath);
		return;
	}

	TSharedPtr<FStatFileTask> task = eventLoop->AddTask<FStatFileTask>(MoveTemp(callback));
	task->StatFile(filePath);
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
	TArray<uint8> bytesAsArray { bytes };
	WriteBytesAsync(filePath, MoveTemp(bytesAsArray), eventLoop, MoveTemp(callback));
}

void FFile::WriteBytesAsync(const FStringView filePath, TArray<uint8> bytes, const TSharedPtr<FEventLoop>& eventLoop, FWriteCallback callback)
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

	TSharedPtr<FWriteFileBytesTask> task = eventLoop->AddTask<FWriteFileBytesTask>(MoveTemp(bytes), MoveTemp(callback));
	task->WriteToFile(filePath);
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

	TSharedPtr<FWriteFileLinesTask> task = eventLoop->AddTask<FWriteFileLinesTask>(MoveTemp(lines), MoveTemp(callback));
	task->WriteToFile(filePath);
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
	FString textAsString { text };
	WriteTextAsync(filePath, MoveTemp(textAsString), eventLoop, MoveTemp(callback));
}

void FFile::WriteTextAsync(const FStringView filePath, FString text, const TSharedPtr<FEventLoop>& eventLoop, FWriteCallback callback)
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

	TSharedPtr<FWriteFileTextTask> task = eventLoop->AddTask<FWriteFileTextTask>(MoveTemp(text), MoveTemp(callback));
	task->WriteToFile(filePath);
}
#include "Containers/StaticArray.h"
#include "Engine/Logging.h"
#include "HAL/File.h"
#include "HAL/EventLoop.h"
#include "HAL/FileSystem.h"
#include "Misc/StringBuilder.h"
#include "Templates/NumericLimits.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsFileSystem.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleFileSystem.h"
#else
#	include "HAL/Linux/LinuxFileSystem.h"
#endif
#include <uv.h>

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
 * @brief Defines an async task for reading a file as text.
 */
class FReadFileTextTask : public IEventTask
{
	struct FRequestDestructor
	{
		void Delete(uv_fs_t* handle)
		{
			uv_fs_req_cleanup(handle);
			FMemory::Free(handle);
		}
	};

	struct FRequestData
	{
		TStaticArray<uint8, 4096> BufferData;
		uv_buf_t Buffer;

		/**
		 * @brief Initializes Buffer to point to BufferData.
		 */
		FRequestData()
		{
			uv_buf_init(reinterpret_cast<char*>(BufferData.GetData()), static_cast<uint32>(BufferData.Num()));
		}
	};

	using FRequestHandle = TUniquePtr<uv_fs_t, FRequestDestructor>;
	using FRequestDataHandle = TUniquePtr<FRequestData>;

public:

	/**
	 * @brief Sets default values for this task's properties.
	 *
	 * @param callback The function to call when complete.
	 * @param errorCallback The function to call when an error is encountered.
	 */
	FReadFileTextTask(FFile::FReadTextCallback callback, FFile::FReadErrorCallback errorCallback)
		: m_CompleteCallback { MoveTemp(callback) }
		, m_ErrorCallback { MoveTemp(errorCallback) }
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

	/**
	 * @brief Starts asynchronously reading a file.
	 *
	 * @param filePath The path to the file.
	 */
	void StartReadingFile(const FStringView filePath)
	{
		m_FilePath = FString { filePath };
		m_OpenRequest = MakeRequest(this);

		uv_loop_t* loop = GetEventLoop()->GetLoop();
		uv_fs_open(loop, m_OpenRequest.Get(), m_FilePath.GetChars(), UV_FS_O_RDONLY, 0, DispatchRequest<&FReadFileTextTask::OnOpen>);
	}

private:

	/**
	 * @brief Dispatches a libuv request.
	 *
	 * @tparam Callback The member function callback.
	 * @param handle The request handle.
	 */
	template<void(FReadFileTextTask::*Callback)()>
	static void DispatchRequest(uv_fs_t* handle)
	{
		FReadFileTextTask* task = reinterpret_cast<FReadFileTextTask*>(handle->data);
		(task->*Callback)();
	}

	/**
	 * @brief Creates a new libuv file request handle.
	 *
	 * @param ownerTask The owner task.
	 * @return The file request handle.
	 */
	static FRequestHandle MakeRequest(FReadFileTextTask* ownerTask)
	{
		FRequestHandle result { FMemory::AllocateObject<uv_fs_t>() };
		FMemory::ZeroOut(result.Get(), sizeof(uv_fs_t));

		result->data = ownerTask;

		return result;
	}

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
			m_ReadBuffer = MakeUnique<FRequestData>();

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
			// TODO Append to m_Text using m_ReadBuffer :)

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

	FRequestHandle m_OpenRequest;
	FRequestHandle m_ReadRequest;
	FRequestDataHandle m_ReadBuffer;
	FRequestHandle m_CloseRequest;
	FFile::FReadTextCallback m_CompleteCallback;
	FFile::FReadErrorCallback m_ErrorCallback;
	FString m_Text;
	FString m_FilePath;
	uv_file m_FileHandle = INDEX_NONE;
};

void FFile::ReadTextAsync(const FStringView filePath, const TSharedPtr<FEventLoop>& eventLoop, FReadTextCallback callback, FReadErrorCallback errorCallback)
{
	if (eventLoop.IsNull())
	{
		if (errorCallback.IsValid())
		{
			errorCallback.Invoke(MAKE_ERROR("Given null event loop"));
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

	TSharedPtr<FReadFileTextTask> task = eventLoop->AddTask<FReadFileTextTask>(MoveTemp(callback), MoveTemp(errorCallback));
	task->StartReadingFile(filePath);
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
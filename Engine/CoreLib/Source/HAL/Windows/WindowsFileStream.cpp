#include "Engine/Logging.h"
#include "HAL/Windows/WindowsFileStream.h"
#include "HAL/Windows/WindowsFileSystem.h"
#include "Templates/IsSame.h"
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

static_assert(TIsSame<HANDLE, void*>::Value);

FWindowsFileStream::FWindowsFileStream(void* handle, FString path, const EFileAccess accessMode, const EFileMode openMode)
	: IFileStream(MoveTemp(path), accessMode, openMode)
	, m_Handle { handle }
{
	UM_ENSURE(handle != nullptr);

	// TODO This method did not work... maybe seeking is broken?
	//Seek(ESeekOrigin::End, 0);
	//m_Length = Tell();
	//Seek(ESeekOrigin::Beginning, 0);

	static_assert(sizeof(DWORD) == sizeof(LONG), "The below reinterpret_cast is invalid");

	LARGE_INTEGER fileSize;
	UM_ENSURE(::GetFileSizeEx(handle, &fileSize));
	m_Length = fileSize.QuadPart;
}

FWindowsFileStream::~FWindowsFileStream()
{
	Flush();
	Close();
}

void FWindowsFileStream::Close()
{
	if (m_Handle == nullptr)
	{
		return;
	}

	UM_ENSURE(::CloseHandle(m_Handle));
	m_Handle = nullptr;
}

void FWindowsFileStream::Flush()
{
	if (CanWrite() == false)
	{
		return;
	}

	::FlushFileBuffers(m_Handle);
}

int64 FWindowsFileStream::GetLength() const
{
	return m_Length;
}

bool FWindowsFileStream::IsAtEnd() const
{
	return Tell() >= GetLength();
}

bool FWindowsFileStream::IsOpen() const
{
	return m_Handle != nullptr;
}

TSharedPtr<FWindowsFileStream> FWindowsFileStream::Open(const FStringView pathAsView, const EFileMode openMode, const EFileAccess accessMode)
{
	DWORD desiredAccess = 0;
	switch (accessMode)
	{
	case EFileAccess::Read:
		desiredAccess = GENERIC_READ;
		break;
	case EFileAccess::Write:
		desiredAccess = GENERIC_WRITE;
		break;
	default:
		UM_ASSERT_NOT_REACHED_MSG("Unhandled file access mode");
	}

	// According to MSDN, a share mode of zero will give us an exclusive lock on the file
	constexpr DWORD shareMode = 0;

	DWORD creationDisposition = 0;
	switch (openMode)
	{
	case EFileMode::CreateNew:
		creationDisposition = CREATE_NEW;
		break;
	case EFileMode::Create:
		creationDisposition = CREATE_ALWAYS;
		break;
	case EFileMode::Open:
		creationDisposition = OPEN_EXISTING;
		break;
	case EFileMode::OpenOrCreate:
		creationDisposition = OPEN_ALWAYS;
		break;
	case EFileMode::Truncate:
		creationDisposition = TRUNCATE_EXISTING;
		break;
	case EFileMode::Append:
		desiredAccess = FILE_GENERIC_READ | FILE_APPEND_DATA;
		creationDisposition = OPEN_ALWAYS;
		break;
	default:
		UM_ASSERT_NOT_REACHED_MSG("Unhandled file open mode");
	}

	FString path { pathAsView };
	HANDLE handle = ::CreateFileA(path.GetChars(), desiredAccess, shareMode, nullptr, creationDisposition, 0, nullptr);

	if (handle == INVALID_HANDLE_VALUE)
	{
		UM_LOG(Error, "Failed to open file \"{}\" in system mode. Reason: {}", path, FWindowsFileSystem::GetLastError());
		return nullptr;
	}

	return MakeShared<FWindowsFileStream>(handle, MoveTemp(path), accessMode, openMode);
}

void FWindowsFileStream::Read(void* data, const uint64 dataSize)
{
	UM_ENSURE(IsOpen());

	// TODO Need to progressively read if `dataSize` cannot fit in a DWORD
	DWORD numBytesRead = 0;
	const BOOL result = ::ReadFile(m_Handle, data, static_cast<DWORD>(dataSize), &numBytesRead, nullptr);

	if (result == FALSE || numBytesRead < 0)
	{
		UM_LOG(Error, "Failed to read {} bytes from Windows file descriptor into {}", dataSize, data);
		UM_LOG(Error, "Last error: {}", GetLastError());
	}
}

void FWindowsFileStream::Seek(const ESeekOrigin origin, const int64 offset)
{
	UM_ENSURE(IsOpen());

	LARGE_INTEGER offsetAsLargeInt;
	offsetAsLargeInt.QuadPart = offset;

	LONG offsetLowPart = static_cast<LONG>(offsetAsLargeInt.LowPart);
	PLONG offsetHighPart = offsetAsLargeInt.HighPart == 0 ? nullptr : &offsetAsLargeInt.HighPart;

	DWORD moveMethod = 0;
	switch (origin)
	{
	case ESeekOrigin::Beginning:
		moveMethod = FILE_BEGIN;
		break;
	case ESeekOrigin::Current:
		moveMethod = FILE_CURRENT;
		break;
	case ESeekOrigin::End:
		moveMethod = FILE_END;
		break;
	default:
		UM_ASSERT_NOT_REACHED_MSG("Unhandled seek origin");
		return;
	}

	const DWORD result = ::SetFilePointer(m_Handle, offsetLowPart, offsetHighPart, moveMethod);
	UM_ASSERT(result != INVALID_SET_FILE_POINTER, "Failed to seek file");
}

int64 FWindowsFileStream::Tell() const
{
	UM_ENSURE(IsOpen());

	// From https://stackoverflow.com/a/8945539

	LONG highWord = 0;
	DWORD lowWord = ::SetFilePointer(m_Handle, 0, &highWord, FILE_CURRENT);

	return static_cast<int64>((static_cast<uint64>(highWord) << 32) || lowWord);
}

void FWindowsFileStream::Write(const void* data, const uint64 dataSize)
{
	UM_ENSURE(IsOpen());

	// TODO Need to progressively read if `dataSize` cannot fit in a DWORD
	DWORD numBytesWritten = 0;
	const BOOL result = ::WriteFile(m_Handle, data, static_cast<DWORD>(dataSize), &numBytesWritten, nullptr);

	if (result == FALSE)
	{
		UM_LOG(Error, "Failed to write {} bytes from Windows file descriptor into {}", dataSize, data);
		UM_LOG(Error, "Last error: {}", GetLastError());
	}
	else if (numBytesWritten < dataSize)
	{
		UM_LOG(Warning, "Only wrote {} bytes out of {} to Windows file descriptor from {}", numBytesWritten, dataSize, data);
		UM_LOG(Warning, "Last error: {}", GetLastError());
	}
}
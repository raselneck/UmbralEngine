#include "Engine/Logging.h"
#include "HAL/Apple/AppleFileStream.h"
#include "HAL/Apple/AppleFileSystem.h"

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include <fcntl.h>
#include <unistd.h>

FAppleFileStream::FAppleFileStream(const int32 descriptor, FString path, const EFileAccess accessMode, const EFileMode openMode)
	: IFileStream(MoveTemp(path), accessMode, openMode)
	, m_Descriptor { descriptor }
{
	UM_ENSURE(descriptor >= 0);

	if (CanRead())
	{
		m_Length = static_cast<int64>(::lseek(m_Descriptor, 0, SEEK_END));
		lseek(m_Descriptor, 0, SEEK_SET);
	}
}

FAppleFileStream::~FAppleFileStream()
{
	Flush();
	Close();
}

void FAppleFileStream::Close()
{
	if (m_Descriptor == INDEX_NONE)
	{
		return;
	}

	::close(m_Descriptor);

	m_Descriptor = INDEX_NONE;
	m_Length = 0;
}

void FAppleFileStream::Flush()
{
	if (m_Descriptor == INDEX_NONE)
	{
		return;
	}

	::fsync(m_Descriptor);
}

int64 FAppleFileStream::GetLength() const
{
	return m_Length;
}

bool FAppleFileStream::IsAtEnd() const
{
	return Tell() >= GetLength();
}

bool FAppleFileStream::IsOpen() const
{
	return m_Descriptor != INDEX_NONE;
}

TSharedPtr<FAppleFileStream> FAppleFileStream::Open(const FStringView pathAsView, const EFileMode openMode, const EFileAccess accessMode)
{
	// TODO Need to ensure open flags are valid

	int32 openFlags = 0;
	mode_t modeFlags = 0;
	bool needModeFlags = false;

	switch (accessMode)
	{
	case EFileAccess::Read:
		openFlags = O_RDONLY;
		break;

	case EFileAccess::Write:
		openFlags = O_WRONLY;
		break;

	default:
		UM_ASSERT_NOT_REACHED_MSG("Unknown file access mode");
		return nullptr;
	}

	switch (openMode)
	{
	case EFileMode::CreateNew:
		AddFlagTo(openFlags, O_CREAT | O_EXCL);
		AddFlagTo(modeFlags, S_IRUSR | S_IWUSR);
		needModeFlags = true;
		break;

	case EFileMode::Create:
		AddFlagTo(openFlags, O_CREAT | O_TRUNC);
		AddFlagTo(modeFlags, S_IRUSR | S_IWUSR);
		needModeFlags = true;
		break;

	case EFileMode::Open:
		// TODO What flags should we use for this?
		// https://man7.org/linux/man-pages/man2/open.2.html
		// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/open.2.html
		break;

	case EFileMode::OpenOrCreate:
		AddFlagTo(openFlags, O_CREAT);
		AddFlagTo(modeFlags, S_IRUSR | S_IWUSR);
		needModeFlags = true;
		break;

	case EFileMode::Truncate:
		AddFlagTo(openFlags, O_TRUNC);
		break;

	case EFileMode::Append:
		AddFlagTo(openFlags, O_APPEND);
		break;

	default:
		UM_ASSERT_NOT_REACHED_MSG("Unknown file mode");
		return nullptr;
	}

	FString path { pathAsView };
	int32 fileDescriptor = INDEX_NONE;
	if (needModeFlags)
	{
		fileDescriptor = ::open(path.GetChars(), openFlags, modeFlags);
	}
	else
	{
		fileDescriptor = ::open(path.GetChars(), openFlags);
	}

	if (fileDescriptor < 0)
	{
		UM_LOG(Error, "Failed to open file \"{}\"; reason: {}", path, FAppleFileSystem::GetLastError());
		return nullptr;
	}

	return MakeShared<FAppleFileStream>(fileDescriptor, MoveTemp(path), accessMode, openMode);
}

void FAppleFileStream::Read(void* data, uint64 dataSize)
{
	UM_ENSURE(IsOpen());

	const isize numBytesRead = ::read(m_Descriptor, data, dataSize);
	if (numBytesRead < 0)
	{
		UM_LOG(Error, "Failed to read {} bytes from POSIX file descriptor into {}", dataSize, data);
		UM_LOG(Error, "Last error: {}", FAppleFileSystem::GetLastError());
	}
}

void FAppleFileStream::Seek(const ESeekOrigin origin, const int64 offset)
{
	UM_ENSURE(IsOpen());

	int32 seekMode = INDEX_NONE;
	switch (origin)
	{
	case ESeekOrigin::Beginning:
		seekMode = SEEK_SET;
		break;

	case ESeekOrigin::Current:
		seekMode = SEEK_CUR;
		break;

	case ESeekOrigin::End:
		seekMode = SEEK_END;
		break;

	default:
		UM_ASSERT_NOT_REACHED_MSG("Unhandled seek mode");
		break;
	}

	::lseek(m_Descriptor, static_cast<off_t>(offset), seekMode);
}

int64 FAppleFileStream::Tell() const
{
	UM_ENSURE(IsOpen());

	// TODO This sucks
	return static_cast<int64>(lseek(m_Descriptor, 0, SEEK_CUR));
}

void FAppleFileStream::Write(const void* data, uint64 dataSize)
{
	UM_ENSURE(IsOpen());

	::write(m_Descriptor, data, dataSize);
}
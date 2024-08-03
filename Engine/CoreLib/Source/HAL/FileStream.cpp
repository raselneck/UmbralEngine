#include "HAL/FileStream.h"

IFileStream::IFileStream(const FString& path, const EFileAccess accessMode, const EFileMode openMode)
	: m_Path { path }
	, m_AccessMode { accessMode }
	, m_OpenMode { openMode }
{
}

IFileStream::IFileStream(FString&& path, const EFileAccess accessMode, const EFileMode openMode)
	: m_Path { MoveTemp(path) }
	, m_AccessMode { accessMode }
	, m_OpenMode { openMode }
{
}

bool IFileStream::CanRead() const
{
	return IsOpen() && m_AccessMode == EFileAccess::Read;
}

bool IFileStream::CanWrite() const
{
	return IsOpen() && m_AccessMode == EFileAccess::Write;
}

FStringView IFileStream::GetPath() const
{
	if (IsOpen())
	{
		return m_Path;
	}

	return nullptr;
}

void IFileStream::Write(TSpan<const uint8> bytes)
{
	if (bytes.IsEmpty())
	{
		return;
	}

	const uint64 sizeToWrite = static_cast<uint64>(bytes.Num());
	Write(bytes.GetData(), sizeToWrite);
}

void IFileStream::Write(const FStringView string)
{
	if (string.IsEmpty())
	{
		return;
	}

	const uint64 sizeToWrite = static_cast<uint64>(string.Length());
	Write(string.GetChars(), sizeToWrite);
}
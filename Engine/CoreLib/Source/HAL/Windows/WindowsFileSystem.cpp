#include "Engine/Logging.h"
#include "HAL/Path.h"
#include "HAL/Windows/WindowsFileSystem.h"
#include "Misc/StringBuilder.h"
#include <climits>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef DeleteFile
#undef GetLastError
#undef GetMessage

static constexpr FStringView LongWindowsPathPrefix = "\\\\?\\"_sv;

static FDateTime FileTimeToDateTime(const FILETIME& time)
{
	// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
	LARGE_INTEGER timeAsTicks {};
	timeAsTicks.LowPart = time.dwLowDateTime;
	timeAsTicks.HighPart = static_cast<LONG>(time.dwHighDateTime);

	const FDateTime windowsEpoch { 1601, 1, 1 }; // January 1, 1601
	const int64 ticksSinceEpoch = timeAsTicks.QuadPart;

	return windowsEpoch + FTimeSpan { ticksSinceEpoch };
}

static FDateTime FileTimeToLocalDateTime(const FILETIME& time)
{
	return FileTimeToDateTime(time).ToLocalTime();
}

static FString MakeGenericQueryString(const FString& path)
{
	if (path.IsEmpty())
	{
		return {};
	}

	const bool endsWithSeparator = FPath::IsDirectorySeparator(path.Last().GetValue());

	FStringBuilder result;
	result.Reserve(path.Length() + LongWindowsPathPrefix.Length() + (endsWithSeparator ? 2 : 1));

	result.Append(LongWindowsPathPrefix);
	for (char ch : path)
	{
		if (ch == '/')
		{
			result.Append('\\');
		}
		else
		{
			result.Append(ch);
		}
	}

	if (endsWithSeparator)
	{
		result.Append('*');
	}
	else
	{
		result.Append("\\*"_sv);
	}

	return result.ReleaseString();
}

static FString MakeSpecificQueryString(const FString& path, const FString& pattern)
{
	FString result = FPath::Join(path, pattern);
	result.Prepend(LongWindowsPathPrefix);

	for (char& ch : result)
	{
		if (ch == '/')
		{
			ch = '\\';
		}
	}

	return result;
}

TErrorOr<void> FWindowsFileSystem::DeleteFile(const FString& filePath)
{
	if (::DeleteFileA(filePath.GetChars()) != 0)
	{
		return {};
	}

	return GetLastErrorAsError();
}

FString FWindowsFileSystem::GetAbsolutePath(const FString& path)
{
	char result[1024];
	FMemory::ZeroOutArray(result);

	char* garbage = nullptr;
	const DWORD resultLength = ::GetFullPathNameA(path.GetChars(), UM_ARRAY_SIZE(result), result, &garbage);
	if (resultLength == 0)
	{
		UM_LOG(Error, "Failed to get absolute path for `{}`; reason: {}", path, FWindowsFileSystem::GetLastError());
		return path;
	}

	FString absolutePath { result, static_cast<FString::SizeType>(resultLength) };
	return FPath::Normalize(absolutePath);
}

FString FWindowsFileSystem::GetExecutablePath()
{
	// Unfortunately, GetModuleFileNameA does not work like GetCurrentDirectoryA as far as
	// getting the length of the directory goes. We need to use a temporary buffer with this
	char buffer[2048] = {};
	DWORD pathLength = ::GetModuleFileNameA(nullptr, buffer, UM_ARRAY_SIZE(buffer));
	if (pathLength == 0 || ::GetLastError() != ERROR_SUCCESS)
	{
		const TErrorOr<void> lastError = GetLastErrorAsError();
		UM_LOG(Error, "Failed to retrieve executable directory. Reason: {}", lastError.GetError().GetMessage());
		return {};
	}

	FString result { buffer, static_cast<FStringView::SizeType>(pathLength) };
	return FPath::Normalize(result);
}

TErrorOr<void> FWindowsFileSystem::GetFiles(const FString& path, const bool recursive, TArray<FString>& files)
{
	(void)recursive;

	if (FPath::IsDirectory(path) == false)
	{
		return MAKE_ERROR("Given path is not a directory");
	}

	const FString query = MakeGenericQueryString(path);

	WIN32_FIND_DATA findData {};
	HANDLE result = FindFirstFileA(query.GetChars(), &findData);
	if (result == INVALID_HANDLE_VALUE)
	{
		return MAKE_ERROR("Failed to begin finding files in `{}`; reason: {}", path, FWindowsFileSystem::GetLastError());
	}

	do
	{
		const FStringView entryName { findData.cFileName, FStringView::CharTraitsType::GetNullTerminatedLength(findData.cFileName) };
		if (entryName == "."_sv || entryName == ".."_sv)
		{
			continue;
		}

		FString entryPath = FPath::Join(path, entryName);
		if (FPath::IsDirectory(entryPath))
		{
			if (recursive == false)
			{
				continue;
			}

			TRY_DO(FWindowsFileSystem::GetFiles(entryPath, true, files));
		}
		else
		{
			files.Add(MoveTemp(entryPath));
		}
	} while (FindNextFileA(result, &findData));

	FindClose(result);

	return {};
}

FStringView FWindowsFileSystem::GetLastError()
{
	// This is the same size buffer that .NET uses, so... good enough... probably
	static char errorBuffer[4096] = {};

	DWORD errorId = ::GetLastError();
	if (errorId == ERROR_SUCCESS)
	{
		return nullptr;
	}

	DWORD errorLength = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorId,
	                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&errorBuffer),
	                                   UM_ARRAY_SIZE(errorBuffer), nullptr);

	return FStringView { errorBuffer, static_cast<FStringView::SizeType>(errorLength) };
}

TErrorOr<void> FWindowsFileSystem::GetLastErrorAsError()
{
	const FStringView lastErrorMessage = FWindowsFileSystem::GetLastError();
	if (lastErrorMessage.IsEmpty())
	{
		return {};
	}

	return MAKE_ERROR("{}", lastErrorMessage);
}

TErrorOr<void> FWindowsFileSystem::FindFiles(const FString& path, const FString& pattern, bool recursive, TArray<FString>& files)
{
	(void)recursive;
	(void)files;

	const FString query = MakeSpecificQueryString(path, pattern);
	(void)query;

	return MAKE_ERROR("FWindowsFileSystem::FindFiles not yet implemented");
}

FString FWindowsFileSystem::GetWorkingDir()
{
	const DWORD directoryLength = ::GetCurrentDirectoryA(0, nullptr);
	if (directoryLength == 0)
	{
		return {};
	}

	FStringBuilder directory;
	directory.Append('\0', static_cast<FStringBuilder::SizeType>(directoryLength));

	if (::GetCurrentDirectoryA(directoryLength, directory.GetChars()) == 0)
	{
		const TErrorOr<void> lastError = GetLastErrorAsError();
		UM_LOG(Error, "Failed to retrieve working directory. Reason: {}", lastError.GetError().GetMessage());
		return {};
	}

	FString result = directory.ReleaseString();
	return FPath::Normalize(result);
}

void FWindowsFileSystem::StatFile(const FString& fileName, FFileStats& stats)
{
	WIN32_FILE_ATTRIBUTE_DATA fileAttributes {};
	if (GetFileAttributesExA(fileName.GetChars(), GetFileExInfoStandard, &fileAttributes) == FALSE)
	{
		stats.Exists = false;
		return;
	}

	// NOTE The POSIX file system returns these as local times, so we need to make sure these times are local
	stats.ModifiedTime = FileTimeToLocalDateTime(fileAttributes.ftLastWriteTime);
	stats.CreationTime = FileTimeToLocalDateTime(fileAttributes.ftCreationTime);
	stats.LastAccessTime = FileTimeToLocalDateTime(fileAttributes.ftLastAccessTime);
	stats.IsDirectory = HasFlag(fileAttributes.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
	stats.IsReadOnly = HasFlag(fileAttributes.dwFileAttributes, FILE_ATTRIBUTE_READONLY);
	stats.Exists = true;

	stats.Size = 0;
	if (stats.IsDirectory == false)
	{
		LARGE_INTEGER size;
		size.HighPart = static_cast<LONG>(fileAttributes.nFileSizeHigh);
		size.LowPart = fileAttributes.nFileSizeLow;
		stats.Size = size.QuadPart;
	}
}
#include "Engine/Logging.h"
#include "HAL/Path.h"
#include "HAL/Linux/LinuxFileSystem.h"
#include "Misc/AtExit.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

[[maybe_unused]] static FStringView GetErrnoName()
{
#define RETURN_ERRNO_CASE(Value) case Value: return UM_JOIN(#Value, _sv)

	switch (errno)
	{
		RETURN_ERRNO_CASE(EACCES);
		RETURN_ERRNO_CASE(EBADF);
		RETURN_ERRNO_CASE(EBUSY);
		RETURN_ERRNO_CASE(EDQUOT);
		RETURN_ERRNO_CASE(EEXIST);
		RETURN_ERRNO_CASE(EFAULT);
		RETURN_ERRNO_CASE(EFBIG);
		RETURN_ERRNO_CASE(EINTR);
		RETURN_ERRNO_CASE(EINVAL);
		RETURN_ERRNO_CASE(EISDIR);
		RETURN_ERRNO_CASE(ELOOP);
		RETURN_ERRNO_CASE(EMFILE);
		RETURN_ERRNO_CASE(ENAMETOOLONG);
		RETURN_ERRNO_CASE(ENFILE);
		RETURN_ERRNO_CASE(ENODEV);
		RETURN_ERRNO_CASE(ENOENT);
		RETURN_ERRNO_CASE(ENOMEM);
		RETURN_ERRNO_CASE(ENOSPC);
		RETURN_ERRNO_CASE(ENOTDIR);
		RETURN_ERRNO_CASE(ENXIO);
		RETURN_ERRNO_CASE(EOPNOTSUPP);
		RETURN_ERRNO_CASE(EOVERFLOW);
		RETURN_ERRNO_CASE(EPERM);
		RETURN_ERRNO_CASE(EROFS);
		RETURN_ERRNO_CASE(ETXTBSY);
		RETURN_ERRNO_CASE(EWOULDBLOCK);
	default: return nullptr;
	}

#undef RETURN_ERRNO_CASE
}

static TErrorOr<void> GetOrFindFilesInDirectory(const FString& path, const FString& pattern, const bool recursive, TArray<FString>& results)
{
	DIR* directoryHandle = ::opendir(path.GetChars());
	ON_EXIT_SCOPE()
	{
		::closedir(directoryHandle);
	};

	if (directoryHandle == nullptr)
	{
		return MAKE_ERROR("Failed to open directory `{}`; reason: {}", path, FLinuxFileSystem::GetLastError());
	}

	struct dirent* entry = nullptr;
	while ((entry = ::readdir(directoryHandle)) != nullptr)
	{
		const FStringView entryName { entry->d_name };
		if (entryName == "."_sv || entryName == ".."_sv)
		{
			continue;
		}

		FString entryPath = FPath::Join(path, entryName);
		if (HasFlag(entry->d_type, DT_DIR))
		{
			if (recursive == false)
			{
				continue;
			}

			TRY_DO(GetOrFindFilesInDirectory(entryPath, pattern, true, results));
		}
		else
		{
			(void)results.Emplace(MoveTemp(entryPath));
		}
	}

	return {};
}

static FDateTime LinuxTimeToDateTime(const struct timespec& time)
{
	// TODO Include time.tv_nsec

	const FTimeSpan timeSinceEpoch = FTimeSpan::FromSeconds(static_cast<double>(time.tv_sec));
	return FDateTime::Epoch + timeSinceEpoch;
}

TErrorOr<void> FLinuxFileSystem::DeleteFile(const FString& filePath)
{
	FFileStats fileStats;
	StatFile(filePath, fileStats);

	if (fileStats.Exists == false)
	{
		return MAKE_ERROR("Cannot delete `{}` as it does not exist", filePath);
	}

	if (fileStats.IsDirectory)
	{
		return MAKE_ERROR("Cannot delete `{}` as it is a directory", filePath);
	}

	if (::remove(filePath.GetChars()) == 0)
	{
		return {};
	}

	return GetLastErrorAsError();
}

FString FLinuxFileSystem::GetAbsolutePath(const FString& path)
{
	char resolvedPath[PATH_MAX];
	static_assert(UM_ARRAY_SIZE(resolvedPath) >= 1024, "Need to reserve more characters for resolved path");

	(void)::realpath(path.GetChars(), resolvedPath);

	FString result;
	result.Append(resolvedPath, TCharTraits<char>::GetNullTerminatedLength(resolvedPath));

	return result;
}

FString FLinuxFileSystem::GetExecutablePath()
{
	// https://stackoverflow.com/a/933996
}

TErrorOr<void> FLinuxFileSystem::GetFiles(const FString& path, const bool recursive, TArray<FString>& files)
{
	if (FPath::IsDirectory(path) == false)
	{
		return MAKE_ERROR("Given path is not a directory");
	}

	const FString pattern;
	return GetOrFindFilesInDirectory(path, pattern, recursive, files);
}

FStringView FLinuxFileSystem::GetLastError()
{
	return FStringView { ::strerror(errno) };
}

TErrorOr<void> FLinuxFileSystem::GetLastErrorAsError()
{
	if (errno == 0)
	{
		return {};
	}

	return MAKE_ERROR("{}", GetLastError());
}

FString FLinuxFileSystem::GetWorkingDir()
{
	char workingDir[PATH_MAX];
	static_assert(UM_ARRAY_SIZE(workingDir) >= 1024, "Need to reserve more characters for working directory");

	const char* resultPtr = ::getcwd(workingDir, UM_ARRAY_SIZE(workingDir));
	UM_ASSERT(resultPtr == workingDir, "Error calling `getcwd`");

	FString result;
	result.Append(workingDir, TCharTraits<char>::GetNullTerminatedLength(workingDir));
	return result;
}

TErrorOr<void> FLinuxFileSystem::FindFiles(const FString& path, const FString& pattern, const bool recursive, TArray<FString>& files)
{
	// TODO Need to attempt to match the file pattern

	(void)pattern;

	return GetFiles(path, recursive, files);
}

void FLinuxFileSystem::StatFile(const FString& fileName, FFileStats& stats)
{
	const FString filePath = GetAbsolutePath(fileName);

	struct stat fileStats {};
	if (::stat(filePath.GetChars(), &fileStats) != 0)
	{
		stats.Exists = false;
		return;
	}

	stats.Size = static_cast<int64>(fileStats.st_size);
	stats.ModifiedTime = LinuxTimeToDateTime(fileStats.st_mtim);
	stats.LastAccessTime = LinuxTimeToDateTime(fileStats.st_mtim);
	stats.IsDirectory = HasFlag(fileStats.st_mode, S_IFDIR);
	stats.IsReadOnly = (HasFlag(fileStats.st_mode, S_IWUSR) == false);
	stats.Exists = true;
}
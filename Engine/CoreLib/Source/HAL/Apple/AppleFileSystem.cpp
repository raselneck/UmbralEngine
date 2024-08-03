#include "Engine/Logging.h"
#include "HAL/Apple/AppleFileSystem.h"
#include "HAL/InternalTime.h"
#include "HAL/Path.h"
#include "Misc/AtExit.h"
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <glob.h>
#include <mach-o/dyld.h>
#include <sys/stat.h>
#include <unistd.h>

[[nodiscard]] static FStringView GetErrnoNameFromValue(const errno_t error)
{
#define RETURN_ERRNO_CASE(Value) case Value: return UM_JOIN(#Value, _sv)

	switch (error)
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

static int32 HandleGlobError(const char* filePath, const errno_t error)
{
	UM_LOG(Error, "Failed to glob file \"{}\" with error {}", filePath, GetErrnoNameFromValue(error));

	// If errfunc returns nonzero, or if GLOB_ERR is set, glob() will terminate after the call to errfunc.
	// https://linux.die.net/man/3/glob
	return 0;
}

static TErrorOr<void> GetOrFindFilesInDirectory(const char* path, const FString& pattern, const bool recursive, TArray<FString>& results)
{
	(void)recursive;
	(void)results;

	const FString globPattern = [&]
	{
		if (pattern.IsEmpty())
		{
			return FPath::Join(FStringView { path }, "*"_sv);
		}
		return FPath::Join(FStringView { path }, pattern);
	}();

	glob_t globber {};
	ON_EXIT_SCOPE()
	{
		::globfree(&globber);
	};

	constexpr int32 globFlags = GLOB_NOSORT; // We sort by ourselves
	const int32 globResult = ::glob(globPattern.GetChars(), globFlags, HandleGlobError, &globber);

	switch (globResult)
	{
	case GLOB_NOSPACE:
		return MAKE_ERROR("Ran out of memory while globbing pattern \"{}\"", globPattern);

	case GLOB_ABORTED:
		return MAKE_ERROR("Encountered read error while globbing pattern \"{}\"", globPattern);

	case GLOB_NOMATCH:
		return {};

	default:
		break;
	}

	struct stat fileStats {};
	for (size_t idx = 0; idx < globber.gl_pathc; ++idx)
	{
		const FStringView filePath { globber.gl_pathv[idx] };
		UM_ENSURE(::stat(filePath.GetChars(), &fileStats) == 0);

		if (HasFlag(fileStats.st_mode, S_IFDIR))
		{
			if (recursive == false)
			{
				continue;
			}

			TRY_DO(GetOrFindFilesInDirectory(filePath.GetChars(), pattern, recursive, results));
		}
		else
		{
			(void)results.Emplace(filePath);
		}
	}

	return {};
}

static FDateTime PosixTimeToDateTime(const struct timespec& time)
{
	const FTimeSpan timeSinceEpoch = FTimeSpan::FromSeconds(static_cast<double>(time.tv_sec)) + Time::NanosecondsToTimeSpan(time.tv_nsec);
	return FDateTime::Epoch + timeSinceEpoch;
}

TErrorOr<void> FAppleFileSystem::DeleteFile(const FString& filePath)
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

FString FAppleFileSystem::GetAbsolutePath(const FString& path)
{
	char resolvedPath[1024];
	static_assert(UM_ARRAY_SIZE(resolvedPath) >= PATH_MAX, "Need to reserve more characters for resolved path");

	(void)::realpath(path.GetChars(), resolvedPath);

	FString result;
	result.Append(resolvedPath, TCharTraits<char>::GetNullTerminatedLength(resolvedPath));

	return result;
}

FString FAppleFileSystem::GetExecutablePath()
{
	char buffer[2048] = {};
	uint32 bufferSize = static_cast<uint32>(UM_ARRAY_SIZE(buffer));

	static_assert(UM_ARRAY_SIZE(buffer) >= PATH_MAX);

	if (_NSGetExecutablePath(buffer, &bufferSize) == 0)
	{
		return FString { buffer, FStringView::StringTraitsType::GetNullTerminatedCharCount(buffer) };
	}

	UM_LOG(Error, "Failed to get executable directory. Buffer size = {}, required size = {}", UM_ARRAY_SIZE(buffer), bufferSize);
	return {};
}

TErrorOr<void> FAppleFileSystem::GetFiles(const FString& path, const bool recursive, TArray<FString>& files)
{
	if (FPath::IsDirectory(path) == false)
	{
		return MAKE_ERROR("Given path is not a directory");
	}

	const FString pattern;
	return GetOrFindFilesInDirectory(path.GetChars(), pattern, recursive, files);
}

FStringView FAppleFileSystem::GetLastError()
{
	const char* errorMessage = ::strerror(errno);
	const int32 errorLength = TStringTraits<char>::GetNullTerminatedCharCount(errorMessage);
	return FStringView { errorMessage, errorLength };
}

TErrorOr<void> FAppleFileSystem::GetLastErrorAsError()
{
	if (errno == 0)
	{
		return {};
	}

	return MAKE_ERROR("{}", GetLastError());
}

FString FAppleFileSystem::GetWorkingDir()
{
	char workingDir[1024];
	static_assert(UM_ARRAY_SIZE(workingDir) >= PATH_MAX, "Need to reserve more characters for working directory");

	const char* resultPtr = ::getcwd(workingDir, UM_ARRAY_SIZE(workingDir));
	UM_ASSERT(resultPtr == workingDir, "Error calling `getcwd`");

	FString result;
	result.Append(workingDir, TCharTraits<char>::GetNullTerminatedLength(workingDir));
	return result;
}

TErrorOr<void> FAppleFileSystem::FindFiles(const FString& path, const FString& pattern, const bool recursive, TArray<FString>& files)
{
	// TODO Need to attempt to match the file pattern

	(void)pattern;

	return GetFiles(path, recursive, files);
}

void FAppleFileSystem::StatFile(const FString& fileName, FFileStats& stats)
{
	const FString filePath = GetAbsolutePath(fileName);

	struct stat fileStats {};
	if (::stat(filePath.GetChars(), &fileStats) != 0)
	{
		stats.Exists = false;
		return;
	}

	stats.Size = static_cast<int64>(fileStats.st_size);
	stats.ModifiedTime = PosixTimeToDateTime(fileStats.st_mtimespec);
	stats.CreationTime = PosixTimeToDateTime(fileStats.st_birthtimespec);
	stats.LastAccessTime = PosixTimeToDateTime(fileStats.st_atimespec);
	stats.IsDirectory = HasFlag(fileStats.st_mode, S_IFDIR);
	stats.IsReadOnly = (HasFlag(fileStats.st_mode, S_IWUSR) == false);
	stats.Exists = true;
}
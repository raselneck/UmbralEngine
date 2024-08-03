#include "Engine/CommandLine.h"
#include "HAL/Directory.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include "Engine/Logging.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsFileSystem.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleFileSystem.h"
#else
#	include "HAL/Linux/LinuxFileSystem.h"
#endif

bool FDirectory::Exists(const FStringView path)
{
	FFileStats stats;
	FFile::Stat(path, stats);

	return stats.Exists && stats.IsDirectory;
}

bool FDirectory::Exists(const FString& path)
{
	FFileStats stats;
	FFile::Stat(path, stats);

	return stats.Exists && stats.IsDirectory;
}

FString FDirectory::GetContentDir()
{
	const FString workingDir = GetWorkingDir();
	FString localContentDir = FPath::Join(workingDir, "Content"_sv);

#ifdef UMBRAL_CONTENT_DIR
	// Prefer the local content directory if it exists
	if (FDirectory::Exists(localContentDir))
	{
		return localContentDir;
	}

	constexpr FStringView contentDirectory = UM_JOIN(UMBRAL_CONTENT_DIR, _sv);
	return FPath::GetAbsolutePath(contentDirectory);
#else
	return localContentDir;
#endif
}

FString FDirectory::GetContentFilePath(FStringView fileName)
{
	FString fileNameAsString { fileName };
	return GetContentFilePath(MoveTemp(fileNameAsString));
}

FString FDirectory::GetContentFilePath(const FString& fileName)
{
	FString copyOfFileName = fileName;
	return GetContentFilePath(MoveTemp(copyOfFileName));
}

FString FDirectory::GetContentFilePath(FString&& fileName)
{
	if (fileName.IsEmpty())
	{
		return fileName;
	}

	if (fileName[0] != '/')
	{
		fileName.Prepend("/"_sv);
	}

	const FString contentDir = GetContentDir();
	fileName.Prepend(contentDir);

	return fileName;
}

FString FDirectory::GetExecutableDir()
{
	const FString executablePath = FNativeDirectory::GetExecutablePath();
	return FPath::GetDirectoryName(executablePath);
}

TErrorOr<void> FDirectory::GetFiles(const FStringView pathAsView, const bool recursive, TArray<FString>& files)
{
	const FString path { pathAsView };
	return GetFiles(path, recursive, files);
}

TErrorOr<void> FDirectory::GetFiles(const FString& path, const bool recursive, TArray<FString>& files)
{
	TRY_DO(FNativeDirectory::GetFiles(path, recursive, files));

	files.Sort();

	return {};
}

FString FDirectory::GetWorkingDir()
{
	return FNativeDirectory::GetWorkingDir();
}
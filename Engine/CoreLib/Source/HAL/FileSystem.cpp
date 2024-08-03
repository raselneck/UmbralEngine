#include "Containers/String.h"
#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "HAL/Directory.h"
#include "HAL/File.h"
#include "HAL/FileSystem.h"
#include "HAL/Path.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsFileStream.h"
#	include "HAL/Windows/WindowsFileSystem.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleFileStream.h"
#	include "HAL/Apple/AppleFileSystem.h"
#else
#	include "HAL/Linux/LinuxFileStream.h"
#	include "HAL/Linux/LinuxFileSystem.h"
#endif

/**
 * @brief Defines a file system mount point.
 */
class FFileSystemMountPoint
{
public:

	/**
	 * @brief The mount point (or path alias).
	 */
	FString MountPoint;

	/**
	 * @brief The absolute paths that the mount point can reference.
	 */
	TArray<FString> Directories;

	/**
	 * @brief Checks to see if the given directory is already registered with this mount point.
	 *
	 * @param directory The directory to check for.
	 * @return True if \p directory is already registered, otherwise false.
	 */
	[[nodiscard]] bool IsDirectoryRegistered(const FStringView directory) const
	{
		const int32 directoryIndex = Directories.IndexOfByPredicate(
			[directory](const FString& dir)
			{
				return dir.Equals(directory, EIgnoreCase::Yes);
			}
		);

		return directoryIndex != INDEX_NONE;
	}
};

// TODO Keep track of opened file paths so we don't accidentally open the same file multiple times?
static TArray<FFileSystemMountPoint> GMountPoints;

/**
 * @brief Finds or registers a new mount point with the given path alias.
 *
 * @param mountPoint The mount point, or path alias.
 * @return The registered mount point.
 */
[[nodiscard]] static FFileSystemMountPoint* FindOrRegisterMountPoint(const FStringView mountPoint)
{
	FFileSystemMountPoint* registeredMountPoint = GMountPoints.FindByPredicate(
		[mountPoint](const FFileSystemMountPoint& mp)
		{
			return mp.MountPoint.Compare(mountPoint, EIgnoreCase::Yes) == ECompareResult::Equals;
		}
	);

	if (registeredMountPoint == nullptr)
	{
		registeredMountPoint = &GMountPoints.AddDefaultGetRef();
		registeredMountPoint->MountPoint = FString { mountPoint };
	}

	return registeredMountPoint;
}

// TODO Need to expose ResolveFilePathWithMountPoints so that all I/O functions can use it

/**
 * @brief Attempts to resolve a file path with all registered mount points.
 *
 * @param path The file path to resolve.
 * @return The resolved file path, or the given path if it could not be resolved with any mount point.
 */
[[nodiscard]] static FString ResolveFilePathWithMountPoints(const FStringView path)
{
	if (FPath::IsAbsolute(path))
	{
		return FString { path };
	}

	for (const FFileSystemMountPoint& mountPoint : GMountPoints)
	{
		// Ignore this mount point if the path does not start with its alias
		if (mountPoint.MountPoint.IsEmpty() == false &&
		    path.StartsWith(mountPoint.MountPoint, EIgnoreCase::Yes) == false)
		{
			continue;
		}

		const FStringView relativePath = path.Right(path.Length() - mountPoint.MountPoint.Length());
		for (const FString& directory : mountPoint.Directories)
		{
			FString absolutePath = FPath::Join(directory, relativePath);
			UM_LOG(Verbose, "Checking to see if `{}` exists...", absolutePath);

			if (FFile::Exists(absolutePath))
			{
				return absolutePath;
			}
		}
	}

	return FPath::GetAbsolutePath(path);
}

bool FFileSystem::CanAccessFilesAnywhere()
{
	// TODO Implement custom file system quarantining that we lost when PhysicsFS was removed
	return true;
}

FStringView FFileSystem::GetLastError()
{
	return FNativeFileSystem::GetLastError();
}

bool FFileSystem::Mount(const FStringView directory)
{
	const FStringView rootMountPoint;
	return Mount(directory, rootMountPoint);
}

bool FFileSystem::Mount(const FStringView directoryAsView, const FStringView mountPoint)
{
	if (directoryAsView.IsEmpty())
	{
		UM_LOG(Error, "Cannot mount empty directory paths");
		return false;
	}

	FString directory = FPath::GetAbsolutePath(directoryAsView);

	if (FDirectory::Exists(directory) == false)
	{
		UM_LOG(Error, "Cannot mount non-existent directory `{}`", directory);
		return false;
	}

	FFileSystemMountPoint* registeredMountPoint = FindOrRegisterMountPoint(mountPoint);
	if (registeredMountPoint->IsDirectoryRegistered(directory))
	{
		UM_LOG(Error, "Cannot register directory `{}` with mount point `{}` more than once", directory, mountPoint);
		return false;
	}

	registeredMountPoint->Directories.Add(MoveTemp(directory));

	return true;
}

TSharedPtr<IFileStream> FFileSystem::Open(const FStringView path, const EFileMode mode, const EFileAccess access)
{
	const FString absolutePath = ResolveFilePathWithMountPoints(path);
	/*if (FFile::Exists(absolutePath) == false)
	{
		UM_LOG(Error, "Failed to open file \"{}\"; does it exist?", absolutePath);
		return nullptr;
	}*/

	if (CanAccessFilesAnywhere() == false)
	{
		UM_LOG(Error, "Failed to open file \"{}\" in restricted mode", path);
		return nullptr;
	}

	return FNativeFileStream::Open(absolutePath, mode, access);
}

void FFileSystem::SetCanAccessFilesAnywhere(const bool canAccessFilesAnywhere)
{
	(void)canAccessFilesAnywhere;
}
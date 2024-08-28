#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include "Math/Math.h"
#include "Misc/StringBuilder.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsFileSystem.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleFileSystem.h"
#else
#	include "HAL/Linux/LinuxFileSystem.h"
#endif

/**
 * @brief Gets the last index of a slash in a path.
 *
 * @param path The path.
 * @return The last index of either a forward slash or a back slash.
 */
static FStringView::SizeType GetLastSlashIndex(const FStringView path)
{
	const int32 lastBackSlashIndex = path.LastIndexOf('\\');
	const int32 lastForwardSlashIndex = path.LastIndexOf('/');
	return FMath::Max(lastBackSlashIndex, lastForwardSlashIndex);
}

/**
 * @brief Returns true if the path uses the canonical form of extended syntax ("\\?\" or "\??\"). If the
 *        path matches exactly (cannot use alternate directory separators) Windows will skip normalization
 *        and path length checks.
 *
 * @param path The path.
 * @return True if \p path is using extended syntax, otherwise false.
 */
[[maybe_unused]] static constexpr bool IsPathExtended(const FStringView path)
{
	constexpr int32 devicePrefixLength = 4;
	return path.Length() >= devicePrefixLength &&
	       path[0] == '\\' &&
	       (path[1] == '\\' || path[1] == '?') &&
	       path[2] == '?' &&
	       path[3] == '\\';
}

/**
 * @brief Checks to see if a character is a valid drive character.
 *
 * @param ch The character.
 * @return True if \p ch is a valid drive character, otherwise false.
 */
[[maybe_unused]] static constexpr bool IsValidDriveCharacter(const char ch)
{
	return FMath::IsWithinRangeInclusive(ch, 'A', 'Z') || FMath::IsWithinRangeInclusive(ch, 'a', 'z');
}

/**
 * @brief Checks to see if a character is a volume separator.
 *
 * @param ch The character.
 * @return True if \p ch is a volume separator, otherwise false.
 */
static constexpr bool IsVolumeSeparatorCharacter(const char ch)
{
#if UMBRAL_PLATFORM_IS_WINDOWS || UMBRAL_PLATFORM_IS_MAC
	return ch == ':';
#else
	return ch == '/';
#endif
}

/**
 * @brief Normalizes all of the separators in the given path to be "/".
 *
 * @param string The string to normalize.
 */
template<typename StringLikeType>
static void NormalizeSeparatorsInline(StringLikeType& path)
{
	for (int32 idx = 0; idx < path.Length(); ++idx)
	{
		if (path.GetChars()[idx] == '\\')
		{
			path.GetChars()[idx] = '/';
		}
	}
}

/**
 * @brief Normalizes all of the separators in the given path to be "/".
 *
 * @param string The string to normalize.
 * @return The normalized path.
 */
static FString NormalizeSeparators(const FStringView path)
{
	FString result { path };
	NormalizeSeparatorsInline(result);
	return result;
}

FString FPath::GetAbsolutePath(const FStringView pathAsView)
{
	const FString path { pathAsView };
	return GetAbsolutePath(path);
}

FString FPath::GetAbsolutePath(const FString& path)
{
	return FNativePath::GetAbsolutePath(path);
}

FString FPath::GetBaseFileName(const FStringView path)
{
	return FString { GetBaseFileNameAsView(path) };
}

FStringView FPath::GetBaseFileNameAsView(const FStringView path)
{
	const FStringView fileName = FPath::GetFileNameAsView(path);
	const FStringView extension = GetExtension(path);

	if (extension.IsEmpty())
	{
		return fileName;
	}

	const int32 extensionLengthToRemove = extension.IsEmpty() ? 0 : extension.Length() + 1;
	return FStringView { fileName.GetChars(), fileName.Length() - extensionLengthToRemove };
}

FString FPath::GetDirectoryName(const FStringView path)
{
	return FString { GetDirectoryNameAsView(path) };
}

FStringView FPath::GetDirectoryNameAsView(const FStringView path)
{
	// TODO: Copy implementation from https://referencesource.microsoft.com/#mscorlib/system/io/path.cs,ecfb67b37299beba
	//       Specifically "InternalGetDirectoryName"

	if (path.IsEmpty() || HasIllegalCharacter(path))
	{
		return {};
	}

	const int32 lastSlashIndex = path.LastIndexOfByPredicate(IsDirectorySeparator);
	return lastSlashIndex == INDEX_NONE ? FStringView { } : path.Left(lastSlashIndex);
}

FStringView FPath::GetExtension(const FStringView path)
{
	const int32 lastDotIndex = path.LastIndexOf('.');
	if (lastDotIndex == INDEX_NONE)
	{
		return nullptr;
	}

	// If given something like `/foo/bar.dir/thing`, there isn't an extension to return
	const int32 lastSlashIndex = GetLastSlashIndex(path);
	if (lastSlashIndex > lastDotIndex)
	{
		return nullptr;
	}

	return path.Right(path.Length() - lastDotIndex - 1);
}

FString FPath::GetExecutableName()
{
	const FString executablePath = FNativeFileSystem::GetExecutablePath();
	return GetBaseFileName(executablePath);
}

FString FPath::GetFileName(const FStringView path)
{
	return FString { GetFileNameAsView(path) };
}

FStringView FPath::GetFileNameAsView(const FStringView path)
{
	const int32 lastSlashIndex = GetLastSlashIndex(path);
	if (lastSlashIndex == INDEX_NONE)
	{
		return path;
	}

	return path.Right(path.Length() - lastSlashIndex - 1);
}

bool FPath::HasIllegalCharacter(const FStringView path)
{
	// TODO Use FStringView::ContainsByPredicate when that exists

	return path.AsSpan().ContainsByPredicate(IsIllegalPathCharacter);
}

bool FPath::IsAbsolute(const FStringView path)
{
	// NOTE: Implementation from https://referencesource.microsoft.com/#mscorlib/system/io/path.cs,09d4ae1717c5b2cc
	//       Specifically "IsPathRooted"

	if (path.IsEmpty() || HasIllegalCharacter(path))
	{
		return false;
	}

	const bool isUnixAbsolute = path.Length() >= 1 && IsDirectorySeparator(path[0]);
	const bool isWindowsAbsolute = path.Length() >= 2 && IsVolumeSeparatorCharacter(path[1]);

	return isUnixAbsolute || isWindowsAbsolute;
}

bool FPath::IsDirectory(const FStringView pathAsView)
{
	const FString path { pathAsView };
	return IsDirectory(path);
}

bool FPath::IsDirectory(const FString& path)
{
	FFileStats stats;
	FFile::Stat(path, stats);

	return stats.IsDirectory;
}

bool FPath::IsFile(const FStringView pathAsView)
{
	const FString path { pathAsView };
	return IsFile(path);
}

bool FPath::IsFile(const FString& path)
{
	FFileStats stats;
	FFile::Stat(path, stats);

	return stats.IsDirectory == false;
}

bool FPath::IsRelative(const FStringView path)
{
	// NOTE: Implementation from https://referencesource.microsoft.com/#mscorlib/system/io/pathinternal.cs
	//       Specifically "IsPartiallyQualified"

	// It isn't fixed, it must be relative.  There is no way to specify a fixed path with one character (or less).
	if (path.Length() < 2)
	{
		return true;
	}

	if (IsDirectorySeparator(path[0]))
	{
		// There is no valid way to specify a relative path with two initial slashes or
		// \? as ? isn't valid for drive relative paths and "\??\" is equivalent to "\\?\"
		return (path[1] == '?' || IsDirectorySeparator(path[1])) == false;
	}

	// The only way to specify a fixed path that doesn't begin with two slashes
	// is the drive, colon, slash format ("C:\")
	return IsAbsolute(path) == false;
}

template<typename StringType>
static FString JoinPathPartsImpl(const TSpan<const StringType>& pathParts)
{
	if (pathParts.IsEmpty())
	{
		return {};
	}

	FStringBuilder builder;
	builder.Append(pathParts[0]);
	for (int32 idx = 1; idx < pathParts.Num(); ++idx)
	{
		const char lastChar = builder.GetLastChar();
		if (FPath::IsDirectorySeparator(lastChar) == false)
		{
			builder.Append('/');
		}

		FStringView pathPartToAdd = static_cast<FStringView>(pathParts[idx]);
		while (pathPartToAdd.Length() > 0 && FPath::IsDirectorySeparator(pathPartToAdd[0]))
		{
			pathPartToAdd = pathPartToAdd.RemoveLeft(1);
		}

		if (pathPartToAdd.IsEmpty() == false)
		{
			builder.Append(pathPartToAdd);
		}
	}

	NormalizeSeparatorsInline(builder);

	return builder.ReleaseString();
}

FString FPath::Join(const TSpan<const FStringView> pathParts)
{
	return JoinPathPartsImpl(pathParts);
}

FString FPath::Join(const TSpan<const FString> pathParts)
{
	return JoinPathPartsImpl(pathParts);
}

FString FPath::Join(const TSpan<const FStringOrStringView> pathParts)
{
	return JoinPathPartsImpl(pathParts);
}

FString FPath::Normalize(FStringView path)
{
	FString result { path };
	NormalizeSeparatorsInline(result);
	return result;
}

FString FPath::Normalize(const FString& path)
{
	FString result { path };
	NormalizeSeparatorsInline(result);
	return result;
}

FString& FPath::Normalize(FString& path)
{
	NormalizeSeparatorsInline(path);
	return path;
}

FString FPath::ResolvePath(const FStringView pathToResolve)
{
#if 1
	UM_LOG(Error, "FPath::ResolvePath is not yet implemented");

	return NormalizeSeparators(pathToResolve);
#else
	// TODO This needs to be implemented :)

	const FStringView ParentDir = "/.."_sv;

	FString result { pathToResolve };
	while (true)
	{
		if (result.IsEmpty())
		{
			break;
		}

		// Consider empty paths or paths which start with .. or /.. as invalid
		// TODO Don't consider .. as invalid, but do consider /.. as invalid
		if (result.StartsWith(".."_sv, EIgnoreCase::No) || result.StartsWith(ParentDir))
		{
			return false;
		}

		// If there are no "/.."s left then we're done
		int32 Index = InPath.Find(ParentDir, EIgnoreCase::No);
		if (Index == -1)
		{
			break;
		}

		// Ignore folders beginning with dots
		while (true)
		{
			if (InPath.Len() <= Index + ParentDirLength || InPath[Index + ParentDirLength] == TEXT('/'))
			{
				break;
			}

			Index = InPath.Find(ParentDir, EIgnoreCase::No, ESearchDir::FromStart, Index + ParentDirLength);
			if (Index == -1)
			{
				break;
			}
		}

		if (Index == -1)
		{
			break;
		}

		int32 PreviousSeparatorIndex = Index;
		while (true)
		{
			// Find the previous slash
			PreviousSeparatorIndex = FMath::Max(0, InPath.Find( TEXT("/"), EIgnoreCase::No, ESearchDir::FromEnd, PreviousSeparatorIndex - 1));

			// Stop if we've hit the start of the string
			if (PreviousSeparatorIndex == 0)
			{
				break;
			}

			// Stop if we've found a directory that isn't "/./"
			if ((Index - PreviousSeparatorIndex) > 1 && (InPath[PreviousSeparatorIndex + 1] != TEXT('.') || InPath[PreviousSeparatorIndex + 2] != TEXT('/')))
			{
				break;
			}
		}

		// If we're attempting to remove the drive letter, that's illegal
		int32 Colon = InPath.Find(TEXT(":"), EIgnoreCase::No, ESearchDir::FromStart, PreviousSeparatorIndex);
		if (Colon >= 0 && Colon < Index)
		{
			return false;
		}

		InPath.RemoveAt(PreviousSeparatorIndex, Index - PreviousSeparatorIndex + ParentDirLength, false);
	}

	InPath.ReplaceInline(TEXT("./"), TEXT(""), EIgnoreCase::No);

	InPath.TrimToNullTerminator();

	return true;
#endif
}
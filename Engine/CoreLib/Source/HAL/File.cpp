#include "HAL/File.h"
#include "Engine/Logging.h"
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

bool FFile::ReadAllBytes(const FStringView fileName, TArray<uint8>& bytes)
{
	TErrorOr<TArray<uint8>> result = ReadAllBytes(fileName);
	if (result.IsError())
	{
		UM_LOG(Error, "{}", result.GetError());
		return false;
	}

	bytes = result.ReleaseValue();

	return true;
}

TErrorOr<TArray<uint8>> FFile::ReadAllBytes(const FStringView fileName)
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

bool FFile::ReadAllLines(const FStringView fileName, TArray<FString>& lines)
{
	FString fileText;
	if (ReadAllText(fileName, fileText) == false)
	{
		return false;
	}

	fileText.SplitByChars("\r\n"_sv, EStringSplitOptions::IgnoreEmptyEntries, lines);

	return true;
}

bool FFile::ReadAllText(const FStringView fileName, FString& text)
{
	TErrorOr<FString> result = ReadAllText(fileName);
	if (result.IsError())
	{
		UM_LOG(Error, "{}", result.GetError());
		return false;
	}

	text = result.ReleaseValue();

	return true;
}

TErrorOr<FString> FFile::ReadAllText(FStringView fileName)
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
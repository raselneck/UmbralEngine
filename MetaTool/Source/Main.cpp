#include "Containers/String.h"
#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "HAL/Directory.h"
#include "HAL/File.h"
#include "HAL/FileStream.h"
#include "HAL/FileSystem.h"
#include "HAL/Path.h"
#include "HAL/TextStreamWriter.h"
#include "Main/Main.h"
#include "MetaTool/HeaderFileGenerator.h"
#include "MetaTool/HeaderFileParser.h"
#include "MetaTool/SourceFileGenerator.h"

static FStringView GTargetName;
static FStringView GTargetSourceDirectory;
static FStringView GOutputDirectory;

/**
 * @brief Defines a context for generating meta reflection data.
 */
class FMetaGenerationContext
{
	UM_DISABLE_COPY(FMetaGenerationContext);
	UM_DISABLE_MOVE(FMetaGenerationContext);

public:

	/**
	 * @brief Sets default values for this meta generation context.
	 */
	FMetaGenerationContext() = default;

	/**
	 * @brief destroys this meta generation context.
	 */
	~FMetaGenerationContext() = default;

	/**
	 * @brief Gets a collection of paths to the generated header files.
	 *
	 * @return A collection of paths to the generated header files.
	 */
	[[nodiscard]] TSpan<const FString> GetGeneratedHeaderFiles() const
	{
		return m_GeneratedHeaderFiles.AsSpan();
	}

	/**
	 * @brief Gets a collection of paths to the generated source files.
	 *
	 * @return A collection of paths to the generated source files.
	 */
	[[nodiscard]] TSpan<const FString> GetGeneratedSourceFiles() const
	{
		return m_GeneratedSourceFiles.AsSpan();
	}

	/**
	 * @brief Gets the collection of header files.
	 *
	 * @return The collection of header files.
	 */
	[[nodiscard]] TSpan<const FStringView> GetHeaderFiles() const
	{
		return m_HeaderFiles.AsSpan();
	}

	/**
	 * @brief Checks to see if this context has written the given generated header file.
	 *
	 * @param path The path to check for.
	 * @return True if \p path has been written by this context.
	 */
	[[nodiscard]] bool HasWrittenGeneratedHeaderFile(const FString& path) const
	{
		// TODO This function currently assumes the given path is absolute and that all registered file paths are absolute

		return m_GeneratedHeaderFiles.ContainsByPredicate([&path](const FString& headerPath)
		{
			return path.Equals(headerPath, EIgnoreCase::Yes);
		});
	}

	/**
	 * @brief Checks to see if this context has written the given generated source file.
	 *
	 * @param path The path to check for.
	 * @return True if \p path has been written by this context.
	 */
	[[nodiscard]] bool HasWrittenGeneratedSourceFile(const FString& path) const
	{
		// TODO This function currently assumes the given path is absolute and that all registered file paths are absolute

		return m_GeneratedSourceFiles.ContainsByPredicate([&path](const FString& headerPath)
		{
			return path.Equals(headerPath, EIgnoreCase::Yes);
		});
	}

	/**
	 * @brief Attempts to read a file that contains a list of header files separated by semicolons.
	 *
	 * @param headersFilePath The path to the headers file.
	 * @return True if the headers file was read and parsed, otherwise false.
	 */
	[[nodiscard]] bool ReadHeadersFile(FStringView headersFilePath);

	/**
	 * @brief Scans all files.
	 */
	bool ScanFiles()
	{
		if (m_HeaderFiles.IsEmpty())
		{
			return true;
		}

		bool foundErrors = false;
		for (FStringView filePath : m_HeaderFiles)
		{
			if (filePath.IsEmpty())
			{
				continue;
			}

			foundErrors |= ScanFile(filePath) == false;
		}
		return foundErrors == false;
	}

private:

	/**
	 * @brief Scans a file.
	 *
	 * @param filePath The path of the file to scan.
	 */
	bool ScanFile(FStringView filePath);

	FString m_HeadersFileContent;
	TArray<FStringView> m_HeaderFiles;
	TArray<FString> m_GeneratedHeaderFiles;
	TArray<FString> m_GeneratedSourceFiles;
};

bool FMetaGenerationContext::ReadHeadersFile(const FStringView headersFilePath)
{
	TErrorOr<FString> readResult = FFile::ReadAllText(headersFilePath);
	if (readResult.IsError())
	{
		UM_LOG(Error, "{}", readResult.ReleaseError());
		return false;
	}

	m_HeadersFileContent = readResult.ReleaseValue();

	m_HeaderFiles.Reset();
	m_HeadersFileContent.SplitByCharsIntoViews(";"_sv, EStringSplitOptions::IgnoreEmptyEntries, m_HeaderFiles);

	return true;
}

bool FMetaGenerationContext::ScanFile(const FStringView filePath)
{
	FHeaderFileParser parser;
	const EHeaderFileParseResult parseResult = parser.ParseFile(filePath, GOutputDirectory);
	if (parseResult == EHeaderFileParseResult::GeneratedFileUpToDate)
	{
		m_GeneratedHeaderFiles.Add(FHeaderFileGenerator::GetTargetFilePath(filePath, GOutputDirectory));
		m_GeneratedSourceFiles.Add(FSourceFileGenerator::GetTargetFilePath(filePath, GOutputDirectory));
		return true;
	}
	else if (parseResult != EHeaderFileParseResult::Success)
	{
		return false;
	}

	if (parser.GetFoundClasses().IsEmpty() &&
	    parser.GetFoundEnums().IsEmpty() &&
	    parser.GetFoundStructs().IsEmpty())
	{
		return true;
	}

	FHeaderFileGenerator headerGenerator;
	if (headerGenerator.Begin(filePath, GOutputDirectory) == false)
	{
		UM_LOG(Error, "{}: Failed to begin writing header file for \"{}\"", GOutputDirectory, filePath);
		return false;
	}

	FSourceFileGenerator sourceGenerator;
	if (sourceGenerator.Begin(filePath, GOutputDirectory) == false)
	{
		UM_LOG(Error, "{}: Failed to begin writing source file for \"{}\"", GOutputDirectory, filePath);
		return false;
	}

	for (const FParsedClassInfo& classInfo : parser.GetFoundClasses())
	{
		headerGenerator.EmitClass(classInfo);
		sourceGenerator.EmitClass(classInfo);
	}
	for (const FParsedEnumInfo& enumInfo : parser.GetFoundEnums())
	{
		headerGenerator.EmitEnum(enumInfo);
		sourceGenerator.EmitEnum(enumInfo);
	}
	for (const FParsedStructInfo& structInfo : parser.GetFoundStructs())
	{
		headerGenerator.EmitStruct(structInfo);
		sourceGenerator.EmitStruct(structInfo);
	}

	(void)m_GeneratedHeaderFiles.Emplace(headerGenerator.GetTargetFilePath());
	(void)m_GeneratedSourceFiles.Emplace(sourceGenerator.GetTargetFilePath());

	return true;
}

/**
 * @brief The header tool's entry point.
 *
 * The header tool is expected to receive the target name as the first command line argument,
 * and then the path to the directory to generate the reflection files in as the second argument.
 *
 * @return The program exit code.
 */
extern "C" int32 UmbralMain()
{
	const auto PrintUsageWithMessage = [=](const FStringView message)
	{
		const FStringView appPath = FCommandLine::GetArgument(0);

		UM_LOG(Error, "{}", message);
		UM_LOG(Error, "Usage: {}  <TargetName> <TargetSourceDirectory> <OutputDirectory>", appPath);
	};

	if (FCommandLine::GetArgc() < 2)
	{
		PrintUsageWithMessage("Umbral Header Tool not given name of target"_sv);
		return EXIT_FAILURE;
	}

	if (FCommandLine::GetArgc() < 3)
	{
		PrintUsageWithMessage("Umbral Header Tool not given target source directory"_sv);
		return EXIT_FAILURE;
	}

	if (FCommandLine::GetArgc() < 4)
	{
		PrintUsageWithMessage("Umbral Header Tool not given output directory"_sv);
		return EXIT_FAILURE;
	}

	GTargetName = FCommandLine::GetArgument(1);
	GTargetSourceDirectory = FCommandLine::GetArgument(2);
	GOutputDirectory = FCommandLine::GetArgument(3);

	FFileSystem::SetCanAccessFilesAnywhere(true);

	const FString headersFileName = FString::Format("{}Headers.txt"_sv, GTargetName);
	const FString headersFilePath = FPath::Join(GOutputDirectory, headersFileName);

	FMetaGenerationContext context;
	if (context.ReadHeadersFile(headersFilePath) == false)
	{
		UM_LOG(Error, "Failed to read headers file \"{}\"; does it exist?", headersFilePath);
		return EXIT_FAILURE;
	}

	if (context.ScanFiles() == false)
	{
		return EXIT_FAILURE;
	}

	// Delete all files that shouldn't exist anymore
	TArray<FString> existingFiles;
	if (auto result = FDirectory::GetFiles(GOutputDirectory, false, existingFiles);
	    result.IsError())
	{
		UM_LOG(Error, "Failed to retrieve files in output directory \"{}\"\n{}", GOutputDirectory, result.GetError());
		return EXIT_FAILURE;
	}

	for (const FString& existingFile : existingFiles)
	{
		if (existingFile.EndsWith(".log"_sv) ||
		    existingFile.EndsWith(".txt"_sv) ||
		    context.HasWrittenGeneratedHeaderFile(existingFile) ||
		    context.HasWrittenGeneratedSourceFile(existingFile))
		{
			continue;
		}

		// UM_LOG(Warning, "May need to delete file \"{}\"", existingFile);

		const TErrorOr<void> deleteResult = FFile::Delete(existingFile);
		if (deleteResult.IsError())
		{
			UM_LOG(Error, "Failed to delete \"{}\". Reason: {}", existingFile, deleteResult.GetError().GetMessage());
		}
	}


	const FString targetFileName = FString { GTargetName } + ".Generated.cpp"_sv;
	const FString targetFilePath = FPath::Join(GOutputDirectory, targetFileName);

	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenWrite(targetFilePath);
	if (fileStream.IsNull())
	{
		UM_LOG(Error, "Failed to open \"{}\" for write", targetFilePath);
		return EXIT_FAILURE;
	}

	FTextStreamWriter writer;
	writer.SetFileStream(fileStream);

	writer.Write("// Full command line: "_sv);
	for (const FStringView& arg : FCommandLine::GetArguments())
	{
		if (arg.Contains(' '))
		{
			writer.Write("\""_sv);
			writer.Write(arg);
			writer.Write("\""_sv);
		}
		else
		{
			writer.Write(arg);
		}
		writer.Write(" "_sv);
	}
	writer.WriteLine();

	// Generate the source file
	writer.WriteLine("// Target name: {}"_sv, GTargetName);
	writer.WriteLine("// Source directory: {}"_sv, GTargetSourceDirectory);
	writer.WriteLine("// Output directory: {}"_sv, GOutputDirectory);
	writer.WriteLine("// Scanned files:"_sv);

	for (FStringView headerFile : context.GetHeaderFiles())
	{
		writer.WriteLine("// -- {}"_sv, headerFile);
	}

	writer.WriteLine("\n// Generated files:"_sv);
	for (FStringView generatedSourceFilePath : context.GetGeneratedSourceFiles())
	{
		writer.WriteLine("#include \"{}\""_sv, generatedSourceFilePath);
	}

	return EXIT_SUCCESS;
}
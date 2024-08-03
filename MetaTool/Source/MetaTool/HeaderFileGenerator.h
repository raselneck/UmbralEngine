#pragma once

#include "Containers/String.h"
#include "HAL/FileStream.h"
#include "Memory/SharedPtr.h"
#include "MetaTool/ClassInfo.h"
#include "MetaTool/EnumInfo.h"

/**
 * @brief Defines a helper for generating header files.
 */
class FHeaderFileGenerator final
{
public:

	/**
	 * @brief Destroys this header file generator.
	 */
	~FHeaderFileGenerator();

	/**
	 * @brief Attempts to begin generating a header file.
	 *
	 * @param sourceFilePath The path to the source file being generated from.
	 * @param targetFileDirectory The path to the directory that will contain the generated file.
	 * @return True if the header file was opened for writing, otherwise false.
	 */
	[[nodiscard]] bool Begin(FStringView sourceFilePath, FStringView targetFileDirectory);

	/**
	 * @brief Attempts to close the generated file.
	 *
	 * @return True if the file was closed, or false if there was an error or a file is not opened.
	 */
	[[nodiscard]] bool Close();

	/**
	 * @brief Emits generated code for a class.
	 *
	 * @param classInfo The class information.
	 */
	void EmitClass(const FParsedClassInfo& classInfo);

	/**
	 * @brief Emits generated code for an enum.
	 *
	 * @param enumInfo The enum information.
	 */
	void EmitEnum(const FParsedEnumInfo& enumInfo);

	/**
	 * @brief Emits generated code for a class.
	 *
	 * @param structInfo The class information.
	 */
	void EmitStruct(const FParsedStructInfo& structInfo);

	/**
	 * @brief Gets the target file path for the given source file.
	 *
	 * @param sourceFile The source file.
	 * @param targetFileDirectory The directory the generated header file will live in.
	 * @return The path of the generated header file.
	 */
	[[nodiscard]] static FString GetTargetFilePath(FStringView sourceFile, FStringView targetFileDirectory);

	/**
	 * @brief Gets the path to the generated file.
	 *
	 * @return The path to generated file.
	 */
	[[nodiscard]] FStringView GetTargetFilePath() const;

	/**
	 * @brief Checks to see if a header file is currently being generated.
	 *
	 * @return True if a header file is currently being generated, otherwise false.
	 */
	[[nodiscard]] bool IsGenerating() const;

private:

	/**
	 * @brief Emits the generated body macro for a class.
	 *
	 * @param classInfo The class info.
	 */
	void EmitClassGeneratedBodyMacro(const FParsedClassInfo& classInfo);

	/**
	 * @brief Emits the type definition template specialization for a class.
	 *
	 * @param classInfo The class info.
	 */
	void EmitClassTypeDefinition(const FParsedClassInfo& classInfo);

	/**
	 * @brief Implements the "ToString" function for an enum.
	 *
	 * @param enumInfo The enum info.
	 */
	void EmitEnumToStringFunction(const FParsedEnumInfo& enumInfo);

	/**
	 * @brief Emits the type definition template specialization for an enum.
	 *
	 * @param enumInfo The enum info.
	 */
	void EmitEnumTypeDefinition(const FParsedEnumInfo& enumInfo);

	/**
	 * @brief Emits the generated body macro for a struct.
	 *
	 * @param structInfo The struct info.
	 */
	void EmitStructGeneratedBodyMacro(const FParsedStructInfo& structInfo);

	/**
	 * @brief Emits the type definition template specialization for a struct.
	 *
	 * @param structInfo The struct info.
	 */
	void EmitStructTypeDefinition(const FParsedStructInfo& structInfo);

	TSharedPtr<IFileStream> m_FileStream;
	FStringView m_SourceFilePath;
	FStringView m_TargetFileDirectory;
	FString m_TargetFilePath;
	FString m_UniqueFileId;
	uint8 m_HasEmitClassHeader : 1 = false;
	uint8 m_HasEmitEnumHeader : 1 = false;
	uint8 m_HasEmitStructHeader : 1 = false;
	uint8 m_HasEmitObjectHeapVisitorDecl : 1 = false;
};
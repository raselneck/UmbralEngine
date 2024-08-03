#pragma once

#include "Containers/Function.h"
#include "Containers/StringView.h"
#include "HAL/FileStream.h"
#include "Memory/SharedPtr.h"
#include "MetaTool/ClassInfo.h"
#include "MetaTool/EnumInfo.h"

class FTextStreamWriter;

/**
 * @brief Defines a helper for generating source files.
 */
class FSourceFileGenerator final
{
public:

	/**
	 * @brief Destroys this header file generator.
	 */
	~FSourceFileGenerator();

	/**
	 * @brief Attempts to begin generating a source file.
	 *
	 * @param sourceFilePath The path to the source file being generated from.
	 * @param targetFileDirectory The path to the directory that will contain the generated file.
	 * @return True if the source file was opened for writing, otherwise false.
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
	 * @brief Emits generated code for a struct.
	 *
	 * @param structInfo The struct information.
	 */
	void EmitStruct(const FParsedStructInfo& structInfo);

	/**
	 * @brief Gets the target file path for the given source file.
	 *
	 * @param sourceFile The source file.
	 * @param targetFileDirectory The directory the generated source file will live in.
	 * @return The path of the generated source file.
	 */
	[[nodiscard]] static FString GetTargetFilePath(FStringView sourceFile, FStringView targetFileDirectory);

	/**
	 * @brief Gets the path to the generated file.
	 *
	 * @return The path to generated file.
	 */
	[[nodiscard]] FStringView GetTargetFilePath() const;

	/**
	 * @brief Checks to see if a source file is currently being generated.
	 *
	 * @return True if a source file is currently being generated, otherwise false.
	 */
	[[nodiscard]] bool IsGenerating() const;

private:

	/**
	 * @brief Emits the StaticType() function for a class.
	 *
	 * @param classInfo The class info.
	 */
	void EmitClassStaticTypeFunction(const FParsedClassInfo& classInfo);

	/**
	 * @brief Emits the type definition template specialization for an enum.
	 *
	 * @param enumInfo The enum info.
	 */
	void EmitEnumTypeDefinition(const FParsedEnumInfo& enumInfo);

	/**
	 * @brief Emits the StaticType() function definition for a struct or a class.
	 *
	 * @param typeInfo The type info.
	 * @param customWriteCallback The callback for writing custom information.
	 */
	void EmitStructOrClassStaticTypeFunction(const FParsedStructInfo& typeInfo, TFunction<void(FTextStreamWriter&)> customWriteCallback);

	/**
	 * @brief Emits the StaticType() function for a struct.
	 *
	 * @param structInfo The struct info.
	 */
	void EmitStructStaticTypeFunction(const FParsedStructInfo& structInfo);

	/**
	 * @brief Emits the type definition template specialization for a struct.
	 *
	 * @param structInfo The struct info.
	 */
	void EmitStructTypeDefinition(const FParsedStructInfo& structInfo);

	/**
	 * @brief Generates the VisitReferencedObjects function for a struct or class.
	 *
	 * @param typeInfo The struct or class info.
	 */
	void EmitStructOrClassVisitReferencedObjectsFunction(const FParsedStructInfo& typeInfo);

	TSharedPtr<IFileStream> m_FileStream;
	FStringView m_SourceFilePath;
	FStringView m_TargetFileDirectory;
	FString m_TargetFilePath;
};
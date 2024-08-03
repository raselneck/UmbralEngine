#include "HAL/FileSystem.h"
#include "HAL/Path.h"
#include "HAL/TextStreamWriter.h"
#include "Misc/CString.h"
#include "Misc/StringBuilder.h"
#include "MetaTool/HeaderFileGenerator.h"
#include <cstring>

static FString GenerateUniqueFileId(const FStringView filePath)
{
	constexpr const FStringView projectSourceDir = UM_JOIN(PROJECT_SOURCE_DIR, _sv);
	constexpr const FStringView fileIdPrefix = "UM_FILE_ID"_sv;

	FStringBuilder result;
	result.Reserve(filePath.Length() - projectSourceDir.Length() + fileIdPrefix.Length());
	result.Append(fileIdPrefix);

	for (int32 idx = projectSourceDir.Length(); idx < filePath.Length(); ++idx)
	{
		const char ch = filePath[idx];
		if (FCString::IsAlphaNumeric(ch))
		{
			result.Append(ch, 1);
		}
		else
		{
			result.Append('_', 1);
		}
	}

	return result.ReleaseString();
}

FHeaderFileGenerator::~FHeaderFileGenerator()
{
	(void)Close();
}

bool FHeaderFileGenerator::Begin(const FStringView sourceFilePath, const FStringView targetFileDirectory)
{
	if (m_FileStream.IsValid() && m_FileStream->IsOpen())
	{
		return false;
	}

	m_SourceFilePath = sourceFilePath;
	m_TargetFileDirectory = targetFileDirectory;
	m_UniqueFileId = GenerateUniqueFileId(sourceFilePath);
	m_TargetFilePath = GetTargetFilePath(m_SourceFilePath, targetFileDirectory);

	m_FileStream = FFileSystem::OpenWrite(m_TargetFilePath);
	if (m_FileStream.IsNull() || m_FileStream->IsOpen() == false)
	{
		return false;
	}

	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	// Output the initial code common to every header
	writer.WriteLine("#pragma once\n"_sv);
	writer.WriteLine("#ifdef UMBRAL_CURRENT_FILE_ID"_sv);
	writer.WriteLine("#\tundef UMBRAL_CURRENT_FILE_ID"_sv);
	writer.WriteLine("#endif\n"_sv);
	writer.WriteLine("#define UMBRAL_CURRENT_FILE_ID {}\n"_sv, m_UniqueFileId);

	return true;
}

bool FHeaderFileGenerator::Close()
{
	if (m_FileStream.IsNull() || m_FileStream->IsOpen() == false)
	{
		return false;
	}

	m_FileStream->Flush();
	m_FileStream->Close();
	m_FileStream.Reset();

	return true;
}

void FHeaderFileGenerator::EmitClass(const FParsedClassInfo& classInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	if (m_HasEmitClassHeader == false)
	{
		writer.WriteLine("#include \"Meta/ClassInfo.h\"\n"_sv);
		m_HasEmitClassHeader = true;
	}

	if (m_HasEmitObjectHeapVisitorDecl == false)
	{
		writer.WriteLine("class FObjectHeapVisitor;\n"_sv);
		m_HasEmitObjectHeapVisitorDecl = true;
	}

	writer.WriteLine("class {};\n"_sv, classInfo.TypeName);
	EmitClassGeneratedBodyMacro(classInfo);
	m_FileStream->Write("\n\n"_sv);
	EmitClassTypeDefinition(classInfo);
	m_FileStream->Write("\n\n"_sv);
}

void FHeaderFileGenerator::EmitEnum(const FParsedEnumInfo& enumInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	if (m_HasEmitEnumHeader == false)
	{
		writer.WriteLine("#include \"Meta/EnumInfo.h\"\n"_sv);
		m_HasEmitEnumHeader = true;
	}

	// TODO Forward declaring this as "enum class" may be inaccurate
	writer.WriteLine("enum class {} : {};\n"_sv, enumInfo.EnumName, enumInfo.UnderlyingType);

	EmitEnumTypeDefinition(enumInfo);
	m_FileStream->Write("\n\n"_sv);
	EmitEnumToStringFunction(enumInfo);
	m_FileStream->Write("\n\n"_sv);
}

void FHeaderFileGenerator::EmitStruct(const FParsedStructInfo& structInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	if (m_HasEmitStructHeader == false)
	{
		writer.WriteLine("#include \"Meta/StructInfo.h\"\n"_sv);
		m_HasEmitStructHeader = true;
	}

	if (m_HasEmitObjectHeapVisitorDecl == false)
	{
		writer.WriteLine("class FObjectHeapVisitor;\n"_sv);
		m_HasEmitObjectHeapVisitorDecl = true;
	}

	// TODO Forward declaring as a class is presumptuous. Need to check if the type was declared as a class or struct
	writer.WriteLine("class {};\n"_sv, structInfo.TypeName);

	EmitStructGeneratedBodyMacro(structInfo);
	m_FileStream->Write("\n\n"_sv);
	EmitStructTypeDefinition(structInfo);
	m_FileStream->Write("\n\n"_sv);
}

FString FHeaderFileGenerator::GetTargetFilePath(const FStringView sourceFile, const FStringView targetFileDirectory)
{
	FStringBuilder targetFileName;
	targetFileName.Append(FPath::GetBaseFileName(sourceFile))
	              .Append(".Generated.h"_sv);

	return FPath::Join(targetFileDirectory, targetFileName.AsStringView());
}

FStringView FHeaderFileGenerator::GetTargetFilePath() const
{
	return m_TargetFilePath;
}

bool FHeaderFileGenerator::IsGenerating() const
{
	return m_FileStream.IsValid() && m_FileStream->IsOpen();
}

static void WriteStaticTypeAndGetTypeFunctions(FTextStreamWriter& writer, const FParsedStructInfo& typeInfo)
{
	const FStringView structOrClass = Is<FParsedClassInfo>(typeInfo) ? "Class"_sv : "Struct"_sv;
	const FStringView overrideSpecifier = typeInfo.BaseTypeName.IsEmpty() ? ""_sv : " override"_sv;

	writer.WriteLine("public: \\"_sv);
	writer.WriteLine("[[nodiscard]] static const F{}Info* StaticType(); \\"_sv, structOrClass);

	writer.WriteLine("[[nodiscard]] virtual const F{}Info* GetType() const{}\\"_sv, structOrClass, overrideSpecifier);
	writer.WriteLine("{ \\"_sv);
	writer.Indent();
	writer.WriteLine("return {}::StaticType(); \\"_sv, typeInfo.TypeName);
	writer.Unindent();
	writer.WriteLine("} \\"_sv);

	if (typeInfo.BaseTypeName.IsEmpty() || typeInfo.HasObjectProperties())
	{
		writer.WriteLine("virtual void VisitReferencedObjects(FObjectHeapVisitor& visitor){}; \\"_sv, overrideSpecifier);
	}
}

void FHeaderFileGenerator::EmitClassGeneratedBodyMacro(const FParsedClassInfo& classInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("#define {}_{}_GENERATED_BODY() \\"_sv, m_UniqueFileId, classInfo.GeneratedBodyMacroLine);
	writer.Indent();
	writer.WriteLine("private: \\"_sv);
	writer.WriteLine("friend class FObjectHeap; \\"_sv);
	writer.WriteLine("protected: \\"_sv);
	if (classInfo.BaseTypeName.IsEmpty())
	{
		writer.WriteLine("{}(); \\"_sv, classInfo.TypeName);
	}
	else
	{
		writer.WriteLine("{}() = default; \\"_sv, classInfo.TypeName);
	}
	writer.WriteLine("UM_DISABLE_COPY({}); \\"_sv, classInfo.TypeName);
	writer.WriteLine("UM_DISABLE_MOVE({}); \\"_sv, classInfo.TypeName);

	WriteStaticTypeAndGetTypeFunctions(writer, classInfo);

	// Emit typedefs for self and super classes. Ensure that the macro must be used with a semicolon
	writer.WriteLine("private: \\"_sv);
	if (classInfo.BaseTypeName.Length() > 0)
	{
		writer.WriteLine("using Super = {}; \\"_sv, classInfo.BaseTypeName);
	}
	writer.WriteLine("using ThisClass = {}"_sv, classInfo.TypeName);
}

void FHeaderFileGenerator::EmitClassTypeDefinition(const FParsedClassInfo& classInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("namespace Private"_sv);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("template<> struct TTypeDefinition<{}>"_sv, classInfo.TypeName);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("static const FClassInfo* Get();"_sv);
	writer.Unindent();
	writer.WriteLine("};"_sv);
	writer.Unindent();
	writer.WriteLine("}"_sv);
}

void FHeaderFileGenerator::EmitEnumToStringFunction(const FParsedEnumInfo& enumInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("inline FStringView ToString(const {} value)"_sv, enumInfo.EnumName);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("const FEnumEntryInfo* entry = ::GetType<{}>()->GetEntryByValue(value);"_sv, enumInfo.EnumName);
	writer.WriteLine("return entry == nullptr ? \"<invalid>\"_sv : entry->GetName();"_sv);
	writer.Unindent();
	writer.WriteLine("}"_sv);
}

void FHeaderFileGenerator::EmitEnumTypeDefinition(const FParsedEnumInfo& enumInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("namespace Private"_sv);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("template<> struct TTypeDefinition<{}>"_sv, enumInfo.EnumName);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("static const FEnumInfo* Get();"_sv);
	writer.Unindent();
	writer.WriteLine("};"_sv);
	writer.Unindent();
	writer.WriteLine("}"_sv);
}

void FHeaderFileGenerator::EmitStructGeneratedBodyMacro(const FParsedStructInfo& structInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("#define {}_{}_GENERATED_BODY() \\"_sv, m_UniqueFileId, structInfo.GeneratedBodyMacroLine);
	writer.Indent();

	WriteStaticTypeAndGetTypeFunctions(writer, structInfo);

	// Emit typedefs for self and super classes. Ensure that the macro must be used with a semicolon
	writer.WriteLine("private: \\"_sv);
	if (structInfo.BaseTypeName.Length() > 0)
	{
		writer.WriteLine("using Super = {}; \\"_sv, structInfo.BaseTypeName);
	}
	writer.WriteLine("using ThisClass = {}"_sv, structInfo.TypeName);
}

void FHeaderFileGenerator::EmitStructTypeDefinition(const FParsedStructInfo& structInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("namespace Private"_sv);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("template<> struct TTypeDefinition<{}>"_sv, structInfo.TypeName);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("static const FStructInfo* Get();"_sv);
	writer.Unindent();
	writer.WriteLine("};"_sv);
	writer.Unindent();
	writer.WriteLine("}"_sv);
}
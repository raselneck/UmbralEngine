#include "Engine/Logging.h"
#include "HAL/FileSystem.h"
#include "HAL/Path.h"
#include "HAL/TextStreamWriter.h"
#include "MetaTool/SourceFileGenerator.h"

FSourceFileGenerator::~FSourceFileGenerator()
{
	(void)Close();
}

bool FSourceFileGenerator::Begin(const FStringView sourceFilePath, const FStringView targetFileDirectory)
{
	if (m_FileStream.IsValid() && m_FileStream->IsOpen())
	{
		return false;
	}

	m_SourceFilePath = sourceFilePath;
	m_TargetFileDirectory = targetFileDirectory;
	m_TargetFilePath = GetTargetFilePath(m_SourceFilePath, m_TargetFileDirectory);

	m_FileStream = FFileSystem::OpenWrite(m_TargetFilePath);
	if (m_FileStream.IsNull() || m_FileStream->IsOpen() == false)
	{
		return false;
	}

	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	// Output the initial code common to every source file
	writer.WriteLine("#include \"{}\""_sv, m_SourceFilePath);
	writer.WriteLine("#include \"Meta/ArrayTypeInfo.h\""_sv);
	writer.WriteLine("#include \"Templates/CanVisitReferencedObjects.h\""_sv);
	writer.WriteLine("#include \"Templates/IsConstructible.h\""_sv);
	writer.WriteLine();

	return true;
}

bool FSourceFileGenerator::Close()
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

void FSourceFileGenerator::EmitClass(const FParsedClassInfo& classInfo)
{
	EmitClassStaticTypeFunction(classInfo);
	m_FileStream->Write("\n"_sv);
	EmitStructTypeDefinition(classInfo);
	m_FileStream->Write("\n"_sv);

	if (classInfo.BaseTypeName.IsEmpty() || classInfo.HasObjectProperties())
	{
		EmitStructOrClassVisitReferencedObjectsFunction(classInfo);
		m_FileStream->Write("\n"_sv);
	}
}

void FSourceFileGenerator::EmitEnum(const FParsedEnumInfo& enumInfo)
{
	EmitEnumTypeDefinition(enumInfo);
	m_FileStream->Write("\n"_sv);
}

void FSourceFileGenerator::EmitStruct(const FParsedStructInfo& structInfo)
{
	EmitStructStaticTypeFunction(structInfo);
	m_FileStream->Write("\n"_sv);
	EmitStructTypeDefinition(structInfo);
	m_FileStream->Write("\n"_sv);

	if (structInfo.BaseTypeName.IsEmpty() || structInfo.HasObjectProperties())
	{
		EmitStructOrClassVisitReferencedObjectsFunction(structInfo);
		m_FileStream->Write("\n"_sv);
	}
}

FString FSourceFileGenerator::GetTargetFilePath(const FStringView sourceFile, const FStringView targetFileDirectory)
{
	FStringBuilder targetFileName;
	targetFileName.Append(FPath::GetBaseFileName(sourceFile))
	    .Append(".Generated.cpp"_sv);

	return FPath::Join(targetFileDirectory, targetFileName.AsStringView());
}

FStringView FSourceFileGenerator::GetTargetFilePath() const
{
	return m_TargetFilePath;
}

bool FSourceFileGenerator::IsGenerating() const
{
	return m_FileStream.IsValid() && m_FileStream->IsOpen();
}

void FSourceFileGenerator::EmitClassStaticTypeFunction(const FParsedClassInfo& classInfo)
{
	EmitStructOrClassStaticTypeFunction(classInfo, [this, &classInfo](FTextStreamWriter& writer)
	{
		(void)this;
		(void)writer;

		writer.WriteLine("typeInfo.SetConstructClassAtLocationFunction([](TBadge<class FClassInfo>, void* location)"_sv);
		writer.WriteLine("{"_sv);
		writer.Indent();

		writer.WriteLine("new (location) {};"_sv, classInfo.TypeName);

		writer.Unindent();
		writer.WriteLine("});"_sv);

		// TODO Need to emit the functions
	});
}

void FSourceFileGenerator::EmitEnumTypeDefinition(const FParsedEnumInfo& enumInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("const FEnumInfo* Private::TTypeDefinition<{}>::Get()"_sv, enumInfo.EnumName);
	writer.WriteLine("{"_sv);
	writer.Indent();

	writer.WriteLine("static const FEnumInfo GEnumInfo_{} = []() -> FEnumInfo"_sv, enumInfo.EnumName);
	writer.WriteLine("{"_sv);
	writer.Indent();

	writer.WriteLine("FEnumInfo enumInfo \\{ \"{}\"_sv, ::GetType<{}>() };"_sv, enumInfo.EnumName, enumInfo.UnderlyingType);

	for (const FParsedEnumEntryInfo& entryInfo : enumInfo.EnumEntries)
	{
		writer.WriteLine("enumInfo.AddEntry(\"{}\"_sv, {}::{});"_sv, entryInfo.EntryName, enumInfo.EnumName, entryInfo.EntryName);
	}

	writer.WriteLine("return enumInfo;"_sv);
	writer.Unindent();

	writer.WriteLine("}();"_sv);
	writer.WriteLine("return &GEnumInfo_{};"_sv, enumInfo.EnumName);
	writer.Unindent();

	writer.WriteLine("}"_sv);
}

void FSourceFileGenerator::EmitStructOrClassStaticTypeFunction(const FParsedStructInfo& typeInfo, TFunction<void(FTextStreamWriter&)> customWriteCallback)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	const bool isClassInfo = Is<FParsedClassInfo>(typeInfo);
	const FStringView type = isClassInfo ? "Class"_sv : "Struct"_sv;

	writer.WriteLine("const F{}Info* {}::StaticType()"_sv, type, typeInfo.TypeName);
	writer.WriteLine("{"_sv);
	writer.Indent();

	writer.WriteLine("static const F{}Info G{}Info_{} = []() -> F{}Info"_sv, type, type, typeInfo.TypeName, type);
	writer.WriteLine("{"_sv);
	writer.Indent();

	if (typeInfo.BaseTypeName.IsEmpty())
	{
		writer.WriteLine("const F{}Info* baseType = nullptr;"_sv, type);
	}
	else
	{
		writer.WriteLine("const F{}Info* baseType = ::GetType<{}>();"_sv, type, typeInfo.BaseTypeName);
	}

	writer.WriteLine("F{}Info typeInfo \\{ \"{}\"_sv, sizeof({}), alignof({}), baseType };"_sv, type, typeInfo.TypeName, typeInfo.TypeName, typeInfo.TypeName);

	const auto EmitAttribute = [&](const FStringView varName, const FParsedAttributeInfo& attribute)
	{
		if (attribute.Value.IsEmpty())
		{
			writer.WriteLine("(void){}.AddAttribute(\"{}\"_sv);"_sv, varName, attribute.Name);
		}
		else
		{
			writer.WriteLine("(void){}.AddAttribute(\"{}\"_sv, \"{}\"_sv);"_sv, varName, attribute.Name, attribute.Value);
		}
	};

	// Emit the attributes
	for (const FParsedAttributeInfo& attribute : typeInfo.Attributes)
	{
		EmitAttribute("typeInfo"_sv, attribute);
	}

	// Emit the properties
	for (int32 idx = 0; idx < typeInfo.Properties.Num(); ++idx)
	{
		const FParsedPropertyInfo& property = typeInfo.Properties[idx];
		const FString propertyVarName = FString::Format("prop_{}"_sv, idx);

		for (const FString& defineConstraint : property.DefineConstraints)
		{
			writer.WriteLineNoIndent("#if {}"_sv, defineConstraint);
		}

		writer.WriteLine("FPropertyInfo& {} = typeInfo.AddProperty(\"{}\"_sv, ::GetType<{}>(), UM_OFFSET_OF({}, {}));"_sv,
		                 propertyVarName,
		                 property.PropertyName,
		                 property.PropertyType,
		                 typeInfo.TypeName,
		                 property.PropertyName);

		if (property.Attributes.IsEmpty())
		{
			writer.WriteLine("(void){};"_sv, propertyVarName);
		}
		else
		{
			// Emit the property attributes
			for (const FParsedAttributeInfo& attribute : property.Attributes)
			{
				EmitAttribute(propertyVarName, attribute);
			}
		}

		for (const FString& defineConstraint : property.DefineConstraints)
		{
			(void)defineConstraint;
			writer.WriteLineNoIndent("#endif"_sv);
		}
	}

	if (customWriteCallback.IsValid())
	{
		customWriteCallback(writer);
	}

	writer.WriteLine("return typeInfo;"_sv);
	writer.Unindent();

	writer.WriteLine("}();"_sv);
	writer.WriteLine("return &G{}Info_{};"_sv, type, typeInfo.TypeName);
	writer.Unindent();

	writer.WriteLine("}"_sv);
}

void FSourceFileGenerator::EmitStructStaticTypeFunction(const FParsedStructInfo& structInfo)
{
	EmitStructOrClassStaticTypeFunction(structInfo, [this, &structInfo](FTextStreamWriter& writer)
	{
		(void)this;

		writer.WriteLine("if constexpr (IsDefaultConstructible<{}>)"_sv, structInfo.TypeName);
		writer.WriteLine("{"_sv);
		writer.Indent();

		// TODO Write default construct stuff once it's supported

		writer.Unindent();
		writer.WriteLine("}"_sv);
	});
}

void FSourceFileGenerator::EmitStructTypeDefinition(const FParsedStructInfo& typeInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	const bool isClassInfo = Is<FParsedClassInfo>(typeInfo);
	const FStringView type = isClassInfo ? "Class"_sv : "Struct"_sv;

	writer.WriteLine("const F{}Info* Private::TTypeDefinition<{}>::Get()"_sv, type, typeInfo.TypeName);
	writer.WriteLine("{"_sv);
	writer.Indent();
	writer.WriteLine("return {}::StaticType();"_sv, typeInfo.TypeName);
	writer.Unindent();
	writer.WriteLine("}"_sv);
}

void FSourceFileGenerator::EmitStructOrClassVisitReferencedObjectsFunction(const FParsedStructInfo& typeInfo)
{
	FTextStreamWriter writer;
	writer.SetFileStream(m_FileStream);

	writer.WriteLine("void {}::VisitReferencedObjects(FObjectHeapVisitor& visitor)"_sv, typeInfo.TypeName);
	writer.WriteLine("{"_sv);
	writer.Indent();

	writer.WriteLine("(void)visitor;\n"_sv);

	if (typeInfo.BaseTypeName.IsEmpty())
	{
		if (typeInfo.IsObjectClass())
		{
			writer.WriteLine("visitor.Visit(this);"_sv);
		}
	}
	else
	{
		writer.WriteLine("Super::VisitReferencedObjects(visitor);"_sv);

		// Don't need to generate any more of the function if there aren't any object properties
		if (typeInfo.HasObjectProperties() == false)
		{
			writer.Unindent();
			writer.WriteLine("}"_sv);

			return;
		}

		writer.WriteLine();
	}

	for (const FParsedPropertyInfo& property : typeInfo.Properties)
	{
		if (property.IsObjectProperty() == false)
		{
			continue;
		}

		for (const FString& defineConstraint : property.DefineConstraints)
		{
			writer.WriteLineNoIndent("#if {}"_sv, defineConstraint);
		}

		writer.WriteLine("visitor.Visit({});"_sv, property.PropertyName);

		for (const FString& defineConstraint : property.DefineConstraints)
		{
			(void)defineConstraint;
			writer.WriteLineNoIndent("#endif"_sv);
		}
	}

	// HACK Manually call this for UObject for now
	if (typeInfo.TypeName == "UObject"_sv)
	{
		writer.WriteLine();
		writer.WriteLine("ManuallyVisitReferencedObjects(visitor);"_sv);
	}

	writer.Unindent();
	writer.WriteLine("}"_sv);
}
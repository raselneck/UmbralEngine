#include "Containers/Optional.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include "MetaTool/HeaderFileGenerator.h"
#include "MetaTool/HeaderFileParser.h"
#include "MetaTool/MetaMacroNames.h"
#include "MetaTool/SourceFileGenerator.h"

namespace TreeSitter
{
	void* Malloc(const size_t size)
	{
		return FMemory::Allocate(static_cast<FMemory::SizeType>(size));
	}

	void* Calloc(const size_t num, const size_t size)
	{
		return FMemory::AllocateArray(static_cast<FMemory::SizeType>(num), static_cast<FMemory::SizeType>(size));
	}

	void* Realloc(void* memory, const size_t newSize)
	{
		return FMemory::Reallocate(memory, static_cast<FMemory::SizeType>(newSize));
	}

	void Free(void* memory)
	{
		return FMemory::Free(memory);
	}
}

class FTreeSitterParserDeleter
{
public:

	void Delete(void* memory) const
	{
		ts_parser_delete(reinterpret_cast<TSParser*>(memory));
	}
};

class FTreeSitterTreeDeleter
{
public:

	void Delete(void* memory) const
	{
		ts_tree_delete(reinterpret_cast<TSTree*>(memory));
	}
};

enum class EReflectionHeaderType
{
	Invalid,
	Class,
	Enum,
	Function,
	Property,
	Struct,
	GeneratedBody
};

namespace Private
{
	template<>
	struct TEnumToString<EReflectionHeaderType>
	{
		static constexpr FStringView Get(const EReflectionHeaderType value)
		{
			switch (value)
			{
			case EReflectionHeaderType::Class:          return MacroNames::ClassSpecifier;
			case EReflectionHeaderType::Enum:           return MacroNames::EnumSpecifier;
			case EReflectionHeaderType::Function:       return MacroNames::FunctionSpecifier;
			case EReflectionHeaderType::Property:       return MacroNames::PropertySpecifier;
			case EReflectionHeaderType::Struct:         return MacroNames::StructSpecifier;
			case EReflectionHeaderType::GeneratedBody:  return MacroNames::GeneratedBodySpecifier;
			case EReflectionHeaderType::Invalid:
			default: return "<invalid>"_sv;
			}
		}
	};

	template<>
	struct TStringToEnum<EReflectionHeaderType>
	{
		using EnumToStringType = TEnumToString<EReflectionHeaderType>;

		static constexpr EReflectionHeaderType Get(const FStringView text)
		{
			#define RETURN_ENUM_TO_STRING_CASE(EnumName) \
				if (text == EnumToStringType::Get(EReflectionHeaderType:: EnumName)) \
				{ \
					return EReflectionHeaderType:: EnumName; \
				}

			RETURN_ENUM_TO_STRING_CASE(Class)
			RETURN_ENUM_TO_STRING_CASE(Enum)
			RETURN_ENUM_TO_STRING_CASE(Function)
			RETURN_ENUM_TO_STRING_CASE(Property)
			RETURN_ENUM_TO_STRING_CASE(Struct)
			RETURN_ENUM_TO_STRING_CASE(GeneratedBody)

			return EReflectionHeaderType::Invalid;

			#undef RETURN_ENUM_TO_STRING_CASE
		}
	};
}

class FReflectionHeaderInfo
{
public:

	EReflectionHeaderType Type = EReflectionHeaderType::Invalid;
	FStringView Identifier;
	TArray<FParsedAttributeInfo> Attributes;
	TSRange Range = {};

	[[nodiscard]] bool IsValid() const
	{
		return Type != EReflectionHeaderType::Invalid && Identifier.IsEmpty() == false;
	}

	void Reset()
	{
		Type = EReflectionHeaderType::Invalid;
		Identifier = {};
		Attributes.Reset();
		Range = {};
	}
};

#define QUICK_DUMP_NODE [this](TSNode n) \
	{ \
		const uint32 cc = ts_node_named_child_count(n); \
		if (cc == 0) \
		{ \
			LogMessage(n, "{} node has no children"_sv, ts_node_type(n)); \
			return; \
		} \
		LogMessage(n, "Dumping {} node children:"_sv, ts_node_type(n)); \
		for (uint32 i = 0; i < cc; ++i) { \
			TSNode cn = ts_node_named_child(n, i); \
			UM_LOG(Info, "\t{"); \
			UM_LOG(Info, "\t\t\"index\": \"{} / {}\",", i + 1, cc); \
			UM_LOG(Info, "\t\t\"type\": \"{}\",", ts_node_type(cn)); \
			UM_LOG(Info, "\t\t\"text\": \"{}\"", GetNodeSourceText(cn)); \
			UM_LOG(Info, "\t}"); \
		} \
	}

extern "C" const TSLanguage* tree_sitter_cpp(void);

[[nodiscard]] static bool AreGeneratedFilesNewerThanSourceFile(const FStringView sourceFile, const FStringView targetFileDirectory)
{
	using FGetTargetFilePathFunction = FString(*)(FStringView, FStringView);

	const auto IsGeneratedFileNewerThanSourceFile = [sourceFile, targetFileDirectory](FGetTargetFilePathFunction getTargetFilePathFunction)
	{
		const FFileStats sourceFileStats = FFile::Stat(sourceFile);
		if (sourceFileStats.Exists == false)
		{
			return false;
		}

		const FString targetFilePath = getTargetFilePathFunction(sourceFile, targetFileDirectory);
		const FFileStats targetFileStats = FFile::Stat(targetFilePath);
		if (targetFileStats.Exists == false)
		{
			return false;
		}

		return targetFileStats.ModifiedTime >= sourceFileStats.ModifiedTime;
	};

	return IsGeneratedFileNewerThanSourceFile(FHeaderFileGenerator::GetTargetFilePath)
	       && IsGeneratedFileNewerThanSourceFile(FSourceFileGenerator::GetTargetFilePath);
}

[[nodiscard]] static TSNode GetChildNodeOfType(TSNode node, const FStringView type)
{
	const uint32 childCount = ts_node_named_child_count(node);
	for (uint32 idx = 0; idx < childCount; ++idx)
	{
		TSNode childNode = ts_node_named_child(node, idx);
		const FStringView childType { ts_node_type(childNode) };

		if (childType == type)
		{
			return childNode;
		}
	}

	return {};
}

[[maybe_unused]] static TArray<TSNode> GetChildNodesOfType(TSNode node, const FStringView type)
{
	TArray<TSNode> children;

	const uint32 childCount = ts_node_named_child_count(node);
	for (uint32 idx = 0; idx < childCount; ++idx)
	{
		TSNode childNode = ts_node_named_child(node, idx);
		const FStringView childType { ts_node_type(childNode) };

		if (childType == type)
		{
			children.Add(childNode);
		}
	}

	return children;
}

[[maybe_unused]] static TArray<TSNode> GetChildNodes(TSNode node)
{
	TArray<TSNode> children;

	const uint32 childCount = ts_node_named_child_count(node);
	for (uint32 idx = 0; idx < childCount; ++idx)
	{
		TSNode childNode = ts_node_named_child(node, idx);
		children.Add(childNode);
	}

	return children;
}

[[nodiscard]] static bool IsValid(TSNode node)
{
	return node.tree != nullptr;
}

[[nodiscard]] static bool IsValidAndOfType(TSNode node, const FStringView desiredType)
{
	if (IsValid(node) == false)
	{
		return false;
	}

	const FStringView nodeType { ts_node_type(node) };
	return nodeType == desiredType;
}

static void PopulateRangeFromNode(TSNode node, TSRange& range)
{
	range.start_point = ts_node_start_point(node);
	range.end_point = ts_node_end_point(node);
	range.start_byte = ts_node_start_byte(node);
	range.end_byte = ts_node_end_byte(node);
}

// TODO It would probably make parsing stuff here easier if we had a concept of a "type" parser
//      i.e. FTypeParser -> FEnumParser, FStructParser -> FClassParser where class can have methods and struct cannot

TSpan<const FStringView> FHeaderFileParser::GetIncludedFiles() const
{
	return m_IncludedFiles.AsSpan();
}

TSpan<const FParsedClassInfo> FHeaderFileParser::GetFoundClasses() const
{
	return m_FoundClasses.AsSpan();
}

TSpan<const FParsedEnumInfo> FHeaderFileParser::GetFoundEnums() const
{
	return m_FoundEnums.AsSpan();
}

TSpan<const FParsedStructInfo> FHeaderFileParser::GetFoundStructs() const
{
	return m_FoundStructs.AsSpan();
}

bool FHeaderFileParser::IncludesFile(const FStringView filePath) const
{
	return m_IncludedFiles.ContainsByPredicate([filePath](const FStringView& includedFile)
	{
		return includedFile == filePath;
	});
}

EHeaderFileParseResult FHeaderFileParser::ParseFile(const FStringView filePath, const FStringView targetFileDirectory)
{
#if WITH_MODULE_EMITTED_TYPES == 0
	if (AreGeneratedFilesNewerThanSourceFile(filePath, targetFileDirectory))
	{
		// TODO Re-enable with module emitted types when we have some kind of caching system for the types we emitted
		return EHeaderFileParseResult::GeneratedFileUpToDate;
	}
#endif

	ts_set_allocator(TreeSitter::Malloc, TreeSitter::Calloc, TreeSitter::Realloc, TreeSitter::Free);

	m_FilePath = filePath;
	m_FileSource.Reset();
	m_FoundClasses.Reset();

	TErrorOr<FString> readResult = FFile::ReadAllText(filePath);
	if (readResult.IsError())
	{
		UM_LOG(Error, "{}", readResult.ReleaseError());
		return EHeaderFileParseResult::CouldNotReadFile;
	}

	m_FileSource = readResult.ReleaseValue();

	TUniquePtr<TSParser, FTreeSitterParserDeleter> parser { ts_parser_new() };
	if (parser.IsNull())
	{
		UM_LOG(Error, "Failed to create parser");
		return EHeaderFileParseResult::ParseError;
	}

	ts_parser_set_language(parser.Get(), tree_sitter_cpp());

	TUniquePtr<TSTree, FTreeSitterTreeDeleter> parseTree
	{
		ts_parser_parse_string(parser.Get(), nullptr, m_FileSource.GetChars(), static_cast<uint32>(m_FileSource.Length()))
	};
	if (parseTree.IsNull())
	{
		UM_LOG(Error, "Failed to parse file \"{}\"", m_FilePath);
		return EHeaderFileParseResult::ParseError;
	}

	TSNode translationUnit = ts_tree_root_node(parseTree.Get());
	const FStringView rootNodeType { ts_node_type(translationUnit) };
	if (rootNodeType != "translation_unit"_sv)
	{
		UM_LOG(Error, "Root node is not a translation unit for C++ file \"{}\"", filePath);
		return EHeaderFileParseResult::ParseError;
	}

	if (VisitTranslationUnit(translationUnit))
	{
		return EHeaderFileParseResult::Success;
	}

	return EHeaderFileParseResult::ParseError;
}

FStringView FHeaderFileParser::GetNodeSourceText(TSNode node) const
{
	const uint32 startByte = ts_node_start_byte(node);
	const uint32 endByte = ts_node_end_byte(node);
	return m_FileSource.SubstringView(static_cast<int32>(startByte), static_cast<int32>(endByte - startByte));
}

bool FHeaderFileParser::ParseArgumentListAsAttributeList(TSNode argumentList, TArray<FParsedAttributeInfo>& attributes) const
{
	if (IsValidAndOfType(argumentList, "argument_list"_sv) == false)
	{
		return false;
	}

	bool foundErrors = false;
	const uint32 argCount = ts_node_named_child_count(argumentList);
	for (uint32 idx = 0; idx < argCount; ++idx)
	{
		TSNode argNode = ts_node_named_child(argumentList, idx);
		const FStringView argType { ts_node_type(argNode) };

		if (argType == "identifier"_sv)
		{
			FParsedAttributeInfo& attribute = attributes.AddDefaultGetRef();
			attribute.Name = GetNodeSourceText(argNode);
		}
		else if (argType == "assignment_expression"_sv)
		{
			// TODO Dedicated function for this so we can validate nodes and values

			FParsedAttributeInfo& attribute = attributes.AddDefaultGetRef();
			attribute.Name = GetNodeSourceText(ts_node_named_child(argNode, 0));
			attribute.Value = GetNodeSourceText(ts_node_named_child(argNode, 1));
		}
		else
		{
			LogError(argNode, "Don't know how to parse \""_sv, argType, "\" node from attribute list"_sv);
			QUICK_DUMP_NODE(argNode);
			foundErrors = true;
		}
	}

	return foundErrors == false;
}

bool FHeaderFileParser::ParseBaseClassClauseForStruct(TSNode classSpecifier, FParsedStructInfo& structInfo) const
{
	// TODO Need to be able to support multiple base classes

	TSNode baseClassClause = GetChildNodeOfType(classSpecifier, "base_class_clause"_sv);
	if (IsValid(baseClassClause) == false)
	{
		//LogError(classSpecifier, "Failed to find base class clause");
		return false;
	}

	TSNode accessSpecifier = GetChildNodeOfType(baseClassClause, "access_specifier"_sv);
	if (IsValid(accessSpecifier) == false)
	{
		LogError(baseClassClause, "Failed to find base class access specifier"_sv);
		return false;
	}

	TSNode typeIdentifier = GetChildNodeOfType(baseClassClause, "type_identifier"_sv);
	if (IsValid(typeIdentifier) == false)
	{
		LogError(baseClassClause, "Failed to find base class type identifier"_sv);
		return false;
	}

	structInfo.BaseTypeInheritanceKind = StringToEnum<ETypeInheritanceKind>(GetNodeSourceText(accessSpecifier));
	structInfo.BaseTypeName = GetNodeSourceText(typeIdentifier);

	return true;
}

bool FHeaderFileParser::ParseFieldDeclarationListForClass(TSNode classSpecifier, FParsedClassInfo& classInfo) const
{
	// Get the field declaration list
	TSNode fieldDeclarationList = GetChildNodeOfType(classSpecifier, "field_declaration_list"_sv);
	if (IsValid(fieldDeclarationList) == false)
	{
		LogError(classSpecifier, "Failed to find field declaration list"_sv);
		return false;
	}

	// TODO Need to also support parsing functions

	return ParseFieldDeclarationListForStruct(fieldDeclarationList, classInfo);
}

// TODO This can be "ForClass" but just check if class info was passed in
bool FHeaderFileParser::ParseFieldDeclarationListForStruct(TSNode fieldDeclarationList, FParsedStructInfo& structInfo) const
{
	if (IsValidAndOfType(fieldDeclarationList, "field_declaration_list"_sv) == false)
	{
		LogError(fieldDeclarationList, "Given invalid field declaration list"_sv);
		return false;
	}

	const TArray<TSNode> fieldDeclarations = GetChildNodes(fieldDeclarationList);
	FReflectionHeaderInfo reflectionHeader;
	bool foundErrors = false;

	for (int32 idx = 0; idx < fieldDeclarations.Num(); ++idx)
	{
		reflectionHeader.Reset();

		FStringView preprocessorCondition;
		TSNode declarationNode = fieldDeclarations[idx];
		FStringView declarationType { ts_node_type(declarationNode) };
		TSNode fieldDeclarationNode {};

		// TODO This probably only works for a single field declaration inside of a preprocessor #if node
		//      as well as a single preprocessor #if node
		if (declarationType == "preproc_if"_sv)
		{
			const TSNode preprocessorIfNode = declarationNode;
			if (const TSNode identifierNode = GetChildNodeOfType(preprocessorIfNode, "identifier"_sv); IsValid(identifierNode))
			{
				preprocessorCondition = GetNodeSourceText(identifierNode);
			}

			declarationNode = GetChildNodeOfType(preprocessorIfNode, "declaration"_sv);
			if (IsValid(declarationNode) == false)
			{
				// TODO The declaration node could contain "function_definition" nodes
				// QUICK_DUMP_NODE(preprocessorIfNode);
				continue;
			}

			declarationType = FStringView { ts_node_type(declarationNode) };
			fieldDeclarationNode = GetChildNodeOfType(preprocessorIfNode, "field_declaration"_sv);
		}

		// UM_PROPERTY() will be parsed as a `declaration` node with a single `function_declarator` node
		if (declarationType == "declaration"_sv)
		{
			if (ParseReflectionHeaderFromDeclaration(declarationNode, reflectionHeader) == false)
			{
				continue;
			}
		}
		// UM_PROPERTY(...) will be parsed as a `field_declaration` node with a `type_identifier` node
		// for "UM_PROPERTY" and a `parenthesized_declarator` node for "(...)"
		/*else if (declarationType == "field_declaration"_sv)
		{
			if (ParseReflectionHeaderFromFieldDeclaration(declarationNode, reflectionHeader) == false)
			{
				continue;
			}
		}*/
		else
		{
			continue;
		}

		// Special early out for the generated body macro
		if (reflectionHeader.Type == EReflectionHeaderType::GeneratedBody)
		{
			structInfo.GeneratedBodyMacroLine = reflectionHeader.Range.start_point.row + 1;
			continue;
		}

		// Ensure we have a next node that this header can belong to
		const bool bAlreadyFoundFieldDeclaration = IsValid(fieldDeclarationNode);
		if (idx + 1 >= fieldDeclarations.Num() && bAlreadyFoundFieldDeclaration == false)
		{
			LogError(declarationNode, "Found erroneous \"{}\" header at the end of class \"{}\""_sv, reflectionHeader.Identifier, structInfo.TypeName);
			foundErrors = true;
			continue;
		}

		if (bAlreadyFoundFieldDeclaration == false)
		{
			// Move to the next node and attempt to parse it (ignore comment nodes)
			do
			{
				++idx;
				declarationNode = fieldDeclarations[idx];
				declarationType = FStringView { ts_node_type(declarationNode) };
			} while (declarationType == "comment"_sv);

			if (declarationType != "field_declaration"_sv)
			{
				LogError(declarationNode, "Found erroneous \"{}\" before \"{}\" node"_sv, reflectionHeader.Identifier, declarationType);
				foundErrors = true;
				continue;
			}

			fieldDeclarationNode = declarationNode;
		}

		if (reflectionHeader.Type == EReflectionHeaderType::Property)
		{
			FParsedPropertyInfo propertyInfo;
			if (ParsePropertyDeclaration(fieldDeclarationNode, propertyInfo) == false)
			{
				foundErrors = true;
				continue;
			}

			propertyInfo.Attributes = MoveTemp(reflectionHeader.Attributes);
			if (preprocessorCondition.IsEmpty() == false)
			{
				(void)propertyInfo.DefineConstraints.Emplace(preprocessorCondition);
			}
			structInfo.Properties.Add(MoveTemp(propertyInfo));
		}
		else if (reflectionHeader.Type == EReflectionHeaderType::Function)
		{
			LogError(fieldDeclarationNode, "Functions are not yet supported"_sv);
			QUICK_DUMP_NODE(fieldDeclarationNode);
			foundErrors = true;
		}
		else
		{
			LogError(declarationNode, "Don't know how to parse \"{}\" node from field declaration list"_sv, ts_node_type(declarationNode));
			foundErrors = true;
			continue;
		}
	}

	return foundErrors == false;
}

bool FHeaderFileParser::ParseOptionalParameterDeclarationAsAttribute(TSNode declaration, FParsedAttributeInfo& attribute) const
{
	if (IsValidAndOfType(declaration, "optional_parameter_declaration"_sv) == false)
	{
		return false;
	}

	const uint32 childNodeCount = ts_node_named_child_count(declaration);
	if (childNodeCount != 2)
	{
		LogError(declaration, "Expected name and value pair"_sv);
		QUICK_DUMP_NODE(declaration);
		return false;
	}

	// TODO Validate name and value types?
	attribute.Name = GetNodeSourceText(ts_node_named_child(declaration, 0));
	attribute.Value = GetNodeSourceText(ts_node_named_child(declaration, 1));

	return true;
}

bool FHeaderFileParser::ParseParameterListAsAttributeList(TSNode parameterList, TArray<FParsedAttributeInfo>& attributes) const
{
	if (IsValidAndOfType(parameterList, "parameter_list"_sv) == false)
	{
		return false;
	}

	bool foundErrors = false;
	const uint32 paramCount = ts_node_named_child_count(parameterList);
	for (uint32 idx = 0; idx < paramCount; ++idx)
	{
		TSNode paramNode = ts_node_named_child(parameterList, idx);
		const FStringView paramType { ts_node_type(paramNode) };

		if (paramType == "parameter_declaration"_sv)
		{
			const uint32 childCount = ts_node_named_child_count(paramNode);
			if (childCount != 1)
			{
				LogError(paramNode, "Expected 1 child node, but found "_sv, childCount);
				foundErrors = true;
				continue;
			}

			FParsedAttributeInfo& attribute = attributes.AddDefaultGetRef();
			attribute.Name = GetNodeSourceText(ts_node_named_child(paramNode, 0));
		}
		else if (paramType == "optional_parameter_declaration"_sv)
		{
			FParsedAttributeInfo attribute;
			if (ParseOptionalParameterDeclarationAsAttribute(paramNode, attribute) == false)
			{
				continue;
			}

			attributes.Add(attribute);
		}
		else
		{
			LogError(paramNode, "Don't know how to parse \"{}\" node from parameter list"_sv, paramType);
			foundErrors = true;
		}
	}

	return foundErrors == false;
}

bool FHeaderFileParser::ParseParenthesizedDeclaratorAsAttributeList(TSNode declarator, TArray<FParsedAttributeInfo>& attributes) const
{
	if (IsValidAndOfType(declarator, "parenthesized_declarator"_sv) == false)
	{
		return false;
	}

	(void)attributes;

	QUICK_DUMP_NODE(declarator);

	return false;
}

bool FHeaderFileParser::ParsePropertyDeclaration(TSNode declaration, FParsedPropertyInfo& propertyInfo) const
{
	/**
	 * For a property declaration like:
	 *     UM_PROPERTY(...)
	 *     FString m_Name;
	 * We will have a type_identifier node for "FString" and a field_identifier node for "m_Outer"
	 *
	 * For a property declaration like:
	 *     UM_PROPERTY(...)
	 *     int m_AttackLevel = 99;
	 * We will have a primitive_type node for "int" and a field_identifier node for "m_AttackLevel"
	 *
	 * For an illegal property declaration like:
	 *     UM_PROPERTY(...)
	 *     UObject* m_Outer = nullptr;
	 * We will have a type_identifier node for "UObject" and a pointer_declarator node for "* m_Outer"
	 *
	 * For an illegal property declaration like:
	 *     UM_PROPERTY(...)
	 *     static constexpr int32 DefaultMaximumHealth = 10;
	 * We will additionally have a storage_class_specifier node for "static" and a type_qualifier for "constexpr"
	 */

	// Raw pointer properties are not allowed
	if (TSNode pointerDeclarator = GetChildNodeOfType(declaration, "pointer_declarator"_sv);
	    IsValid(pointerDeclarator))
	{
		LogError(pointerDeclarator, "Raw pointer properties are not supported"_sv);
		return false;
	}

	// Storage specifiers are not allowed
	if (TSNode storageSpecifier = GetChildNodeOfType(declaration, "storage_class_specifier"_sv);
	    IsValid(storageSpecifier))
	{
		LogError(storageSpecifier, "Property storage specifier \"{}\" is not supported"_sv, GetNodeSourceText(storageSpecifier));
		return false;
	}

	// Type qualifiers are not allowed
	if (TSNode typeQualifier = GetChildNodeOfType(declaration, "type_qualifier"_sv);
		IsValid(typeQualifier))
	{
		LogError(typeQualifier, "Property type qualifier \"{}\" is not supported"_sv, GetNodeSourceText(typeQualifier));
		return false;
	}

	// Find the property's type
	if (TSNode typeIdentifier = GetChildNodeOfType(declaration, "type_identifier"_sv);
	    IsValid(typeIdentifier))
	{
		propertyInfo.PropertyType = GetNodeSourceText(typeIdentifier);
	}
	else if (TSNode primitiveType = GetChildNodeOfType(declaration, "primitive_type"_sv);
	         IsValid(primitiveType))
	{
		// TODO Print warning if using something like int, short, or long long
		//      since we have explicitly sized typedefs for those
		//      (will be easier to NOT emit a warning if it's like float, double, char16_t, etc)
		propertyInfo.PropertyType = GetNodeSourceText(primitiveType);
	}
	else if (TSNode templateType = GetChildNodeOfType(declaration, "template_type"_sv);
	         IsValid(templateType))
	{
		propertyInfo.PropertyType = GetNodeSourceText(templateType);
	}
	else
	{
		LogError(declaration, "Failed to parse property type"_sv);
		return false;
	}

	// Find the property's name
	if (TSNode fieldIdentifier = GetChildNodeOfType(declaration, "field_identifier"_sv);
	    IsValid(fieldIdentifier))
	{
		propertyInfo.PropertyName = GetNodeSourceText(fieldIdentifier);
	}
	else
	{
		LogError(declaration, "Failed to parse property name"_sv);
		return false;
	}

	return true;
}

bool FHeaderFileParser::ParseReflectionHeaderFromDeclaration(TSNode declaration, FReflectionHeaderInfo& info) const
{
	if (IsValidAndOfType(declaration, "declaration"_sv) == false)
	{
		return false;
	}

	PopulateRangeFromNode(declaration, info.Range);

	TSNode functionDeclaratorNode = GetChildNodeOfType(declaration, "function_declarator"_sv);
	if (IsValid(functionDeclaratorNode) == false)
	{
		return false;
	}

	// Kind of annoying that we need to do this here instead of when parsing a property declaration,
	// but tree-sitter will parse the following:
	//     UM_PROPERTY(...)
	//     volatile int32 m_Health;
	// by giving `volatile` to our function_declarator node instead of the property_declaration node.
	// We don't support any kind of type_qualifier node on properties (yet), so if we find one here
	// then that's an invalid property declaration

	if (TSNode typeQualifier = GetChildNodeOfType(functionDeclaratorNode, "type_qualifier"_sv);
	    IsValid(typeQualifier))
	{
		LogError(typeQualifier, "Property type qualifier \""_sv, GetNodeSourceText(typeQualifier),  "\" is not supported"_sv);
		return false;
	}

	const uint32 childNodeCount = ts_node_named_child_count(functionDeclaratorNode);
	if (childNodeCount != 2)
	{
		return false;
	}

	TSNode identifierNode = GetChildNodeOfType(functionDeclaratorNode, "identifier"_sv);
	if (IsValid(identifierNode) == false)
	{
		return false;
	}

	const FStringView identifierText = GetNodeSourceText(identifierNode);
	const EReflectionHeaderType identifierType = StringToEnum<EReflectionHeaderType>(identifierText);
	if (identifierType == EReflectionHeaderType::Invalid)
	{
		return false;
	}

	TSNode parameterListNode = GetChildNodeOfType(functionDeclaratorNode, "parameter_list"_sv);
	if (IsValid(parameterListNode) == false)
	{
		return false;
	}

	if (ParseParameterListAsAttributeList(parameterListNode, info.Attributes) == false)
	{
		return false;
	}

	info.Identifier = identifierText;
	info.Type = identifierType;

	return true;
}

bool FHeaderFileParser::ParseReflectionHeaderFromFieldDeclaration(TSNode fieldDeclaration, FReflectionHeaderInfo& info) const
{
	if (IsValidAndOfType(fieldDeclaration, "field_declaration"_sv) == false)
	{
		return false;
	}

	PopulateRangeFromNode(fieldDeclaration, info.Range);

	TSNode typeIdentifier = GetChildNodeOfType(fieldDeclaration, "type_identifier"_sv);
	if (IsValid(typeIdentifier) == false)
	{
		return false;
	}

	TSNode parenthesizedDeclarator = GetChildNodeOfType(fieldDeclaration, "parenthesized_declarator"_sv);
	if (IsValid(parenthesizedDeclarator) == false)
	{
		return false;
	}

	if (ParseParenthesizedDeclaratorAsAttributeList(parenthesizedDeclarator, info.Attributes) == false)
	{
		return false;
	}

	info.Identifier = GetNodeSourceText(typeIdentifier);
	info.Type = StringToEnum<EReflectionHeaderType>(info.Identifier);
	if (info.Type == EReflectionHeaderType::Invalid)
	{
		return false;
	}

	return false;
}

bool FHeaderFileParser::ParseReflectionHeaderFromExpressionStatement(TSNode expressionStatement, FReflectionHeaderInfo& info) const
{
	if (IsValid(expressionStatement) == false)
	{
		return false;
	}

	const FStringView nodeType { ts_node_type(expressionStatement) };
	if (nodeType != "expression_statement"_sv)
	{
		return false;
	}

	PopulateRangeFromNode(expressionStatement, info.Range);

	// It must also have a call_expression node as a child. Sometimes it can have comment nodes as children,
	// but we don't care about those (at least not yet)
	TSNode callExpressionNode = GetChildNodeOfType(expressionStatement, "call_expression"_sv);
	if (IsValid(callExpressionNode) == false)
	{
		return false;
	}

	const uint32 childNodeCount = ts_node_named_child_count(callExpressionNode);
	if (childNodeCount != 2)
	{
		return false;
	}

	// TODO Verify types are identifier and arg_list
	TSNode identifier = ts_node_named_child(callExpressionNode, 0);
	TSNode argumentList = ts_node_named_child(callExpressionNode, 1);

	if (ParseArgumentListAsAttributeList(argumentList, info.Attributes) == false)
	{
		return false;
	}

	info.Identifier = GetNodeSourceText(identifier);
	info.Type = StringToEnum<EReflectionHeaderType>(info.Identifier);

	return info.Type != EReflectionHeaderType::Invalid;
}

bool FHeaderFileParser::ValidateTranslationUnit()
{
	if (m_FoundClasses.IsEmpty() && m_FoundStructs.IsEmpty() && m_FoundEnums.IsEmpty())
	{
		return true;
	}

	bool foundErrors = false;

	const FString generatedHeaderName = FString::Format("{}.Generated.h"_sv, FPath::GetBaseFileName(m_FilePath));
	if (IncludesFile(generatedHeaderName) == false)
	{
		LogError({}, "Generated header \"{}\" must be included"_sv, generatedHeaderName);
		foundErrors = true;
	}

	for (const FParsedClassInfo& classInfo : m_FoundClasses)
	{
		if (classInfo.GeneratedBodyMacroLine == INVALID_LINE)
		{
			LogError({}, "Class \"{}\" is missing its {} macro"_sv, classInfo.TypeName, MacroNames::GeneratedBodySpecifier);
			foundErrors = true;
		}

		if (const FParsedAttributeInfo* childOfAttribute = classInfo.FindAttributeByName("ChildOf"_sv);
		    childOfAttribute != nullptr && childOfAttribute->Value.IsEmpty())
		{
			LogError({}, "Class \"{}\" has a ChildOf attribute, but the parent class is empty"_sv, classInfo.TypeName);
			foundErrors = true;
		}
	}

	for (const FParsedStructInfo& structInfo : m_FoundStructs)
	{
		if (structInfo.GeneratedBodyMacroLine == INVALID_LINE)
		{
			LogError({}, "Struct \"{}\" is missing its {} macro"_sv, structInfo.TypeName, MacroNames::GeneratedBodySpecifier);
			foundErrors = true;
		}
	}

	return foundErrors == false;
}

bool FHeaderFileParser::VisitClassSpecifier(FReflectionHeaderInfo& metaInfo, TSNode classSpecifier)
{
	FParsedClassInfo classInfo;
	classInfo.Attributes = MoveTemp(metaInfo.Attributes);

	// Get the class name
	if (TSNode typeIdentifier = GetChildNodeOfType(classSpecifier, "type_identifier"_sv);
	    IsValid(typeIdentifier))
	{
		classInfo.TypeName = GetNodeSourceText(typeIdentifier);
	}
	else
	{
		LogError(classSpecifier, "Missing class name"_sv);
		return false;
	}

	(void)ParseBaseClassClauseForStruct(classSpecifier, classInfo);

	// Get the field declaration list
	TSNode fieldDeclarationList = GetChildNodeOfType(classSpecifier, "field_declaration_list"_sv);
	if (IsValid(fieldDeclarationList) == false)
	{
		LogError(classSpecifier, "Failed to find field declaration list"_sv);
		return false;
	}

	// Get the declaration info
	if (ParseFieldDeclarationListForClass(classSpecifier, classInfo) == false)
	{
		return false;
	}

	m_FoundClasses.Add(MoveTemp(classInfo));

	return true;
}

bool FHeaderFileParser::VisitEnumSpecifier(FReflectionHeaderInfo& metaInfo, TSNode enumSpecifier)
{
	FParsedEnumInfo enumInfo;
	enumInfo.Attributes = MoveTemp(metaInfo.Attributes);

	// Get the enum's name
	const TArray<TSNode> typeIdentifiers = GetChildNodesOfType(enumSpecifier, "type_identifier"_sv);
	if (typeIdentifiers.Num() >= 1)
	{
		enumInfo.EnumName = GetNodeSourceText(typeIdentifiers[0]);
	}
	else
	{
		LogError(enumSpecifier, "Missing enum name from enum identifier"_sv);
		return false;
	}

	// Check for a custom base type that isn't just int
	if (typeIdentifiers.Num() >= 2)
	{
		enumInfo.UnderlyingType = GetNodeSourceText(typeIdentifiers[1]);
	}
	else if (TSNode typeSpecifier = GetChildNodeOfType(enumSpecifier, "sized_type_specifier"_sv);
	         IsValid(typeSpecifier))
	{
		enumInfo.UnderlyingType = GetNodeSourceText(typeSpecifier);
	}

	TSNode enumeratorList = GetChildNodeOfType(enumSpecifier, "enumerator_list"_sv);
	if (IsValid(enumeratorList) == false)
	{
		LogError(enumSpecifier, "Missing enumerator list"_sv);
		return false;
	}

	bool foundErrors = false;
	const TArray<TSNode> enumerators = GetChildNodesOfType(enumeratorList, "enumerator"_sv);
	for (TSNode enumerator : enumerators)
	{
		// There could be many child nodes of the enumerator, but we only care about the first
		// identifier node since that will be the name of the entry
		TSNode identifier = GetChildNodeOfType(enumerator, "identifier"_sv);
		if (IsValid(identifier) == false)
		{
			LogError(enumerator, "Missing identifier for entry name"_sv);
			foundErrors = true;
			continue;
		}

		const FStringView enumeratorName = GetNodeSourceText(identifier);
		enumInfo.EnumEntries.AddDefaultGetRef().EntryName = enumeratorName;
	}

	m_FoundEnums.Add(MoveTemp(enumInfo));

	return foundErrors == false;
}

bool FHeaderFileParser::VisitPreprocessorInclude(TSNode preprocessorInclude)
{
	if (TSNode stringLiteral = GetChildNodeOfType(preprocessorInclude, "string_literal"_sv);
	    IsValid(stringLiteral))
	{
		// Use the string content child node so we don't have quotes
		TSNode stringContent = GetChildNodeOfType(stringLiteral, "string_content"_sv);
		m_IncludedFiles.Add(GetNodeSourceText(stringContent));

		return true;
	}
	else if (TSNode systemLibString = GetChildNodeOfType(preprocessorInclude, "system_lib_string"_sv);
	         IsValid(systemLibString))
	{
		// TODO This has brackets surrounding it. Maybe trim those off?
		m_IncludedFiles.Add(GetNodeSourceText(systemLibString));

		return true;
	}

	LogError(preprocessorInclude, "Don't know how to parse preprocessor include \""_sv, GetNodeSourceText(preprocessorInclude), "\""_sv);

	return false;
}

bool FHeaderFileParser::VisitStructSpecifier(FReflectionHeaderInfo& metaInfo, TSNode structSpecifier)
{
	FParsedStructInfo structInfo;
	structInfo.Attributes = MoveTemp(metaInfo.Attributes);

	// Get the struct name
	if (TSNode typeIdentifier = GetChildNodeOfType(structSpecifier, "type_identifier"_sv);
	    IsValid(typeIdentifier))
	{
		structInfo.TypeName = GetNodeSourceText(typeIdentifier);
	}
	else
	{
		LogError(structSpecifier, "Missing struct name"_sv);
		return false;
	}

	(void)ParseBaseClassClauseForStruct(structSpecifier, structInfo);

	// Get the field declaration list
	TSNode fieldDeclarationList = GetChildNodeOfType(structSpecifier, "field_declaration_list"_sv);
	if (IsValid(fieldDeclarationList) == false)
	{
		LogError(structSpecifier, "Failed to find field declaration list"_sv);
		return false;
	}

	// Get the declaration info
	if (ParseFieldDeclarationListForStruct(fieldDeclarationList, structInfo) == false)
	{
		return false;
	}

	m_FoundStructs.Add(MoveTemp(structInfo));

	return true;
}

bool FHeaderFileParser::VisitTranslationUnit(TSNode translationUnit)
{
	bool foundErrors = false;

	const uint32 childCount = ts_node_named_child_count(translationUnit);
	for (uint32 idx = 0; idx < childCount; ++idx)
	{
		TSNode childNode = ts_node_named_child(translationUnit, idx);
		FStringView childType { ts_node_type(childNode) };

		if (childType == "preproc_include"_sv)
		{
			foundErrors |= VisitPreprocessorInclude(childNode) == false;
			continue;
		}

		// For now, we only care about expression statements because those are our meta macros
		// and, in a translation unit, will precede classes, structs, enums, and free functions
		if (childType != "expression_statement"_sv)
		{
			continue;
		}

		FReflectionHeaderInfo metaInfo;
		if (ParseReflectionHeaderFromExpressionStatement(childNode, metaInfo) == false)
		{
			continue;
		}

		// Valid meta macros cannot happen at the end of the file
		if (idx + 1 >= childCount)
		{
			LogError(childNode, "Found meta specifier \"{}\" at the end of translation unit"_sv, metaInfo.Identifier);
			foundErrors = true;
			continue;
		}

		++idx;
		childNode = ts_node_named_child(translationUnit, idx);
		childType = FStringView { ts_node_type(childNode) };

		// Verify the meta specifier and visit the definitions
		if (childType == "class_specifier"_sv || childType == "struct_specifier"_sv)
		{
			if (metaInfo.Type == EReflectionHeaderType::Class)
			{
				foundErrors |= VisitClassSpecifier(metaInfo, childNode) == false;
			}
			else if (metaInfo.Type == EReflectionHeaderType::Struct)
			{
				foundErrors |= VisitStructSpecifier(metaInfo, childNode) == false;
			}
			else
			{
				LogError(childNode, "Found invalid \""_sv, childType, "\" node after \""_sv, EnumToString(metaInfo.Type), "\" meta specifier"_sv);
				continue;
			}
		}
		else if (childType == "enum_specifier"_sv)
		{
			if (metaInfo.Type != EReflectionHeaderType::Enum)
			{
				LogError(childNode, "Found invalid \""_sv, childType, "\" node after \""_sv, MacroNames::EnumSpecifier, "\" meta specifier"_sv);
				continue;
			}

			foundErrors |= VisitEnumSpecifier(metaInfo, childNode) == false;
		}
		// TODO Support free functions?
		else
		{
			LogError(childNode, "Found meta specifier \""_sv, metaInfo.Identifier, "\" before node type \""_sv, childType, "\""_sv);
			foundErrors = true;
		}
	}

	foundErrors |= ValidateTranslationUnit() == false;
	return foundErrors == false;
}
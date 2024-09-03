#pragma once

#include "Containers/Stack.h"
#include "Containers/String.h"
#include "Engine/Logging.h"
#include "MetaTool/ClassInfo.h"
#include "MetaTool/EnumInfo.h"
#include <tree_sitter/api.h>

class FReflectionHeaderInfo; // TODO Rename to FParsedHeaderInfo ?

/**
 * @brief An enumeration of results for parsing a header file.
 */
enum class EHeaderFileParseResult
{
	/** @brief Successfully parsed a header file. */
	Success,
	/** @brief The generated file is already up-to-date. */
	GeneratedFileUpToDate,
	/** @brief Failed to read the header file. */
	CouldNotReadFile,
	/** @brief Encountered an error while parsing the header file. */
	ParseError,
	/** @brief Ran out of memory while parsing. */
	OutOfMemory
};

/**
 * @brief Defines a parser for C++ header files.
 */
class FHeaderFileParser final
{
	UM_DISABLE_COPY(FHeaderFileParser);
	UM_DISABLE_MOVE(FHeaderFileParser);

public:

	/**
	 * @brief Sets default values for this header file parser's properties.
	 */
	FHeaderFileParser() = default;

	/**
	 * @brief Destroys this header file parser.
	 */
	~FHeaderFileParser() = default;

	/**
	 * @brief Gets all of the files included by the parsed file.
	 *
	 * @return The files included by the parsed file.
	 */
	[[nodiscard]] TSpan<const FStringView> GetIncludedFiles() const;

	/**
	 * @brief Gets the collection of parsed classes.
	 *
	 * @return The collection of parsed classes.
	 */
	[[nodiscard]] TSpan<const FParsedClassInfo> GetFoundClasses() const;

	/**
	 * @brief Gets the collection of parsed enums.
	 *
	 * @return The collection of parsed enums.
	 */
	[[nodiscard]] TSpan<const FParsedEnumInfo> GetFoundEnums() const;

	/**
	 * @brief Gets the collection of parsed structs.
	 *
	 * @return The collection of parsed structs.
	 */
	[[nodiscard]] TSpan<const FParsedStructInfo> GetFoundStructs() const;

	/**
	 * @brief Checks to see if the parsed file includes the given file.
	 *
	 * @param filePath The file path to check.
	 * @return True if the files includes \p filePath, otherwise false.
	 */
	[[nodiscard]] bool IncludesFile(FStringView filePath) const;

	/**
	 * @brief Attempts to parse a header file for types to generate meta info for.
	 *
	 * @param filePath The path to the file being parsed.
	 * @param targetFileDirectory The directory that the generated files will live in.
	 * @return True if parsing was successful, otherwise false.
	 */
	[[nodiscard]] EHeaderFileParseResult ParseFile(FStringView filePath, FStringView targetFileDirectory);

private:

	/**
	 * @brief Gets the text from the source for a node.
	 *
	 * @param node The node.
	 * @return The text for the node.
	 */
	[[nodiscard]] FStringView GetNodeSourceText(TSNode node) const;

	/**
	 * @brief Logs an error.
	 *
	 * @tparam ArgTypes The types of the log arguments.
	 * @param context The node the error occurred at.
	 * @param args The log arguments.
	 */
	template<typename... ArgTypes>
	void LogError(TSNode context, FStringView message, ArgTypes&&... args) const
	{
		const TSPoint location = ts_node_start_point(context);

		if constexpr (sizeof...(args) == 0)
		{
			UM_LOG(Error, "{}({}:{}) {}", m_FilePath, location.row + 1, location.column + 1, message);
		}
		else
		{
			const FString formattedMessage = FString::Format(message, Forward<ArgTypes>(args)...);
			UM_LOG(Error, "{}({}:{}) {}", m_FilePath, location.row + 1, location.column + 1, formattedMessage);
		}
	}

	/**
	 * @brief Logs a message with a node as context.
	 *
	 * @tparam ArgTypes The types of the arguments to log.
	 * @param context The context node.
	 * @param args The log arguments.
	 */
	template<typename... ArgTypes>
	void LogMessage(TSNode context, FStringView message, ArgTypes&&... args) const
	{
	    const TSPoint location = ts_node_start_point(context);

		if constexpr (sizeof...(args) == 0)
		{
			UM_LOG(Info, "{}({}:{}) {}", m_FilePath, location.row + 1, location.column + 1, message);
		}
		else
		{
			const FString formattedMessage = FString::Format(message, Forward<ArgTypes>(args)...);
			UM_LOG(Info, "{}({}:{}) {}", m_FilePath, location.row + 1, location.column + 1, formattedMessage);
		}
	}

	/**
	 * @brief Parses an attribute list from an argument_list node.
	 *
	 * @param argumentList The argument_list node.
	 * @param attributes The attributes populate.
	 * @return True if the attribute list was parsed successfully, otherwise false.
	 */
	[[nodiscard]] bool ParseArgumentListAsAttributeList(TSNode argumentList, TArray<FParsedAttributeInfo>& attributes) const;

	/**
	 * @brief Parses a base_class_clause node for a class.
	 *
	 * @param classSpecifier The class specifier.
	 * @param structInfo The current class info.
	 * @return True if the base class clause was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseBaseClassClauseForStruct(TSNode classSpecifier, FParsedStructInfo& structInfo) const;

	/**
	 * @brief Parses a field_declaration_list node for a class.
	 *
	 * @param classSpecifier The class specifier.
	 * @param classInfo The current class info.
	 * @return True if the field declaration list was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseFieldDeclarationListForClass(TSNode classSpecifier, FParsedClassInfo& classInfo) const;

	/**
	 * @brief Parses a field_declaration_list node for a struct.
	 *
	 * @param fieldDeclarationList The field_declaration_list node.
	 * @param structInfo The current struct info.
	 * @return True if the field declaration list was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseFieldDeclarationListForStruct(TSNode fieldDeclarationList, FParsedStructInfo& structInfo) const;

	/**
	 * @brief Attempts to parse an attribute from an optional_parameter_declaration node.
	 *
	 * @param declaration The optional_parameter_declaration node.
	 * @param attribute The attribute to populate.
	 * @return True if the attribute was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseOptionalParameterDeclarationAsAttribute(TSNode declaration, FParsedAttributeInfo& attribute) const;

	/**
	 * @brief Parses an attribute list from a parameter_list node.
	 *
	 * @param argumentList The parameterList node.
	 * @param attributes The attributes populate.
	 * @return True if the attribute list was parsed successfully, otherwise false.
	 */
	[[nodiscard]] bool ParseParameterListAsAttributeList(TSNode parameterList, TArray<FParsedAttributeInfo>& attributes) const;

	/**
	 * @brief Parses an attribute list from a parenthesized_declarator node.
	 *
	 * @param declarator The parenthesized_declarator node.
	 * @param attributes The attributes populate.
	 * @return True if the attribute list was parsed successfully, otherwise false.
	 */
	[[nodiscard]] bool ParseParenthesizedDeclaratorAsAttributeList(TSNode declarator, TArray<FParsedAttributeInfo>& attributes) const;

	/**
	 * @brief Attempts to parse a property declaration.
	 *
	 * @param declaration The declaration node.
	 * @param propertyInfo The property info to populate.
	 * @return True if the property declaration was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParsePropertyDeclaration(TSNode declaration, FParsedPropertyInfo& propertyInfo) const;

	/**
	 * @brief Attempts to parse a reflection header from a declaration node.
	 *
	 * @param declaration The declaration node.
	 * @param info The reflection header to populate.
	 * @return True if the reflection header was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseReflectionHeaderFromDeclaration(TSNode declaration, FReflectionHeaderInfo& info) const;

	/**
	 * @brief Attempts to parse a reflection header from a field_declaration statement.
	 *
	 * @param fieldDeclaration The field_declaration node.
	 * @param info The reflection header to populate.
	 * @return True if the reflection header was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseReflectionHeaderFromFieldDeclaration(TSNode fieldDeclaration, FReflectionHeaderInfo& info) const;

	/**
	 * @brief Parses meta specifier information from an expression_statement node.
	 *
	 * @param node The expression statement.
	 * @param info The meta specifier info.
	 * @return True if the meta specifier info was parsed, otherwise false.
	 */
	[[nodiscard]] bool ParseReflectionHeaderFromExpressionStatement(TSNode expressionStatement, FReflectionHeaderInfo& info) const;

	/**
	 * @brief Attempts to validate the parsed translation unit. Is called by VisitTranslationUnit after parsing has completed.
	 */
	[[nodiscard]] bool ValidateTranslationUnit();

	/**
	 * @brief Visits a class_specifier node.
	 *
	 * @param metaInfo The parsed meta info for the class.
	 * @param classSpecifier The class specifier.
	 */
	[[nodiscard]] bool VisitClassSpecifier(FReflectionHeaderInfo& metaInfo, TSNode classSpecifier);

	/**
	 * @brief Visits an enum_specifier node.
	 *
	 * @param metaInfo The parsed meta info for the enum.
	 * @param enumSpecifier The enum specifier.
	 */
	[[nodiscard]] bool VisitEnumSpecifier(FReflectionHeaderInfo& metaInfo, TSNode enumSpecifier);

	/**
	 * @brief Visits a preproc_include node.
	 *
	 * @param preprocessorInclude A preproc_include node.
	 */
	[[nodiscard]] bool VisitPreprocessorInclude(TSNode preprocessorInclude);

	/**
	 * @brief Visits a struct_specifier node.
	 *
	 * @param metaInfo The parsed meta info for the struct.
	 * @param structSpecifier The struct specifier.
	 */
	[[nodiscard]] bool VisitStructSpecifier(FReflectionHeaderInfo& metaInfo, TSNode structSpecifier);

	/**
	 * @brief Visits a translation_unit node.
	 *
	 * @param translationUnit The translation unit.
	 */
	[[nodiscard]] bool VisitTranslationUnit(TSNode translationUnit);

	FString m_FileSource;
	FStringView m_FilePath;
	TArray<FString> m_ErrorMessages;
	TArray<FParsedClassInfo> m_FoundClasses;
	TArray<FParsedEnumInfo> m_FoundEnums;
	TArray<FParsedStructInfo> m_FoundStructs;
	TArray<FStringView> m_IncludedFiles;
	TStack<FString> m_DefineConstraints;
};
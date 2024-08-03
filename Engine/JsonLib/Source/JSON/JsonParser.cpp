#include "Engine/Logging.h"
#include "HAL/File.h"
#include "HAL/FileStream.h"
#include "JSON/JsonParser.h"
#include "Memory/UniquePtr.h"
#include "Misc/StringBuilder.h"
#include "Misc/StringParsing.h"
#include <tree_sitter/api.h>

#define EXPECT_NODE_TYPE(Node, ExpectedType)                                                            \
	TRY_DO([&]() -> TErrorOr<void> {                                                                    \
		const FStringView thisNodeType { ts_node_type(Node) };                                          \
		if (thisNodeType != UM_JOIN(ExpectedType, _sv))                                                 \
		{                                                                                               \
			return MAKE_ERROR("Expected node type `" ExpectedType "`, but found `{}`", thisNodeType);   \
		}                                                                                               \
		return {};                                                                                      \
	}())

extern "C" const TSLanguage* tree_sitter_json(void);

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

/**
 * @brief Defines a deleter type for unique pointers storing tree-sitter parsers.
 */
class FTreeSitterParserDeleter
{
public:

	void Delete(void* memory) const
	{
		ts_parser_delete(reinterpret_cast<TSParser*>(memory));
	}
};

/**
 * @brief Defines a deleter type for unique pointers storing tree-sitter parse trees.
 */
class FTreeSitterTreeDeleter
{
public:

	void Delete(void* memory) const
	{
		ts_tree_delete(reinterpret_cast<TSTree*>(memory));
	}
};

/**
 * @brief Defines a JSON parser.
 */
class FJsonParser
{
public:

	/**
	 * @brief Creates a JSON parser.
	 *
	 * @param text The text to parse.
	 * @return The JSON parser, or the error encountered while parsing.
	 */
	[[nodiscard]] static TErrorOr<FJsonParser> Create(FStringView text);

	/**
	 * @brief Parses the text that this JSON parser was created with.
	 *
	 * @return The parsed JSON value, or any error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FJsonValue> Parse();

private:

	/**
	 * @brief Gets the source text for a tree-sitter node.
	 *
	 * @param node The node.
	 * @return The source text.
	 */
	[[nodiscard]] FStringView GetNodeSourceText(TSNode node) const;

	/**
	 * @brief Parses a JSON array from an array node.
	 *
	 * @param node The array node.
	 * @return The parsed JSON array, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FJsonArray> ParseArray(TSNode node);

	/**
	 * @brief Parses a number from a string node.
	 *
	 * @param node The number node.
	 * @return The parsed number, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<double> ParseNumber(TSNode node);

	/**
	 * @brief Parses a JSON object from an object node.
	 *
	 * @param node The object node.
	 * @return The parsed JSON object, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FJsonObject> ParseObject(TSNode node);

	/**
	 * @brief Parses a JSON object key-value pair from a pair node.
	 *
	 * @param node The pair node.
	 * @return The parsed JSON object key-value pair, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FJsonObjectKeyValuePair> ParseObjectPair(TSNode node);

	/**
	 * @brief Parses a string from a string node.
	 *
	 * @param node The string node.
	 * @return The parsed string, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FString> ParseString(TSNode node);

	/**
	 * @brief Parses a JSON value from a node.
	 *
	 * @param node The node.
	 * @return The parsed JSON value, otherwise the error that was encountered.
	 */
	[[nodiscard]] TErrorOr<FJsonValue> ParseValue(TSNode node);

	FStringView m_Text;
	TUniquePtr<TSParser, FTreeSitterParserDeleter> m_Parser;
	TUniquePtr<TSTree, FTreeSitterTreeDeleter> m_ParseTree;
};

TErrorOr<FJsonParser> FJsonParser::Create(const FStringView text)
{
	ts_set_allocator(TreeSitter::Malloc, TreeSitter::Calloc, TreeSitter::Realloc, TreeSitter::Free);

	FJsonParser result;
	result.m_Text = text;

	result.m_Parser = decltype(result.m_Parser) { ts_parser_new() };
	if (result.m_Parser.IsNull())
	{
		return MAKE_ERROR("Failed to allocate tree-sitter parser");
	}

	ts_parser_set_language(result.m_Parser.Get(), tree_sitter_json());

	result.m_ParseTree = decltype(result.m_ParseTree)
	{
		ts_parser_parse_string(result.m_Parser.Get(), nullptr, text.GetChars(), static_cast<uint32>(text.Length()))
	};
	if (result.m_ParseTree.IsNull())
	{
		return MAKE_ERROR("Failed to parse JSON string");
	}

	return result;
}

TErrorOr<FJsonValue> FJsonParser::Parse()
{
	const TSNode rootNode = ts_tree_root_node(m_ParseTree.Get());
	const FStringView rootNodeType { ts_node_type(rootNode) };

	if (rootNodeType != "document"_sv)
	{
		// TODO Add MAKE_ERROR_FROM_NODE macro that will include the node's source location
		return MAKE_ERROR("Expected `document` root node, but found `{}`", rootNodeType);
	}

	const uint32 childNodeCount = ts_node_child_count(rootNode);
	if (childNodeCount != 1)
	{
		return MAKE_ERROR("Expected one document child node, but found {}", childNodeCount);
	}

	const TSNode documentTypeNode = ts_node_child(rootNode, 0);
	const FStringView documentType { ts_node_type(documentTypeNode) };
	if (documentType == "array"_sv)
	{
		TRY_EVAL(FJsonArray result, ParseArray(documentTypeNode));
		return FJsonValue::FromArray(result);
	}
	if (documentType == "object"_sv)
	{
		TRY_EVAL(FJsonObject result, ParseObject(documentTypeNode));
		return FJsonValue::FromObject(result);
	}

	return MAKE_ERROR("Expected `array` or `object` document node, but found `{}`", documentType);
}

FStringView FJsonParser::GetNodeSourceText(const TSNode node) const
{
	const uint32 startByte = ts_node_start_byte(node);
	const uint32 endByte = ts_node_end_byte(node);
	return m_Text.Substring(static_cast<int32>(startByte), static_cast<int32>(endByte - startByte));
}

TErrorOr<FJsonArray> FJsonParser::ParseArray(const TSNode node)
{
	FJsonArray result;

	const uint32 childNodeCount = ts_node_child_count(node);
	EXPECT_NODE_TYPE(ts_node_child(node, 0), "[");
	EXPECT_NODE_TYPE(ts_node_child(node, childNodeCount - 1), "]");

	for (uint32 idx = 1; idx < childNodeCount - 1; ++idx)
	{
		TSNode childNode = ts_node_child(node, idx);

		TRY_EVAL(FJsonValue childValue, ParseValue(childNode));
		result.Add(MoveTemp(childValue));

		if (idx + 1 >= childNodeCount)
		{
			continue;
		}

		// If the next node is a comma, then skip past it
		childNode = ts_node_child(node, idx + 1);
		const FStringView childType { ts_node_type(childNode) };
		if (childType == ","_sv)
		{
			++idx;
		}
	}

	return result;
}

TErrorOr<double> FJsonParser::ParseNumber(const TSNode node)
{
	TOptional<double> result = FStringParser::TryParseDouble(GetNodeSourceText(node));
	if (result.IsEmpty())
	{
		return MAKE_ERROR("Failed to parse number text `{}`", GetNodeSourceText(node));
	}

	return result.GetValue();
}

TErrorOr<FJsonObject> FJsonParser::ParseObject(const TSNode node)
{
	FJsonObject result;

	const uint32 childNodeCount = ts_node_child_count(node);
	EXPECT_NODE_TYPE(ts_node_child(node, 0), "{");
	EXPECT_NODE_TYPE(ts_node_child(node, childNodeCount - 1), "}");

	for (uint32 idx = 1; idx < childNodeCount - 1; ++idx)
	{
		TSNode childNode = ts_node_child(node, idx);
		FStringView childType { ts_node_type(childNode) };

		if (childType != "pair"_sv)
		{
			return MAKE_ERROR("Expected `pair` node in object, but found `{}`", GetNodeSourceText(childNode));
		}

		TRY_EVAL(FJsonObjectKeyValuePair childValue, ParseObjectPair(childNode));
		result.SetPair(MoveTemp(childValue));

		if (idx + 1 >= childNodeCount)
		{
			continue;
		}

		// If the next node is a comma, then skip past it
		childNode = ts_node_child(node, idx + 1);
		childType = FStringView { ts_node_type(childNode) };
		if (childType == ","_sv)
		{
			++idx;
		}
	}

	return result;
}

TErrorOr<FJsonObjectKeyValuePair> FJsonParser::ParseObjectPair(const TSNode node)
{
	const uint32 childNodeCount = ts_node_child_count(node);
	if (childNodeCount != 3)
	{
		return MAKE_ERROR("Expected three object pair child nodes, but found {}", childNodeCount);
	}

	const TSNode childNodes[3] { ts_node_child(node, 0), ts_node_child(node, 1), ts_node_child(node, 2) };
	EXPECT_NODE_TYPE(childNodes[0], "string");
	EXPECT_NODE_TYPE(childNodes[1], ":");

	FJsonObjectKeyValuePair result;
	TRY_EVAL(result.Key, ParseString(childNodes[0]));
	TRY_EVAL(result.Value, ParseValue(childNodes[2]));

	return result;
}

TErrorOr<FString> FJsonParser::ParseString(const TSNode node)
{
	const uint32 childNodeCount = ts_node_child_count(node);
	if (childNodeCount != 3)
	{
		return MAKE_ERROR("Expected 3 child nodes for string node, but found {}", childNodeCount);
	}

	const TSNode childNodes[3] { ts_node_child(node, 0), ts_node_child(node, 1), ts_node_child(node, 2) };
	EXPECT_NODE_TYPE(childNodes[0], "\"");
	EXPECT_NODE_TYPE(childNodes[1], "string_content");
	EXPECT_NODE_TYPE(childNodes[2], "\"");

	const FStringView nodeText = GetNodeSourceText(childNodes[1]);
	return FString { nodeText };
}

TErrorOr<FJsonValue> FJsonParser::ParseValue(const TSNode node)
{
	const FStringView nodeType { ts_node_type(node) };
	if (nodeType == "array"_sv)
	{
		TRY_EVAL(FJsonArray result, ParseArray(node));
		return FJsonValue::FromArray(result);
	}
	if (nodeType == "null"_sv)
	{
		return FJsonValue { EJsonValueType::Null };
	}
	if (nodeType == "number"_sv)
	{
		TRY_EVAL(const double result, ParseNumber(node));
		return FJsonValue::FromNumber(result);
	}
	if (nodeType == "object"_sv)
	{
		TRY_EVAL(FJsonObject result, ParseObject(node));
		return FJsonValue::FromObject(result);
	}
	if (nodeType == "string"_sv)
	{
		TRY_EVAL(FString result, ParseString(node));
		return FJsonValue::FromString(result);
	}
	if (nodeType == "ERROR"_sv)
	{
		return MAKE_ERROR("Expected JSON value, but found `{}`", GetNodeSourceText(node));
	}

	return MAKE_ERROR("Unknown node type `{}`", nodeType);
}

TErrorOr<FJsonValue> JSON::ParseFile(const FStringView filePath)
{
	TRY_EVAL(const FString text, FFile::ReadAllText(filePath));

	TErrorOr<FJsonValue> result = ::JSON::ParseString(text.AsStringView());
	if (result.IsError())
	{
		return MAKE_ERROR("Failed to parse file \"{}\" as JSON", filePath);
	}

	return result.ReleaseValue();
}

TErrorOr<FJsonValue> JSON::ParseString(const FStringView text)
{
	TRY_EVAL(FJsonParser parser, FJsonParser::Create(text));
	return parser.Parse();
}

TErrorOr<FJsonValue> JSON::ParseString(const FString& text)
{
	return ::JSON::ParseString(text.AsStringView());
}
#include "Engine/Logging.h"
#include "HAL/File.h"
#include "HAL/FileStream.h"
#include "JSON/JsonParser.h"
#include "JSON/JsonScanner.h"
#include "Memory/UniquePtr.h"
#include "Misc/StringBuilder.h"
#include "Misc/StringParsing.h"
#include "Parsing/Parser.h"

/**
 * @brief Defines a JSON token parser.
 */
class FJsonParser : public FParser
{
public:

	/**
	 * @brief Releases the parsed JSON value.
	 *
	 * @return The parsed JSON value.
	 */
	[[nodiscard]] FJsonValue ReleaseParsedValue()
	{
		return MoveTemp(m_ParsedValue);
	}

private:

	FJsonValue m_ParsedValue;
};

TErrorOr<FJsonValue> JSON::ParseFile(const FStringView filePath)
{
	TRY_EVAL(const FString text, FFile::ReadText(filePath));

	TErrorOr<FJsonValue> result = ::JSON::ParseString(text.AsStringView());
	if (result.IsError())
	{
		return MAKE_ERROR("Failed to parse file \"{}\" as JSON", filePath);
	}

	return result.ReleaseValue();
}

TErrorOr<FJsonValue> JSON::ParseString(const FStringView text)
{
	FJsonScanner scanner;
	scanner.SetLineCommentBegin("//"_sv);
	scanner.SetMultiLineComment("/*"_sv, "*/"_sv);

	scanner.ScanTextForTokens(text);

	if (scanner.HasErrors())
	{
		for (const FParseError& error : scanner.GetErrors())
		{
			UM_LOG(Error, "JSON scan error: {}", error);
		}
		return MAKE_ERROR("Encountered {} errors while scanning JSON text; see log for more details", scanner.GetErrors().Num());
	}

	FJsonParser parser;
	parser.ParseTokens(scanner.GetTokens());

	if (parser.HasErrors())
	{
		for (const FParseError& error : parser.GetErrors())
		{
			UM_LOG(Error, "JSON parse error: {}", error);
		}
		return MAKE_ERROR("Encountered {} errors while parsing JSON text; see log for more details", parser.GetErrors().Num());
	}

	return parser.ReleaseParsedValue();
}

TErrorOr<FJsonValue> JSON::ParseString(const FString& text)
{
	return ::JSON::ParseString(text.AsStringView());
}

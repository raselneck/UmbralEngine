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

protected:

	/** @inheritdoc FParser::OnParseBegin() */
	virtual bool OnParseBegin() override
	{
		// JSON documents must either have an array or an object as the root value
		if (Peek().Type != ETokenType::LeftBracket && Peek().Type != ETokenType::LeftBrace)
		{
			RecordError(Peek().Location, "Expected JSON array or object"_sv);
			return false;
		}

		return true;
	}

	/** @inheritdoc FParser::ParseFromCurrentToken() */
	virtual EIterationDecision ParseFromCurrentToken() override
	{
		TOptional<FJsonValue> rootValue = ParseJsonValue();
		if (rootValue.HasValue())
		{
			m_ParsedValue = rootValue.ReleaseValue();
		}

		// We can safely ignore anything after the root value
		// TODO Maybe record an error if we're not at the end after skipping some comments?
		return EIterationDecision::Break;
	}

	/**
	 * @brief Parses a JSON array.
	 *
	 * @return The parsed JSON array.
	 */
	[[nodiscard]] TOptional<FJsonArray> ParseJsonArray()
	{
		if (Consume(ETokenType::LeftBracket, "Expected '[' to start JSON array"_sv) == false)
		{
			return nullopt;
		}

		FJsonArray array;
		bool first = true;
		while (IsAtEnd() == false && Peek().Type != ETokenType::RightBracket)
		{
			if (first)
			{
				first = false;
			}
			else if (Consume(ETokenType::Comma, "Expected ',' between JSON array values"_sv) == false)
			{
				return nullopt;
			}

			TOptional<FJsonValue> value = ParseJsonValue();
			if (value.IsEmpty())
			{
				return nullopt;
			}

			array.Add(value.ReleaseValue());
		}

		if (Consume(ETokenType::RightBracket, "Expected ']' to end JSON array"_sv))
		{
			return array;
		}

		return nullopt;
	}

	/**
	 * @brief Parses a JSON number.
	 *
	 * @return The parsed number.
	 */
	[[nodiscard]] TOptional<FJsonValue> ParseJsonNumber()
	{
		const FSourceLocation numberLocation = Peek().Location;
		FStringBuilder numberString;

		// Ignore the plus
		if (Peek().Type == ETokenType::Plus)
		{
			AdvanceToken();
		}
		// Keep the negative sign
		if (Peek().Type == ETokenType::Minus)
		{
			numberString.Append(AdvanceToken().Text); // Negation
		}

		numberString.Append(AdvanceToken().Text); // Number

		// Check for a decimal
		bool decimal = false;
		if (Peek().Type == ETokenType::Period && PeekNext().Type == ETokenType::Number)
		{
			numberString.Append(AdvanceToken().Text); // Period
			numberString.Append(AdvanceToken().Text); // Number
			decimal = true;
		}

		if (decimal)
		{
			const TOptional<double> number = FStringParser::TryParseDouble(numberString.AsStringView());
			if (number.HasValue())
			{
				return FJsonValue::FromNumber(number.GetValue());
			}
			else
			{
				RecordError(numberLocation, "Failed to parse \"{}\" as a decimal"_sv, numberString.AsStringView());
				return nullopt;
			}
		}

		const TOptional<int64> number = FStringParser::TryParseInt64(numberString.AsStringView());
		if (number.HasValue())
		{
			return FJsonValue::FromNumber(static_cast<double>(number.GetValue()));
		}

		RecordError(numberLocation, "Failed to parse \"{}\" as an integer"_sv, numberString.AsStringView());
		return nullopt;
	}

	/**
	 * @brief Parses a JSON object.
	 *
	 * @return The parsed JSON object.
	 */
	[[nodiscard]] TOptional<FJsonObject> ParseJsonObject()
	{
		if (Consume(ETokenType::LeftBrace, "Expected '{' to start JSON object"_sv) == false)
		{
			return nullopt;
		}
	}

	/**
	 * @brief Parses a JSON value.
	 *
	 * @return The parsed JSON value.
	 */
	[[nodiscard]] TOptional<FJsonValue> ParseJsonValue()
	{
		switch (Peek().Type)
		{
		case ETokenType::LeftBracket:
		{
			TOptional<FJsonArray> array = ParseJsonArray();
			if (array.HasValue())
			{
				return FJsonValue::FromArray(array.ReleaseValue());
			}
			return nullopt;
		}

		case ETokenType::LeftBrace:
		{
			TOptional<FJsonObject> object = ParseJsonObject();
			if (object.HasValue())
			{
				return FJsonValue::FromObject(object.ReleaseValue());
			}
			return nullopt;
		}

		case ETokenType::Minus:
		case ETokenType::Plus: // Non-standard, but allow numbers like "+42.5"
			if (PeekNext().Type == ETokenType::Number)
			{
				return ParseJsonNumber();
			}
			break;

		case ETokenType::Number:
			return ParseJsonNumber();

		case ETokenType::String:
			return FJsonValue::FromString(AdvanceToken().Text);

		case ETokenType::Identifier:
			if (Peek().Text == "null"_sv)
			{
				AdvanceToken();
				return FJsonValue::Null;
			}
			if (Peek().Text == "true"_sv)
			{
				AdvanceToken();
				return FJsonValue::True;
			}
			if (Peek().Text == "false"_sv)
			{
				AdvanceToken();
				return FJsonValue::False;
			}
			break;

		default:
			break;
		}

		RecordError(Peek().Location, "Unexpected \"{}\""_sv, Peek().Text);

		return nullopt;
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

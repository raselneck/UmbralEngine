#include "Engine/Logging.h"
#include "JSON/JsonParser.h"
#include <gtest/gtest.h>

TEST(ParseTests, FromStringSimple)
{
	constexpr FStringView jsonArrayString = "[]"_sv;
	constexpr FStringView jsonObjectString = "{}"_sv;
	constexpr FStringView jsonInvalidString = "null"_sv;

	TErrorOr<FJsonValue> parseResult = JSON::ParseString(jsonArrayString);
	EXPECT_FALSE(parseResult.IsError());
	EXPECT_TRUE(parseResult.GetValue().IsArray());

	parseResult = JSON::ParseString(jsonObjectString);
	EXPECT_FALSE(parseResult.IsError());
	EXPECT_TRUE(parseResult.GetValue().IsObject());

	parseResult = JSON::ParseString(jsonInvalidString);
	EXPECT_TRUE(parseResult.IsError());
}

TEST(ParseTests, FromStringComplex)
{
	constexpr FStringView jsonString = "[\"string\", +42, -3.14, null, {\"key\": \"value\"}]"_sv;

	const TErrorOr<FJsonValue> parseResult = JSON::ParseString(jsonString);
	ASSERT_FALSE(parseResult.IsError());
	ASSERT_TRUE(parseResult.GetValue().IsArray());

	const FJsonArray* array = parseResult.GetValue().AsArray();
	ASSERT_EQ(array->Num(), 5);

	const FJsonValue& firstValue = array->At(0);
	EXPECT_TRUE(firstValue.IsString());
	EXPECT_EQ(firstValue.AsStringView(), "string"_sv);

	const FJsonValue& secondValue = array->At(1);
	EXPECT_TRUE(secondValue.IsNumber());
	EXPECT_DOUBLE_EQ(secondValue.AsNumber(), 42.0);

	const FJsonValue& thirdValue = array->At(2);
	EXPECT_TRUE(secondValue.IsNumber());
	EXPECT_DOUBLE_EQ(thirdValue.AsNumber(), -3.14);

	const FJsonValue& fourthValue = array->At(3);
	EXPECT_TRUE(fourthValue.IsNull());

	const FJsonValue& fifthValue = array->At(4);
	EXPECT_TRUE(fifthValue.IsObject());
	EXPECT_EQ(fifthValue.AsObject()->Num(), 1);
	EXPECT_TRUE(fifthValue.AsObject()->Contains("key"_sv));

	const FJsonValue* keyValue = fifthValue.AsObject()->Find("key"_sv);
	EXPECT_NE(keyValue, nullptr);
	EXPECT_TRUE(keyValue->IsString());
	EXPECT_EQ(keyValue->AsStringView(), "value"_sv);
}

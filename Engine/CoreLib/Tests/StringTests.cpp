#include "Containers/String.h"
#include "Engine/Logging.h"
#include <gtest/gtest.h>
#include <gtest/gtest-printers.h>

TEST(StringTests, AppendLongToEmpty)
{
	FString string;
	EXPECT_TRUE(string.IsEmpty());

	constexpr FStringView longString = "this is a long string to get around small buffer optimization"_sv;
	string.Append(longString);
	EXPECT_EQ(string, longString);
	EXPECT_EQ(string.Length(), longString.Length());
}

TEST(StringTests, AppendLongToLong)
{
	constexpr FStringView longString1 = "this is the first long string to use."_sv;
	FString string { longString1 };
	EXPECT_EQ(string, longString1);
	EXPECT_EQ(string.Length(), longString1.Length());

	constexpr FStringView longString2 = " this is the second long string to use."_sv;
	string.Append(longString2);
	EXPECT_TRUE(string.EndsWith(longString2));
	EXPECT_TRUE(string.StartsWith(longString1));
	EXPECT_EQ(string.Length(), longString1.Length() + longString2.Length());
}

TEST(StringTests, AppendLongToShort)
{
	FString string;
	EXPECT_TRUE(string.IsEmpty());

	constexpr FStringView shortString = "short"_sv;
	string.Append(shortString);
	EXPECT_EQ(string, shortString);
	EXPECT_EQ(string.Length(), shortString.Length());

	constexpr FStringView longString = " string. this is the long string to use."_sv;
	string.Append(longString);
	EXPECT_TRUE(string.EndsWith(longString));
	EXPECT_TRUE(string.StartsWith(shortString));
	EXPECT_EQ(string.Length(), shortString.Length() + longString.Length());
}

TEST(StringTests, AppendMultipleTimesToSameString)
{
	FString value;
	EXPECT_EQ(value.Length(), 0);

	value += "test"_sv;
	EXPECT_EQ(value.Length(), 4);
	EXPECT_EQ(value, "test"_sv);

	value += "test"_sv;
	EXPECT_EQ(value.Length(), 8);
	EXPECT_EQ(value, "testtest"_sv);

	value += "test"_sv;
	EXPECT_EQ(value.Length(), 12);
	EXPECT_EQ(value, "testtesttest"_sv);
}

TEST(StringTests, AppendShortToEmpty)
{
	FString string;
	EXPECT_TRUE(string.IsEmpty());

	constexpr FStringView shortString = "short"_sv;
	string.Append(shortString);
	EXPECT_EQ(string, shortString);
	EXPECT_EQ(string.Length(), shortString.Length());
}

TEST(StringTests, AppendShortToLong)
{
	constexpr FStringView longString = "this is a long string. the next string is "_sv;
	FString string { longString };
	EXPECT_EQ(string, longString);
	EXPECT_EQ(string.Length(), longString.Length());

	constexpr FStringView shortString = "short :)"_sv;
	string.Append(shortString);
	EXPECT_TRUE(string.EndsWith(shortString));
	EXPECT_TRUE(string.StartsWith(longString));
	EXPECT_EQ(string.Length(), longString.Length() + shortString.Length());
}

TEST(StringTests, AppendShortToShort)
{
	constexpr FStringView shortString1 = "short "_sv;
	constexpr FStringView shortString2 = "string"_sv;

	FString string;
	EXPECT_TRUE(string.IsEmpty());

	string.Append(shortString1);
	EXPECT_EQ(string, shortString1);
	EXPECT_EQ(string.Length(), shortString1.Length());

	string.Append(shortString2);
	EXPECT_TRUE(string.StartsWith(shortString1));
	EXPECT_TRUE(string.EndsWith(shortString2));
	EXPECT_EQ(string.Length(), shortString1.Length() + shortString2.Length());
}

TEST(StringTests, AsLower)
{
	const FString value = "Hello World"_s;
	EXPECT_EQ(value, "Hello World"_sv);
	EXPECT_EQ(value.AsLower(), "hello world"_sv);
}

TEST(StringTests, AsUpper)
{
	const FString value = "Hello World"_s;
	EXPECT_EQ(value, "Hello World"_sv);
	EXPECT_EQ(value.AsUpper(), "HELLO WORLD"_sv);
}

TEST(StringTests, Format)
{
	const FString value = FString::Format("{} \\{} hello {} :) {"_sv, 42, 3.14);
	EXPECT_EQ(value, "42 {} hello 3.14 :) {"_sv);
}

TEST(StringTests, FormatCStyle)
{
	const FString value = FString::FormatCStyle("%d %% hello %.02f :) {"_sv, 42, 3.14);
	EXPECT_EQ(value, "42 % hello 3.14 :) {"_sv);
}

TEST(StringTests, IndexOfChar_AtBeginning)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf('h'), 0);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, IndexOfChar_InMiddle)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf('l'), 2);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, IndexOfChar_AtEnd)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf('o'), 4);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, IndexOfString_AtBeginning)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf("he"_sv), 0);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, IndexOfString_InMiddle)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf("ll"_sv), 2);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, IndexOfString_AtEnd)
{
	const FString string = "hello"_s;

	EXPECT_EQ(string.IndexOf("lo"_sv), 3);
	EXPECT_EQ(string.IndexOf('a'), INDEX_NONE);
}

TEST(StringTests, OperatorPlusEmptyToLarge)
{
	constexpr FStringView longString = "this is a long string to get around small buffer allocation"_sv;
	FString string;
	string += longString;
	EXPECT_EQ(string, longString);
}

TEST(StringTests, OperatorPlusEmptyToSmall)
{
	FString string;
	string += "hello"_sv;
	EXPECT_EQ(string, "hello"_sv);
}

TEST(StringTests, OperatorPlusVarThenStringView)
{
	// NOTE Purposefully not const FString vars to ensure the strings aren't being modified

	FString string1 { "hello"_sv };
	EXPECT_EQ(string1, "hello"_sv);

	FString string2 = string1 + " world"_sv;
	EXPECT_EQ(string1, "hello"_sv);
	EXPECT_EQ(string2, "hello world"_sv);
}

TEST(StringTests, OperatorPlusInlineConstructThenStringView)
{
	constexpr FStringView expectedValue = "hello world this is a long string to get around small buffer optimization"_sv;
	constexpr FStringView baseString = expectedValue.Left(11);
	constexpr FStringView stringToAdd = expectedValue.Right(expectedValue.Length() - baseString.Length());

	const FString string = FString { baseString } + stringToAdd;
	EXPECT_EQ(string, expectedValue);
}

TEST(StringTests, OperatorPlusStringViewThenInlineConstruct)
{
	constexpr FStringView expectedValue = "hello world this is a long string to get around small buffer optimization"_sv;
	constexpr FStringView baseString = expectedValue.Left(11);
	constexpr FStringView stringToAdd = expectedValue.Right(expectedValue.Length() - baseString.Length());

	const FString string = baseString + FString { stringToAdd };
	EXPECT_EQ(string, expectedValue);
	EXPECT_EQ(string.Length(), expectedValue.Length());
}

TEST(StringTests, PrependLongToEmpty)
{
	FString string;
	EXPECT_TRUE(string.IsEmpty());

	constexpr FStringView longString = "this is a long string to get around small buffer optimization"_sv;
	string.Prepend(longString);
	EXPECT_EQ(string, longString);
	EXPECT_EQ(string.Length(), longString.Length());
}

TEST(StringTests, PrependLongToLong)
{
	constexpr FStringView longString1 = "this is the first long string to use."_sv;
	FString string { longString1 };
	EXPECT_EQ(string, longString1);
	EXPECT_EQ(string.Length(), longString1.Length());

	constexpr FStringView longString2 = "this is the second long string to use. "_sv;
	string.Prepend(longString2);
	EXPECT_TRUE(string.StartsWith(longString2));
	EXPECT_TRUE(string.EndsWith(longString1));
	EXPECT_EQ(string.Length(), longString1.Length() + longString2.Length());
}

TEST(StringTests, PrependLongToShort)
{
	constexpr FStringView shortString = "short string"_sv;
	static_assert(shortString.Length() <= Private::FShortStringData::MaxLength);

	FString string { shortString };
	EXPECT_EQ(string, shortString);
	EXPECT_EQ(string.Length(), shortString.Length());

	constexpr FStringView longString = "this is the long string to use. this is the "_sv;
	string.Prepend(longString);
	EXPECT_TRUE(string.StartsWith(longString));
	EXPECT_TRUE(string.EndsWith(shortString));
	EXPECT_EQ(string.Length(), shortString.Length() + longString.Length());
}

TEST(StringTests, PrependMultipleTimesToSameString)
{
	FString value;
	EXPECT_EQ(value.Length(), 0);

	value.Prepend("test!"_sv);
	EXPECT_EQ(value.Length(), 5);
	EXPECT_EQ(value, "test!"_sv);

	value.Prepend("test!"_sv);
	EXPECT_EQ(value.Length(), 10);
	EXPECT_EQ(value, "test!test!"_sv);

	value.Prepend("test!"_sv);
	EXPECT_EQ(value.Length(), 15);
	EXPECT_EQ(value, "test!test!test!"_sv);

	value.Prepend("test!"_sv);
	EXPECT_EQ(value.Length(), 20);
	EXPECT_EQ(value, "test!test!test!test!"_sv);
}

TEST(StringTests, PrependShortToEmpty)
{
	FString string;
	EXPECT_TRUE(string.IsEmpty());

	constexpr FStringView shortString = "short"_sv;
	string.Prepend(shortString);
	EXPECT_EQ(string, shortString);
	EXPECT_EQ(string.Length(), shortString.Length());
}

TEST(StringTests, PrependShortToLong)
{
	constexpr FStringView longString = " string first, then the long string :)"_sv;
	FString string { longString };
	EXPECT_EQ(string, longString);
	EXPECT_EQ(string.Length(), longString.Length());

	constexpr FStringView shortString = "short"_sv;
	string.Prepend(shortString);
	EXPECT_TRUE(string.StartsWith(shortString));
	EXPECT_TRUE(string.EndsWith(longString));
	EXPECT_EQ(string.Length(), longString.Length() + shortString.Length());
}

TEST(StringTests, PrependShortToShort)
{
	constexpr FStringView shortString1 = "short "_sv;
	constexpr FStringView shortString2 = "string"_sv;

	FString string;
	EXPECT_TRUE(string.IsEmpty());

	string.Prepend(shortString1);
	EXPECT_EQ(string, shortString1);
	EXPECT_EQ(string.Length(), shortString1.Length());

	string.Prepend(shortString2);
	EXPECT_TRUE(string.EndsWith(shortString1));
	EXPECT_TRUE(string.StartsWith(shortString2));
	EXPECT_EQ(string.Length(), shortString1.Length() + shortString2.Length());
}

TEST(StringTests, SplitByCharsIntoViews)
{
	const FString stringToSplit = "h1e2l3l1o"_s;
	const TArray<FStringView> splits = stringToSplit.SplitByCharsIntoViews("123"_sv);

	EXPECT_EQ(splits.Num(), 5);
	EXPECT_EQ(splits[0], "h"_sv);
	EXPECT_EQ(splits[1], "e"_sv);
	EXPECT_EQ(splits[2], "l"_sv);
	EXPECT_EQ(splits[3], "l"_sv);
	EXPECT_EQ(splits[4], "o"_sv);
}

TEST(StringTests, SplitByStringIntoViews)
{
#if 0
	const FString stringToSplit = "HtestEtestLtestLtestO"_s;
	const TArray<FStringView> splits = stringToSplit.SplitByStringIntoViews("test"_sv);

	EXPECT_EQ(splits.Num(), 5);
	EXPECT_EQ(splits[0], "H"_sv);
	EXPECT_EQ(splits[1], "E"_sv);
	EXPECT_EQ(splits[2], "L"_sv);
	EXPECT_EQ(splits[3], "L"_sv);
	EXPECT_EQ(splits[4], "O"_sv);
#endif
}
#include "Regex/Regex.h"
#include <gtest/gtest.h>

TEST(RegexTests, Match)
{
	EXPECT_EQ(FRegex::Match("\\d+"_s, "hello 123 world"_s), "123"_sv);
	EXPECT_EQ(FRegex::Match("[A-Z]+"_s, "hello 123 world"_s), ""_sv);
}

TEST(RegexTests, MatchAll)
{
	const FString text = "hello 123 world 456 how9are 01010you doi62019ng"_s;
	TArray<FStringView> matches = FRegex::MatchAll("\\d+"_s, text);

	EXPECT_EQ(matches.Num(), 5);
	EXPECT_EQ(matches[0], "123"_sv);
	EXPECT_EQ(matches[1], "456"_sv);
	EXPECT_EQ(matches[2], "9"_sv);
	EXPECT_EQ(matches[3], "01010"_sv);
	EXPECT_EQ(matches[4], "62019"_sv);
}

TEST(RegexTests, Matches)
{
	EXPECT_TRUE(FRegex::Matches("\\d+"_s, "hello 123 world"_s));
	EXPECT_FALSE(FRegex::Matches("[A-Z]+"_s, "hello 123 world"_s));
}
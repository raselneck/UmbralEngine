#include "Parsing/Scanner.h"
#include <gtest/gtest.h>

TEST(ScannerTests, SingleLineComment)
{
	const FStringView text = "this is a //single line comment"_sv;

	FScanner scanner;
	scanner.SetLineCommentBegin("//"_sv);
	scanner.ScanTextForTokens(text);

	EXPECT_FALSE(scanner.HasErrors());
	ASSERT_EQ(scanner.GetTokens().Num(), 4);
	EXPECT_EQ(scanner.GetTokens()[0].Text, "this"_sv);
	EXPECT_EQ(scanner.GetTokens()[1].Text, "is"_sv);
	EXPECT_EQ(scanner.GetTokens()[2].Text, "a"_sv);
	EXPECT_EQ(scanner.GetTokens()[3].Text, "single line comment"_sv);
}

TEST(ScannerTests, MultiLineComment)
{
	const FStringView text = "this is a /*multi\nline\rcomment\r\n:)*/ hello world"_sv;

	FScanner scanner;
	scanner.SetMultiLineComment("/*"_sv, "*/"_sv);
	scanner.ScanTextForTokens(text);

	EXPECT_FALSE(scanner.HasErrors());
	ASSERT_EQ(scanner.GetTokens().Num(), 6);
	EXPECT_EQ(scanner.GetTokens()[0].Text, "this"_sv);
	EXPECT_EQ(scanner.GetTokens()[1].Text, "is"_sv);
	EXPECT_EQ(scanner.GetTokens()[2].Text, "a"_sv);
	EXPECT_EQ(scanner.GetTokens()[3].Text, "multi\nline\rcomment\r\n:)"_sv);
	EXPECT_EQ(scanner.GetTokens()[4].Text, "hello"_sv);
	EXPECT_EQ(scanner.GetTokens()[5].Text, "world"_sv);
}

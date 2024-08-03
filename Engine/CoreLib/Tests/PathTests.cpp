#include "HAL/Path.h"
#include <gtest/gtest.h>

TEST(PathTests, GetBaseFileName)
{
	EXPECT_EQ(FPath::GetBaseFileName("/foo/bar.dir/thing"_sv), "thing"_sv);
	EXPECT_EQ(FPath::GetBaseFileName("MyFile.txt.bak"_sv), "MyFile.txt"_sv);
}

TEST(PathTests, GetDirectoryName)
{
	EXPECT_EQ(FPath::GetDirectoryName("foo.txt"_sv), ""_sv);
	EXPECT_EQ(FPath::GetDirectoryName("C:\\MyFiles\\foo.txt"_sv), "C:\\MyFiles"_sv);
	EXPECT_EQ(FPath::GetDirectoryName(".\\foo.txt"_sv), "."_sv);
	EXPECT_EQ(FPath::GetDirectoryName("MyFiles\\foo.txt"_sv), "MyFiles"_sv);
	EXPECT_EQ(FPath::GetDirectoryName("/foo/bar.dir/thing"_sv), "/foo/bar.dir"_sv);
	EXPECT_EQ(FPath::GetDirectoryName("MyFile.txt.bak"_sv), ""_sv);
}

TEST(PathTests, GetExtension)
{
	EXPECT_EQ(FPath::GetExtension("/foo/bar.dir/thing"_sv), ""_sv);
	EXPECT_EQ(FPath::GetExtension("MyFile.txt.bak"_sv), "bak"_sv);
}
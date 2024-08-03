#include "Containers/StaticArray.h"
#include <gtest/gtest.h>

TEST(StaticArrayTests, BeginAndEnd)
{
	const TStaticArray<int32, 3> array {{ 1, 2, 3 }};

	auto iter = array.begin();
	EXPECT_EQ(*iter, 1);
	++iter;
	EXPECT_EQ(*iter, 2);
	++iter;
	EXPECT_EQ(*iter, 3);
	EXPECT_NE(iter, array.end());
	++iter;
	EXPECT_EQ(iter, array.end());
}
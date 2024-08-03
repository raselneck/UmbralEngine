#include "Containers/Function.h"
#include <gtest/gtest.h>

TEST(FunctionTests, MoveConstruct)
{
	const uintptr testAddress = reinterpret_cast<uintptr>(this);
	TFunction<uintptr()> func1 = [&testAddress]() { return testAddress; };

	EXPECT_TRUE(func1.IsValid());
	EXPECT_EQ(func1(), testAddress);

	TFunction<uintptr()> func2 = MoveTemp(func1);

	EXPECT_FALSE(func1.IsValid());
	EXPECT_TRUE(func2.IsValid());
	EXPECT_EQ(func2(), testAddress);
}
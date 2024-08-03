#include "Misc/AtExit.h"
#include "Misc/DoOnce.h"
#include <gtest/gtest.h>

TEST(MiscTests, AtExit)
{
	int32 value = INDEX_NONE;
	EXPECT_EQ(value, INDEX_NONE);

	{
		value = 999;

		ON_EXIT_SCOPE()
		{
			value = 42;
		};
	}

	EXPECT_EQ(value, 42);
}

TEST(MiscTests, DoOnce_NoResult)
{
	int32 value = INDEX_NONE;
	EXPECT_EQ(value, INDEX_NONE);

	const auto TryModifyValue = [&]
	{
		DO_ONCE([&] { value = 42; });
	};

	TryModifyValue();
	EXPECT_EQ(value, 42);

	value = 999;
	TryModifyValue();
	EXPECT_EQ(value, 999);
}

TEST(MiscTests, DoOnce_Result)
{
	int32 counter = 0;
	EXPECT_EQ(counter, 0);

	const auto TryIncrementCounter = [&]
	{
		const int32 result = DO_ONCE([&] { return ++counter; });
		EXPECT_EQ(result, counter);
		EXPECT_EQ(result, 1);
	};

	TryIncrementCounter();
	TryIncrementCounter();
}
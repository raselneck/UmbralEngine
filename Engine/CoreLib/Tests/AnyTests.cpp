#include "LargeTypes.h"
#include "Containers/Any.h"
#include <gtest/gtest.h>

TEST(AnyTests, DefaultConstruct)
{
	FAny value;

	EXPECT_FALSE(value.HasValue());
	EXPECT_EQ(value.GetValue<int>(), nullptr);
}

TEST(AnyTests, CopyConstruct)
{
	const FAny firstValue { 42 };
	const FAny secondValue = firstValue;

	EXPECT_TRUE(firstValue.HasValue());
	EXPECT_EQ(firstValue.HasValue(), secondValue.HasValue());
	EXPECT_EQ(*firstValue.GetValue<int32>(), *secondValue.GetValue<int32>());
	EXPECT_NE(firstValue.GetValue<int32>(), secondValue.GetValue<int32>());
}

TEST(AnyTests, MoveConstruct)
{
	FAny firstValue { 42 };
	const FAny secondValue = MoveTemp(firstValue);

	EXPECT_FALSE(firstValue.HasValue());
	EXPECT_TRUE(secondValue.HasValue());
	EXPECT_FALSE(firstValue.Is<int32>());
	EXPECT_TRUE(secondValue.Is<int32>());
	EXPECT_EQ(firstValue.GetValue<int32>(), nullptr);
	EXPECT_NE(secondValue.GetValue<int32>(), nullptr);
	EXPECT_EQ(*secondValue.GetValue<int32>(), 42);
}

TEST(AnyTests, FromInt)
{
	const FAny value { 42 };

	EXPECT_TRUE(value.HasValue());
	EXPECT_TRUE(value.Is<int32>());
	EXPECT_NE(value.GetValue<int32>(), nullptr);
	EXPECT_EQ(*value.GetValue<int32>(), 42);
}

TEST(AnyTests, FromLargeValue)
{
	FLargeType::NumDestroyed = 0;
	FLargerType::NumDestroyed = 0;
	FLargestType::NumDestroyed = 0;

	FAny value;

	value.SetValue<FLargestType>({});
	EXPECT_TRUE(value.HasValue());
	EXPECT_TRUE(value.Is<FLargestType>());
	EXPECT_TRUE(value.Is<const FLargestType>());
	EXPECT_FALSE(value.Is<double>());
	EXPECT_FALSE(value.Is<const double>());
	EXPECT_NE(value.GetValue<const FLargestType>(), nullptr);
	EXPECT_NE(value.GetValue<FLargestType>(), nullptr);
	EXPECT_EQ(value.GetValue<double>(), nullptr);
	const void* largestValueAddress = value.GetValue<FLargestType>();

	value.SetValue<FLargeType>({});
	EXPECT_TRUE(value.HasValue());
	EXPECT_TRUE(value.Is<FLargeType>());
	EXPECT_TRUE(value.Is<const FLargeType>());
	EXPECT_NE(value.GetValue<FLargeType>(), nullptr);
	const void* largeValueAddress = value.GetValue<FLargeType>();

	EXPECT_EQ(largestValueAddress, largeValueAddress);

	value.Reset();

	EXPECT_FALSE(value.HasValue());
	EXPECT_EQ(value.GetValue<FLargestType>(), nullptr);
	EXPECT_EQ(value.GetValue<FLargerType>(), nullptr);
	EXPECT_EQ(value.GetValue<FLargeType>(), nullptr);

	EXPECT_GT(FLargeType::NumDestroyed, 0);
	EXPECT_EQ(FLargerType::NumDestroyed, 0);
	EXPECT_GT(FLargestType::NumDestroyed, 0);
}
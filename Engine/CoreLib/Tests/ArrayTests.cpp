#include "Containers/Array.h"
#include <gtest/gtest.h>

struct FArrayTestHelper
{
	bool bDefaultConstructed = false;
	bool bCopyConstructed = false;
	bool bMoveConstructedTo = false;
	bool bMoveConstructedFrom = false;
	bool bMoveAssignedTo = false;
	bool bMoveAssignedFrom = false;

	FArrayTestHelper()
		: bDefaultConstructed { true }
	{
	}

	FArrayTestHelper(const FArrayTestHelper&)
		: bCopyConstructed { true }
	{
	}

	FArrayTestHelper(FArrayTestHelper&& Other)
		: bMoveConstructedTo { true }
	{
		Other.bMoveConstructedFrom = true;
	}

	FArrayTestHelper& operator=(const FArrayTestHelper&) = delete;

	FArrayTestHelper& operator=(FArrayTestHelper&& Other)
	{
		bMoveAssignedTo = true;
		Other.bMoveAssignedFrom = true;
		return *this;
	}
};

// TODO Need to do a name style pass on this file

TEST(ArrayTests, DefaultConstruct)
{
	const TArray<int32> Array;
	EXPECT_EQ(Array.GetCapacity(), 0);
	EXPECT_EQ(Array.GetData(), nullptr);
	EXPECT_EQ(Array.Num(), 0);
}

TEST(ArrayTests, InitializerListConstruct)
{
	const TArray<int32> Array {{ 1, 2, 3, 4 }};
	EXPECT_GT(Array.GetCapacity(), 0);
	EXPECT_NE(Array.GetData(), nullptr);
	EXPECT_EQ(Array.Num(), 4);
}

TEST(ArrayTests, CopyConstruct)
{
	const TArray<int32> FirstArray {{ 1, 2, 3, 4 }};
	const TArray<int32> SecondArray = FirstArray;

	EXPECT_GT(FirstArray.GetCapacity(), 0);
	EXPECT_NE(FirstArray.GetData(), nullptr);
	EXPECT_EQ(FirstArray.Num(), 4);

	EXPECT_GT(SecondArray.GetCapacity(), 0);
	EXPECT_NE(SecondArray.GetData(), nullptr);
	EXPECT_EQ(SecondArray.Num(), 4);

	EXPECT_EQ(FirstArray.Num(), SecondArray.Num());
	EXPECT_NE(FirstArray.GetData(), SecondArray.GetData());

	for (int32 idx = 0; idx < FirstArray.Num(); ++idx)
	{
		constexpr auto AreValuesEqual = [](auto first, auto second, [[maybe_unused]] auto index) { return first == second; };
		EXPECT_PRED3(AreValuesEqual, FirstArray[idx], SecondArray[idx], idx);
	}
}

TEST(ArrayTests, MoveConstruct)
{
	TArray<int32> FirstArray {{ 1, 2, 3, 4 }};
	const TArray<int32> SecondArray = MoveTemp(FirstArray);

	EXPECT_EQ(FirstArray.GetCapacity(), 0);
	EXPECT_EQ(FirstArray.GetData(), nullptr);
	EXPECT_EQ(FirstArray.Num(), 0);

	EXPECT_GT(SecondArray.GetCapacity(), 0);
	EXPECT_NE(SecondArray.GetData(), nullptr);
	EXPECT_EQ(SecondArray.Num(), 4);
}

TEST(ArrayTests, AddCopy)
{
	FArrayTestHelper ValueToCopy;

	TArray<FArrayTestHelper> HelperArray;
	HelperArray.Add(static_cast<const FArrayTestHelper&>(ValueToCopy));

	EXPECT_GT(HelperArray.GetCapacity(), 0);
	EXPECT_NE(HelperArray.GetData(), nullptr);
	EXPECT_EQ(HelperArray.Num(), 1);
	EXPECT_TRUE(HelperArray[0].bCopyConstructed);
}

TEST(ArrayTests, AddMove)
{
	FArrayTestHelper ValueToMove;

	TArray<FArrayTestHelper> HelperArray;
	HelperArray.Add(MoveTemp(ValueToMove));

	EXPECT_GT(HelperArray.GetCapacity(), 0);
	EXPECT_NE(HelperArray.GetData(), nullptr);
	EXPECT_EQ(HelperArray.Num(), 1);
	EXPECT_TRUE(HelperArray[0].bMoveConstructedTo);
	EXPECT_TRUE(ValueToMove.bMoveConstructedFrom);
}

// TODO Need a test for AddDefault but with at least 5 items being added
TEST(ArrayTests, AddDefault)
{
	constexpr int32 NumToAdd = 1;

	TArray<FArrayTestHelper> HelperArray;
	const int32 ValueIndex = HelperArray.AddDefault(NumToAdd);

	EXPECT_EQ(ValueIndex, 0);
	EXPECT_EQ(HelperArray.Num(), NumToAdd);
	EXPECT_GT(HelperArray.GetCapacity(), 0);
	EXPECT_TRUE(HelperArray[0].bDefaultConstructed);
}

TEST(ArrayTests, AddDefaultGetRef)
{
	TArray<FArrayTestHelper> HelperArray;
	FArrayTestHelper& AddedValue = HelperArray.AddDefaultGetRef();

	EXPECT_GT(HelperArray.GetCapacity(), 0);
	EXPECT_NE(HelperArray.GetData(), nullptr);
	EXPECT_EQ(HelperArray.Num(), 1);
	EXPECT_TRUE(AddedValue.bDefaultConstructed);
}

TEST(ArrayTests, AppendRawArray)
{
	const int32 RawArray[] { 1, 2, 3, 4 };
	constexpr int32 NumRawArrayElements = sizeof(RawArray) / sizeof(RawArray[0]);

	TArray<int32> Array;
	Array.Append(RawArray, NumRawArrayElements);

	EXPECT_GT(Array.GetCapacity(), 0);
	EXPECT_NE(Array.GetData(), nullptr);
	EXPECT_EQ(Array.Num(), NumRawArrayElements);
	for (int32 Idx = 0; Idx < NumRawArrayElements; ++Idx)
	{
		EXPECT_EQ(Array[Idx], RawArray[Idx]);
	}
}

TEST(ArrayTests, AppendArray)
{
	const TArray<int32> FirstArray {{ 1, 2, 3, 4, 5 }};

	TArray<int32> SecondArray;
	SecondArray.Append(FirstArray.AsSpan());

	EXPECT_GT(FirstArray.GetCapacity(), 0);
	EXPECT_GT(SecondArray.GetCapacity(), 0);
	EXPECT_NE(FirstArray.GetData(), SecondArray.GetData());
	EXPECT_EQ(FirstArray.Num(), SecondArray.Num());
	for (int32 Idx = 0; Idx < FirstArray.Num(); ++Idx)
	{
		EXPECT_EQ(FirstArray[Idx], SecondArray[Idx]);
	}
}

// TODO Insert_AtBeginning

// TODO Insert_AtEnd

// TODO Insert_Copy

TEST(ArrayTests, Insert_InMiddle)
{
	const TArray<int32> result {{ 1, 2, 3, 4, 5 }};

	TArray<int32> value {{ 1, 2, 4, 5 }};
	value.Insert(2, 3);

	EXPECT_EQ(result.Num(), value.Num());

	for (int32 idx = 0; idx < result.Num(); ++idx)
	{
		EXPECT_EQ(result[idx], value[idx]);
	}
}

// TODO Insert_Move

TEST(ArrayTests, InsertDefault_FromEmpty)
{
	constexpr int32 numElements = 10;

	TArray<FArrayTestHelper> values;
	values.InsertDefault(0, numElements);

	EXPECT_EQ(values.Num(), numElements);

	for (const FArrayTestHelper& value : values)
	{
		EXPECT_TRUE(value.bDefaultConstructed);
	}
}

TEST(ArrayTests, InsertUninitialized_FromNonEmpty)
{
	const TArray<int32> result {{ 1, 2, 0, 0, 4, 5 }};

	TArray<int32> value {{ 1, 2, 4, 5 }};
	value.InsertUninitialized(2, 2);

	EXPECT_EQ(result.Num(), value.Num());

	for (int32 idx = 0; idx < result.Num(); ++idx)
	{
		EXPECT_EQ(result[idx], value[idx]);
	}
}

TEST(ArrayTests, InsertUninitialized_FromEmpty)
{
	constexpr int32 numElements = 10;

	TArray<int32> values;
	values.InsertUninitialized(0, numElements);

	EXPECT_EQ(values.Num(), numElements);

	// "Uninitialized" values must be zeroed out
	for (int32 value : values)
	{
		EXPECT_EQ(value, 0);
	}
}

TEST(ArrayTests, Iterate)
{
	const TArray<int32> values {{ 1, 2, 3, 4 }};

	constexpr int32 expectedSum = 1 + 2 + 3 + 4;
	int32 sum = 0;
	values.Iterate([&sum](const int32 value)
	{
		sum += value;
		return EIterationDecision::Continue;
	});

	EXPECT_EQ(sum, expectedSum);

	sum = 0;
	values.Iterate([&sum](const int32 value)
	{
		if (value > 2)
		{
			return EIterationDecision::Break;
		}

		sum += value;
		return EIterationDecision::Continue;
	});

	EXPECT_EQ(sum, 3);
}

TEST(ArrayTests, BeginAndEnd)
{
	const TArray<int32> array {{ 1, 2, 3 }};

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

TEST(ArrayTests, SortCustom)
{
	TArray<int32> numbers = {{ 1, 1, 2, 3, 4, 4 }};
	numbers.Sort([](const int32 first, const int32 second)
	{
		if (first < second)
		{
			return ECompareResult::GreaterThan;
		}
		if (first > second)
		{
			return ECompareResult::LessThan;
		}
		return ECompareResult::Equals;
	});

	EXPECT_EQ(numbers[0], 4);
	EXPECT_EQ(numbers[1], 4);
	EXPECT_EQ(numbers[2], 3);
	EXPECT_EQ(numbers[3], 2);
	EXPECT_EQ(numbers[4], 1);
	EXPECT_EQ(numbers[5], 1);
}

TEST(ArrayTests, SortDefault)
{
	TArray<int32> numbers = {{ 9, 1, 8, 2, 7, 3, 6, 4, 5 }};
	numbers.Sort();

	EXPECT_EQ(numbers[0], 1);
	EXPECT_EQ(numbers[1], 2);
	EXPECT_EQ(numbers[2], 3);
	EXPECT_EQ(numbers[3], 4);
	EXPECT_EQ(numbers[4], 5);
	EXPECT_EQ(numbers[5], 6);
	EXPECT_EQ(numbers[6], 7);
	EXPECT_EQ(numbers[7], 8);
	EXPECT_EQ(numbers[8], 9);
}
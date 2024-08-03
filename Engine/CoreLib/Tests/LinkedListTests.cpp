#include "Containers/Array.h"
#include "Containers/LinkedList.h"
#include <gtest/gtest.h>

TEST(LinkedListTests, DefaultConstruct)
{
	const TLinkedList<int32> list;
	EXPECT_FALSE(list.Contains(42));
	EXPECT_EQ(list.Num(), 0);
}

TEST(LinkedListTests, AddSingleAndContains)
{
	TLinkedList<int32> list;
	list.Add(42);

	EXPECT_TRUE(list.Contains(42));
	EXPECT_FALSE(list.Contains(99));
	EXPECT_EQ(list.Num(), 1);
}

TEST(LinkedListTests, AddManyAndContains)
{
	constexpr int32 startValue = 0;
	constexpr int32 numValues = 32;

	TLinkedList<int32> list;
	for (int32 idx = startValue; idx < numValues; ++idx)
	{
		list.Add(idx);
	}

	const auto ListContains = [&list](const int32 value) { return list.Contains(value); };
	for (int32 idx = startValue; idx < numValues; ++idx)
	{
		EXPECT_PRED1(ListContains, idx);
	}
}

TEST(LinkedListTests, Iterator)
{
	TLinkedList<int32> list;
	list.Add(42);
	list.Add(13);
	list.Add(0xBEEF);

	decltype(list)::ConstIteratorType iter = list.CreateConstIterator();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 42);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 13);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 0xBEEF);

	iter.MoveNext();
	EXPECT_FALSE(iter);
}

TEST(LinkedListTests, IteratorRemove)
{
	TLinkedList<int32> values {{ 1, 2, 3, 4, 5, 6 }};
	values.RemoveByPredicate([](int32 value) { return (value % 2) == 0; });

	for (auto iter = values.CreateIterator(); iter; ++iter)
	{
		if ((*iter) % 2 == 0)
		{
			iter.Remove();
		}
	}

	decltype(values)::ConstIteratorType iter = values.CreateConstIterator();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 1);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 3);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 5);

	iter.MoveNext();
	EXPECT_FALSE(iter);
}

TEST(LinkedListTests, RemoveFromListWithOneElement)
{
	TLinkedList<int32> list;
	list.Add(42);

	EXPECT_EQ(list.Num(), 1);
	EXPECT_TRUE(list.Contains(42));

	EXPECT_TRUE(list.Remove(42));
	EXPECT_EQ(list.Num(), 0);

	EXPECT_FALSE(list.Remove(42));
}

TEST(LinkedListTests, RemoveHead)
{
	TLinkedList<int32> list;
	list.Add(42);
	list.Add(13);
	list.Add(0xBEEF);

	EXPECT_EQ(list.Num(), 3);
	EXPECT_TRUE(list.Contains(42));
	EXPECT_TRUE(list.Contains(13));
	EXPECT_TRUE(list.Contains(0xBEEF));

	EXPECT_TRUE(list.Remove(42));

	EXPECT_FALSE(list.Contains(42));
	EXPECT_TRUE(list.Contains(13));
	EXPECT_TRUE(list.Contains(0xBEEF));
	EXPECT_EQ(list.Num(), 2);
}

TEST(LinkedListTests, RemoveMiddle)
{
	TLinkedList<int32> list;
	list.Add(42);
	list.Add(13);
	list.Add(0xBEEF);

	EXPECT_EQ(list.Num(), 3);
	EXPECT_TRUE(list.Contains(42));
	EXPECT_TRUE(list.Contains(13));
	EXPECT_TRUE(list.Contains(0xBEEF));

	EXPECT_TRUE(list.Remove(13));

	EXPECT_TRUE(list.Contains(42));
	EXPECT_FALSE(list.Contains(13));
	EXPECT_TRUE(list.Contains(0xBEEF));
	EXPECT_EQ(list.Num(), 2);
}

TEST(LinkedListTests, RemoveTail)
{
	TLinkedList<int32> list;
	list.Add(42);
	list.Add(13);
	list.Add(0xBEEF);

	EXPECT_EQ(list.Num(), 3);
	EXPECT_TRUE(list.Contains(42));
	EXPECT_TRUE(list.Contains(13));
	EXPECT_TRUE(list.Contains(0xBEEF));

	EXPECT_TRUE(list.Remove(0xBEEF));

	EXPECT_TRUE(list.Contains(42));
	EXPECT_TRUE(list.Contains(13));
	EXPECT_FALSE(list.Contains(0xBEEF));
	EXPECT_EQ(list.Num(), 2);
}

TEST(LinkedListTests, RemoveByPredicate)
{
	TLinkedList<int32> values {{ 1, 2, 3, 4, 5, 6 }};
	values.RemoveByPredicate([](int32 value) { return (value % 2) == 0; });

	decltype(values)::ConstIteratorType iter = values.CreateConstIterator();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 1);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 3);

	iter.MoveNext();
	EXPECT_TRUE(iter);
	EXPECT_EQ(*iter, 5);

	iter.MoveNext();
	EXPECT_FALSE(iter);
}
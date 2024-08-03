#include "Containers/HashTable.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Logging.h"
#include <gtest/gtest.h>

template<typename T>
[[maybe_unused]] static void PrintHashTableBuckets(const THashTable<T>& hashTable)
{
	using BucketType = typename THashTable<T>::FBucket;

	UM_LOG(Info, "BEGIN HASH TABLE(Size={})", hashTable.Num());

	int32 index = 0;
	for (const BucketType& bucket : hashTable.DebugGetBuckets())
	{
		if (bucket.HasValue())
		{
			UM_LOG(Info, "{} -> Value={}, Hash={}", index, bucket.GetValue(), bucket.CachedHash);
		}
		else
		{
			UM_LOG(Info, "{} -> ---", index);
		}

		++index;
	}

	UM_LOG(Info, "END HASH TABLE");
}

TEST(HashTableTests, DefaultConstruct)
{
	THashTable<int32> hashTable;
	EXPECT_EQ(hashTable.Num(), 0);

	int32 numElements = 0;
	for (auto iter = hashTable.CreateIterator(); iter; ++iter)
	{
		++numElements;
	}

	EXPECT_EQ(numElements, 0);

	int32 numConstElements = 0;
	for (auto iter = hashTable.CreateConstIterator(); iter; ++iter)
	{
		++numConstElements;
	}

	EXPECT_EQ(numConstElements, 0);
}

TEST(HashTableTests, AddAndContainsSingle)
{
	THashTable<int32> hashTable;

	constexpr int32 singleValue = 20;
	EXPECT_TRUE(hashTable.Add(singleValue));
	EXPECT_TRUE(hashTable.Contains(singleValue));
}

TEST(HashTableTests, AddAndContainsManyDispersed)
{
	const auto ExecuteTest = [](const int32 startValue, const int32 deltaValue, const int32 numValues)
	{
		THashTable<int32> hashTable;
		hashTable.Reserve(numValues);

		const auto AddToHashTable = [&hashTable](int32 value)
		{
			return hashTable.Add(value);
		};

		const auto HashTableContains = [&hashTable](int32 value)
		{
			return hashTable.Contains(value);
		};

		// Add all the values
		for (int32 idx = 0; idx < numValues; ++idx)
		{
			EXPECT_PRED1(AddToHashTable, idx * deltaValue + startValue);
		}

		// Ensure the values are in the hash table
		for (int32 idx = 0; idx < numValues; ++idx)
		{
			EXPECT_PRED1(HashTableContains, idx * deltaValue + startValue);
		}
	};

	constexpr int32 primeStartValue = 11;
	constexpr int32 primeDeltaValue = 19;
	constexpr int32 primeNumValues = 108631;

	constexpr int32 startValue = 10;
	constexpr int32 deltaValue = 18;
	constexpr int32 numValues = 10240;

	// The prime numbers here present one of the worst case scenarios for dispersing values in the table
	ExecuteTest(primeStartValue, primeDeltaValue, primeNumValues);
	ExecuteTest(startValue, deltaValue, numValues);
}

TEST(HashTableTests, AddAndContainsManyInSeries)
{
	const auto ExecuteTest = [](const int32 startValue, const int32 deltaValue, const int32 numValues)
	{
		THashTable<int32> hashTable;
		hashTable.Reserve(numValues);

		const auto AddToHashTable = [&hashTable](int32 value)
		{
			return hashTable.Add(value);
		};

		const auto HashTableContains = [&hashTable](int32 value)
		{
			return hashTable.Contains(value);
		};

		// Add all the values
		for (int32 idx = 0; idx < numValues; ++idx)
		{
			EXPECT_PRED1(AddToHashTable, idx * deltaValue + startValue);
		}

		// Ensure the values are in the hash table
		for (int32 idx = 0; idx < numValues; ++idx)
		{
			EXPECT_PRED1(HashTableContains, idx * deltaValue + startValue);
		}
	};

	constexpr int32 startValue = 0;
	constexpr int32 deltaValue = 1;
	constexpr int32 numValues = 10240;

	ExecuteTest(startValue, deltaValue, numValues);
}

TEST(HashTableTests, AddAndContainsMultipleInSeries)
{
	using ValueType = char;
	using HashTableType = THashTable<ValueType>;

	HashTableType hashTable;
	EXPECT_EQ(hashTable.Num(), 0);

	const auto AbleToAddToTable = [&hashTable](const ValueType value) { return hashTable.Add(value); };
	const auto UnableToAddToTable = [&hashTable](const ValueType value) { return hashTable.Add(value) == false; };
	const auto TableContains = [&hashTable](const ValueType value) { return hashTable.Contains(value); };

	// Test if the hash table can store all of ASCII
	for (char value = 0x20; value < 0x7F; ++value)
	{
		EXPECT_PRED1(AbleToAddToTable, value);
	}
	for (char value = 0x20; value < 0x7F; ++value)
	{
		EXPECT_PRED1(UnableToAddToTable, value);
	}
	for (char value = 0x20; value < 0x7F; ++value)
	{
		EXPECT_PRED1(TableContains, value);
	}
}

TEST(HashTableTests, AddAndContainsMultipleOfSameValue)
{
	THashTable<int32> hashTable;
	EXPECT_EQ(hashTable.Num(), 0);

	EXPECT_TRUE(hashTable.Add(20));
	EXPECT_FALSE(hashTable.Add(20));
	EXPECT_FALSE(hashTable.Add(20));

	EXPECT_EQ(hashTable.Num(), 1);

	EXPECT_TRUE(hashTable.Contains(20));
}

TEST(HashTableTests, AddAndContainsString)
{
	THashTable<FStringView> hashTable;
	EXPECT_TRUE(hashTable.Add("hello"_sv));
	EXPECT_TRUE(hashTable.Add("world"_sv));
	EXPECT_TRUE(hashTable.Contains("hello"_sv));
	EXPECT_TRUE(hashTable.Contains("world"_sv));
	EXPECT_FALSE(hashTable.Contains("HELLO"_sv));
	EXPECT_FALSE(hashTable.Contains("WORLD"_sv));
}

TEST(HashTableTests, Iterator)
{
	THashTable<int32> hashTable;
	EXPECT_TRUE(hashTable.Add(20));
	EXPECT_TRUE(hashTable.Add(30));
	EXPECT_TRUE(hashTable.Add(40));
	EXPECT_TRUE(hashTable.Add(50));

	TArray<int32> hashTableValues;
	for (auto iter = hashTable.CreateIterator(); iter; ++iter)
	{
		hashTableValues.Add(*iter);
	}

	EXPECT_EQ(hashTable.Num(), hashTableValues.Num());
	EXPECT_TRUE(hashTableValues.Contains(20));
	EXPECT_TRUE(hashTableValues.Contains(30));
	EXPECT_TRUE(hashTableValues.Contains(40));
	EXPECT_TRUE(hashTableValues.Contains(50));
	EXPECT_FALSE(hashTableValues.Contains(60));
}

TEST(HashTableTests, ConstIterator)
{
	THashTable<int32> hashTable;
	EXPECT_TRUE(hashTable.Add(20));
	EXPECT_TRUE(hashTable.Add(30));
	EXPECT_TRUE(hashTable.Add(40));
	EXPECT_TRUE(hashTable.Add(50));

	TArray<int32> hashTableValues;
	for (auto iter = hashTable.CreateConstIterator(); iter; ++iter)
	{
		hashTableValues.Add(*iter);
	}

	EXPECT_EQ(hashTable.Num(), hashTableValues.Num());
	EXPECT_TRUE(hashTableValues.Contains(20));
	EXPECT_TRUE(hashTableValues.Contains(30));
	EXPECT_TRUE(hashTableValues.Contains(40));
	EXPECT_TRUE(hashTableValues.Contains(50));
}

TEST(HashTableTests, Remove)
{
	THashTable<int32> hashTable;

	EXPECT_TRUE(hashTable.Add(10));
	EXPECT_TRUE(hashTable.Contains(10));
	EXPECT_TRUE(hashTable.Remove(10));

	EXPECT_TRUE(hashTable.Add(13));
	EXPECT_TRUE(hashTable.Contains(13));
	EXPECT_FALSE(hashTable.Contains(10));

	EXPECT_TRUE(hashTable.Remove(13));
	EXPECT_FALSE(hashTable.Contains(13));
}
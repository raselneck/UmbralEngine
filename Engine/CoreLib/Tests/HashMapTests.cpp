#include "Containers/HashMap.h"
#include "Containers/StringView.h"
#include "Engine/Logging.h"
#include <gtest/gtest.h>

template<typename KeyType, typename ValueType>
static void DebugPrintBuckets(const THashMap<KeyType, ValueType>& hashMap)
{
	using PairType = TKeyValuePair<KeyType, ValueType>;
	using BucketType = typename THashTable<PairType>::BucketType;

	const TArray<BucketType>& buckets = hashMap.DebugGetBuckets();

	UM_LOG(Info, "Count: {}", buckets.Num());

	int32 index = -1;
	for (const BucketType& bucket : buckets)
	{
		++index;

		if (bucket.HasValue() == false)
		{
			UM_LOG(Info, "~~ [{}] ------------", index);
			continue;
		}

		UM_LOG(Info, "~~ [{}] '{}' -> '{}'", index, bucket.GetValue().Key, bucket.GetValue().Value);
	}
}

TEST(HashMapTests, BracketOperator)
{
	THashMap<int32, FStringView> hashMap;

	hashMap[10] = "ten"_sv;
	hashMap[20] = "twenty"_sv;
	hashMap[30] = "thirty"_sv;

	EXPECT_EQ(hashMap[40], nullptr);
	EXPECT_EQ(hashMap[30], "thirty"_sv);
	EXPECT_EQ(hashMap[20], "twenty"_sv);
	EXPECT_EQ(hashMap[10], "ten"_sv);
}
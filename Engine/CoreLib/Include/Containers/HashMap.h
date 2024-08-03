#pragma once

#include "Containers/HashTable.h"
#include "Containers/Pair.h"
#include "Engine/Hashing.h"

/**
 * @brief Defines hash map, which uses a hash table to map keys to values.
 *
 * @tparam TKey The key type.
 * @tparam TValue The value type.
 */
template<typename TKey, typename TValue>
class THashMap final : private THashTable<TKeyValuePair<TKey, TValue>>
{
	using Super = THashTable<TKeyValuePair<TKey, TValue>>;
	using BucketType = typename Super::BucketType;
	using TraitsType = TComparisonTraits<TKey>;

public:

	using KeyType = TKey;
	using ValueType = TValue;
	using PairType = TKeyValuePair<TKey, TValue>;

	using Super::Add;
	using Super::Clear;
	using Super::Contains;
	using Super::CreateIterator;
	using Super::CreateConstIterator;
	using Super::GetCapacity;
	using Super::IsEmpty;
	using Super::Num;
	using Super::Reserve;
	using Super::Reset;

	/**
	 * @brief Sets default values for this hash map's properties.
	 */
	THashMap() = default;

	/**
	 * @brief Sets default values for this hash map's properties.
	 *
	 * @param values The initial set of values for this hash map.
	 */
	THashMap(std::initializer_list<PairType> values)
		: Super(values)
	{
	}

	// TODO CreateKeyIterator
	// TODO CreateConstKeyIterator

	/**
	 * @brief Adds a key-value pair to this hash map.
	 *
	 * @param key The key value.
	 * @param value The value.
	 * @return True if the pair was added, or false if an element exists with the given key.
	 */
	[[maybe_unused]] bool Add(KeyType key, ValueType value)
	{
		PairType valueToAdd { MoveTemp(key), MoveTemp(value) };
		return Super::Add(MoveTemp(valueToAdd));
	}

	/**
	 * @brief Checks to see if this map contains a value with the given key.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return True if this map contains a value with the given key, otherwise false.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] bool ContainsKey(const SimilarKeyType& key) const
	{
		return FindBucketForKey<SimilarKeyType>(key) != nullptr;
	}

	/**
	 * @brief Attempts to find the value associated with a given key.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return A pointer to the value associated with \p key, or nullptr if a pair was not found.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] const ValueType* Find(const SimilarKeyType& key) const
	{
		const BucketType* bucket = FindBucketForKey(key);
		if (bucket && bucket->HasValue())
		{
			return &bucket->GetValue().Value;
		}
		return nullptr;
	}

	/**
	 * @brief Attempts to find the value associated with a given key.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return A pointer to the value associated with \p key, or nullptr if a pair was not found.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] ValueType* Find(const SimilarKeyType& key)
	{
		BucketType* bucket = FindBucketForKey(key);
		if (bucket && bucket->HasValue())
		{
			return &bucket->GetValue().Value;
		}
		return nullptr;
	}

	/**
	 * @brief Gets a reference to the value associated with a given key. Will assert if the key is not in this map.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return The found value.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] const ValueType& FindRef(const SimilarKeyType& key) const
	{
		const BucketType* bucket = FindBucketForKey(key);
		UM_ASSERT(bucket && bucket->HasValue(), "This map does not contain the specified key");

		return bucket->GetValue().Value;
	}

	/**
	 * @brief Gets a reference to the value associated with a given key. Will assert if the key is not in this map.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return The found value.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] ValueType& FindRef(const SimilarKeyType& key)
	{
		BucketType* bucket = FindBucketForKey(key);
		UM_ASSERT(bucket && bucket->HasValue(), "This map does not contain the specified key");

		return bucket->GetValue().Value;
	}

	/**
	 * @brief Attempts to remove the key-value pair designated by the given key.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key of the pair to remove.
	 * @return True if the value was removed, otherwise false.
	 */
	template<typename SimilarKeyType>
	[[maybe_unused]] bool Remove(const SimilarKeyType& key)
	{
		if (BucketType* bucket = FindBucketForKey(key))
		{
			bucket->Reset();
			return true;
		}
		return false;
	}

	/**
	 * @brief Gets a reference to the value associated with a given key. Will assert if the key is not in this map.
	 *
	 * @remarks Equivalent to calling FindRef(const KeyType&).
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return The found value.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] const ValueType& operator[](const SimilarKeyType& key) const
	{
		return FindRef(key);
	}

	/**
	 * @brief Gets a reference to the value associated with a given key. If the key-value pair is not in this map,
	 *        then a default-constructed value will be added with the given key and then returned.
	 *
	 * @tparam SimilarKeyType The type of the key. Must be comparable with KeyType.
	 * @param key The key.
	 * @return The found or added value.
	 */
	template<typename SimilarKeyType>
	[[nodiscard]] ValueType& operator[](const SimilarKeyType& key)
	{
		static_assert(TIsDefaultConstructible<ValueType>::Value);
		static_assert(TIsConstructible<KeyType, SimilarKeyType>::Value, "Cannot construct key with given key type");

		ValueType* value = Find(key);
		if (value)
		{
			return *value;
		}

		PairType pairToAdd;
		pairToAdd.Key = KeyType { key };
		pairToAdd.Value = {};

		BucketType* pairBucket = Super::AddValueAndGetBucket(MoveTemp(pairToAdd));
		UM_ASSERT(pairBucket && pairBucket->HasValue(), "Failed to add key to hash map");

		return pairBucket->GetValue().Value;
	}

	// STL compatibility BEGIN
	using Super::begin;
	using Super::end;
	// STL compatibility END

#if WITH_TESTING
	using Super::DebugGetBuckets;
#endif

private:

	/**
	 * @brief Attempts to find the bucket for the given key.
	 *
	 * @param key The key.
	 * @return The bucket corresponding to the key, otherwise nullptr.
	 */
	template<typename SimilarKeyType>
	const BucketType* FindBucketForKey(const SimilarKeyType& key) const
	{
		using OtherTraitsType = TComparisonTraits<SimilarKeyType>;

		const uint64 keyHash = GetHashCode(key);
		return Super::FindBucketByPredicate(keyHash, [&key](const BucketType& bucket, const uint64 bucketHash)
		{
			if (bucket.CachedHash != bucketHash || bucket.HasValue() == false)
			{
				return false;
			}

			const PairType& bucketPair = bucket.GetValue();
			return OtherTraitsType::Equals(static_cast<SimilarKeyType>(bucketPair.Key), key);
		});
	}

	/**
	 * @brief Attempts to find the bucket for the given key.
	 *
	 * @param key The key.
	 * @return The bucket corresponding to the key, otherwise nullptr.
	 */
	template<typename SimilarKeyType>
	BucketType* FindBucketForKey(const SimilarKeyType& key)
	{
		return const_cast<BucketType*>(const_cast<const THashMap*>(this)->template FindBucketForKey<SimilarKeyType>(key));
	}
};
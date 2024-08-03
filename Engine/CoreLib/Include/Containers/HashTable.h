#pragma once

#include "Containers/Array.h"
#include "Containers/Optional.h"
#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/TypeTraits.h"
#include <initializer_list>

namespace Private
{
	// clang-tidy off

	/**
	 * @brief A table of prime numbers to use for hash table sizes.
	 *
	 * @remarks Copied from the .NET Reference Source. See HashHelper class.
	 */
	constexpr TStaticArray<int32, 72> PrimeTable
	{{
		3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631,
		761, 919, 1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103,
		12143, 14591, 17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631,
		130363, 156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403,
		968897, 1162687, 1395263, 1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559,
		5999471, 7199369
	}};

	// clang-tidy on

	/**
	 * @brief Gets the next prime capacity value at least two times the given current capacity.
	 *
	 * @param currentCapacity The current capacity.
	 * @return The next capacity.
	 */
	constexpr int32 GetNextPrime(const int32 currentCapacity)
	{
		int32 nextCapacityIndex = PrimeTable.IndexOfByPredicate([=](const int32 value)
		{
			return value > currentCapacity;
		});

		if (nextCapacityIndex == INDEX_NONE)
		{
			nextCapacityIndex = PrimeTable.Num() - 1;
		}

		return PrimeTable[nextCapacityIndex];
	}

	/**
	 * @brief Defines a hash table bucket.
	 *
	 * @tparam T The bucket value type.
	 */
	template<typename T>
	class THashTableBucket
	{
	public:

		using ElementType = T;

		static constexpr uint64 InvalidHash = 0xFFFF'FFFF'FFFF'FFFFULL;

		/** @brief The cached hash of this bucket's value. */
		uint64 CachedHash = InvalidHash;

		/** @brief This bucket's value. */
		TOptional<ElementType> Value;

		/**
		 * @brief Gets a pointer to this bucket's value. Will assert if this bucket does not have a value.
		 *
		 * @return This bucket's value.
		 */
		[[nodiscard]] const ElementType* GetValueAsPointer() const
		{
			UM_ASSERT(Value.HasValue(), "Attempting to retrieve value from empty bucket");
			return Value.GetValueAsPointer();
		}

		/**
		 * @brief Gets a pointer to this bucket's value. Will assert if this bucket does not have a value.
		 *
		 * @return This bucket's value.
		 */
		[[nodiscard]] ElementType* GetValueAsPointer()
		{
			UM_ASSERT(Value.HasValue(), "Attempting to retrieve value from empty bucket");
			return Value.GetValueAsPointer();
		}

		/**
		 * @brief Gets this bucket's value. Will assert if this bucket does not have a value.
		 *
		 * @return This bucket's value.
		 */
		[[nodiscard]] const ElementType& GetValue() const
		{
			UM_ASSERT(Value.HasValue(), "Attempting to retrieve value from empty bucket");
			return Value.GetValue();
		}

		/**
		 * @brief Gets this bucket's value. Will assert if this bucket does not have a value.
		 *
		 * @return This bucket's value.
		 */
		[[nodiscard]] ElementType& GetValue()
		{
			UM_ASSERT(Value.HasValue(), "Attempting to retrieve value from empty bucket");
			return Value.GetValue();
		}

		/**
		 * @brief Gets a value indicating whether or not this bucket has a value.
		 *
		 * @return True if this bucket has a value, otherwise false.
		 */
		[[nodiscard]] bool HasValue() const
		{
			return Value.HasValue();
		}

		/**
		 * @brief Resets this bucket, clearing out any value it might have.
		 */
		void Reset()
		{
			CachedHash = InvalidHash;
			Value.Reset();
		}
	};
}

/**
 * @brief Defines a hash table, which can store unique values with fast insertion, look-up, and removal.
 *
 * @tparam T The element type contained within the hash table.
 */
template<typename T>
class THashTable
{
public:

	using ElementType = T;
	using BucketType = Private::THashTableBucket<ElementType>;
	using SizeType = typename TArray<BucketType>::SizeType;
	using TraitsType = TComparisonTraits<ElementType>;

	/**
	 * @brief Defines an iterator for this hash table.
	 */
	template<bool IsConst>
	class TIterator
	{
	public:

		using ThisType = TIterator;
		using BucketArrayType = typename TConditionalAddConst<IsConst, TArray<BucketType>>::Type;

		/**
		 * @brief Sets default values for this iterator's properties.
		 *
		 * @param hashTable The hash table to iterate.
		 */
		TIterator(BucketArrayType& buckets, SizeType index)
			: m_Buckets { buckets }
			, m_Index { index }
		{
			if (m_Buckets.IsValidIndex(m_Index) && m_Buckets[m_Index].HasValue())
			{
				return;
			}

			MoveNext();
		}

		/**
		 * @brief Returns a value indicating whether or not this iterator is valid.
		 *
		 * @return True if this iterator is valid, otherwise false.
		 */
		[[nodiscard]] bool IsValid() const
		{
			return m_Buckets.IsValidIndex(m_Index);
		}

		/**
		 * @brief Moves this iterator to the next valid element.
		 */
		void MoveNext()
		{
			do
			{
				++m_Index;
			}
			while (m_Buckets.IsValidIndex(m_Index) && m_Buckets[m_Index].HasValue() == false);
		}

		/**
		 * @brief Checks to see if this iterator is equal to another iterator.
		 *
		 * @param other The other iterator.
		 * @return True if this iterator is equal to \p other, otherwise false.
		 */
		[[nodiscard]] bool operator==(const ThisType& other) const
		{
			return &m_Buckets == &other.m_Buckets && m_Index == other.m_Index;
		}

		/**
		 * @brief Checks to see if this iterator is not equal to another iterator.
		 *
		 * @param other The other iterator.
		 * @return True if this iterator is not equal to \p other, otherwise false.
		 */
		[[nodiscard]] bool operator!=(const ThisType& other) const
		{
			return &m_Buckets != &other.m_Buckets || m_Index != other.m_Index;
		}

		/**
		 * @brief Returns a value indicating whether or not this iterator is valid.
		 *
		 * @return True if this iterator is valid, otherwise false.
		 */
		[[nodiscard]] operator bool() const
		{
			return IsValid();
		}

		/**
		 * @brief Moves this iterator to the next valid element.
		 *
		 * @return This iterator.
		 */
		ThisType& operator++()
		{
			MoveNext();
			return *this;
		}

		/**
		 * @brief Moves this iterator to the next valid element.
		 *
		 * @return A copy of this iterator before incrementing.
		 */
		ThisType operator++(int)
		{
			ThisType returnValue = *this;
			MoveNext();
			return returnValue;
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		const ElementType* operator->() const
		{
			return m_Buckets[m_Index].GetValueAsPointer();
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		ElementType* operator->()
		    requires(IsConst == false)
		{
			return m_Buckets[m_Index].GetValueAsPointer();
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		const ElementType& operator*() const
		{
			return m_Buckets[m_Index].GetValue();
		}

		/**
		 * @brief Gets the current value this iterator points to.
		 *
		 * @return The current value this iterator points to.
		 */
		ElementType& operator*()
			requires(IsConst == false)
		{
			return m_Buckets[m_Index].GetValue();
		}

	private:

		BucketArrayType& m_Buckets;
		SizeType m_Index;
	};

	using IteratorType = TIterator<false>;
	using ConstIteratorType = TIterator<true>;

	/**
	 * @brief Sets default values for this hash table's properties.
	 */
	THashTable() = default;

	/**
	 * @brief Sets default values for this hash table's properties.
	 *
	 * @param values The initial set of values for this hash table.
	 */
	THashTable(std::initializer_list<ElementType> values)
	{
		Reserve(static_cast<SizeType>(values.size()));

		for (const ElementType& value : values)
		{
			Add(value);
		}
	}

	/**
	 * @brief Attempts to add the given element value.
	 *
	 * @param element
	 * @return True if the element was added, otherwise false.
	 */
	[[maybe_unused]] bool Add(const ElementType& element)
	{
		return AddValue(element);
	}

	/**
	 * @brief Attempts to add the given element value.
	 *
	 * @param element
	 * @return True if the element was added, otherwise false.
	 */
	[[maybe_unused]] bool Add(ElementType&& element)
	{
		return AddValue(MoveTemp(element));
	}

	/**
	 * @brief Clears this hash table.
	 */
	void Clear()
	{
		m_Buckets.Clear();
		m_NumItems = 0;
	}

	/**
	 * @brief Checks to see if this hash table contains the given element.
	 *
	 * @param element The element to check for.
	 * @return True if this hash table contains the given element, otherwise false.
	 */
	[[nodiscard]] bool Contains(const ElementType& element) const
	{
		return FindBucketForValue(element) != nullptr;
	}

	/**
	 * @brief Creates a non-mutating iterator for this hash table.
	 *
	 * @return A non-mutating iterator for this hash table.
	 */
	[[nodiscard]] ConstIteratorType CreateConstIterator() const
	{
		return ConstIteratorType { m_Buckets, INDEX_NONE };
	}

	/**
	 * @brief Creates a non-mutating iterator for this hash table.
	 *
	 * @return A non-mutating iterator for this hash table.
	 */
	[[nodiscard]] ConstIteratorType CreateIterator() const
	{
		return ConstIteratorType { m_Buckets, INDEX_NONE };
	}

	/**
	 * @brief Creates an iterator for this hash table.
	 *
	 * @return An iterator for this hash table.
	 */
	[[nodiscard]] IteratorType CreateIterator()
	{
		return IteratorType { m_Buckets, INDEX_NONE };
	}

	/**
	 * @brief Gets the number of items that this hash table can contain before re-growing.
	 *
	 * @return The number of items that this hash table can contain before re-growing.
	 */
	[[nodiscard]] SizeType GetCapacity() const
	{
		return m_Buckets.GetCapacity();
	}

	/**
	 * @brief Returns a value indicating whether or not this hash table is empty.
	 *
	 * @return True if this hash table is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_Buckets.IsEmpty();
	}

	/**
	 * @brief Gets the number of items currently in this hash table.
	 *
	 * @return The number of items currently in this hash table.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_NumItems;
	}

	/**
	 * @brief Attempts to remove an element from this hash table.
	 *
	 * @param element The element value to remove.
	 * @return True if the element was removed, otherwise false.
	 */
	[[maybe_unused]] bool Remove(const ElementType& element)
	{
		if (BucketType* elementBucket = FindBucketForValue(element))
		{
			elementBucket->Reset();
			return true;
		}
		return false;
	}

	/**
	 * @brief Reserves at least a certain number of buckets in this hash table.
	 *
	 * @param count The minimum number of buckets to reserve.
	 */
	void Reserve(const SizeType count)
	{
		const SizeType actualCount = Private::GetNextPrime(count);
		if (actualCount <= m_Buckets.GetCapacity())
		{
			return;
		}

		GrowBucketsArrayToCapacity(m_Buckets, actualCount);
	}

	/**
	 * @brief Clears this hash table without releasing its memory.
	 */
	void Reset()
	{
		m_Buckets.Reset();
		m_NumItems = 0;
	}

	// STL compatibility BEGIN
	IteratorType      begin()       { return CreateIterator(); }
	ConstIteratorType begin() const { return CreateConstIterator(); }
	IteratorType      end()         { return IteratorType { m_Buckets, m_Buckets.Num() + 1 }; }
	ConstIteratorType end()   const { return ConstIteratorType { m_Buckets, m_Buckets.Num() + 1 }; }
	// STL compatibility END

#if WITH_TESTING
	/**
	 * @brief Gets this hash table's bucket array.
	 *
	 * @return This hash table's bucket array.
	 */
	[[nodiscard]] const TArray<BucketType>& DebugGetBuckets() const
	{
		return m_Buckets;
	}
#endif

protected:

	/**
	 * @brief Does the heavy lifting of adding a value to this hash table.
	 *
	 * @param value The value to add.
	 * @return True if the value was added, otherwise false.
	 */
	[[nodiscard]] bool AddValue(ElementType value)
	{
		return AddValueAndGetBucket(MoveTemp(value)) != nullptr;
	}

	/**
	 * @brief Does the heavy lifting of adding a value to this hash table.
	 *
	 * @param value The value to add.
	 * @return The value's bucket if it was added, otherwise nullptr.
	 */
	[[nodiscard]] BucketType* AddValueAndGetBucket(ElementType value)
	{
		if (m_Buckets.IsEmpty() || m_NumItems >= m_Buckets.GetCapacity())
		{
			ConditionalGrow();
		}

		BucketType* valueBucket = AddValueToBucketsArray(m_Buckets, MoveTemp(value), [this]
		{
			constexpr bool forceGrow = true;
			ConditionalGrow(forceGrow);
		});

		if (valueBucket)
		{
			++m_NumItems;
		}

		return valueBucket;
	}

	/**
	 * @brief Adds a value to a bucket array.
	 *
	 * @tparam GrowCallbackType The type of the grow callback.
	 * @param buckets The bucket array.
	 * @param value The value to add to the bucket array.
	 * @param growCallback The callback to use when needing to grow the buckets array.
	 * @return True if the value was newly added to the buckets array, otherwise false (if the element was already there).
	 */
	template<typename GrowCallbackType>
	[[nodiscard]] static BucketType* AddValueToBucketsArray(TArray<BucketType>& buckets, ElementType value, GrowCallbackType growCallback)
	{
		const uint64 elementHash = GetHashCode(value);
		uint64 hashForBucketLookup = elementHash;

		while (true)
		{
			BucketType* bucket = FindBucketForAddFromHash(buckets, hashForBucketLookup);
			if (bucket == nullptr)
			{
				hashForBucketLookup = RehashHashCode(hashForBucketLookup);
			}
			else if (bucket->HasValue() == false)
			{
				bucket->CachedHash = hashForBucketLookup;
				bucket->Value = MoveTemp(value);
				return bucket;
			}
			else if (TraitsType::Equals(bucket->GetValue(), value))
			{
				return nullptr;
			}
			else
			{
				growCallback();
			}
		}
	}

	/**
	 * @brief Grows this hash table if it needs to.
	 *
	 * @param forceGrow True to force a grow.
	 */
	void ConditionalGrow(const bool forceGrow = false)
	{
		if (m_NumItems < m_Buckets.GetCapacity() && forceGrow == false)
		{
			return;
		}

		GrowBucketsArray(m_Buckets);
	}

	/**
	 * @brief Finds a mutable bucket for use when adding to this hash table based on the element's hash.
	 *
	 * @param hash The hash.
	 * @return The bucket for \p hash, or nullptr if there is no bucket for \p hash.
	 */
	[[nodiscard]] static BucketType* FindBucketForAddFromHash(TArray<BucketType>& buckets, const uint64 hash)
	{
		const int32 index = GetBucketIndexForHash(buckets, hash);
		UM_ASSERT(index != INDEX_NONE, "Hash table has not yet been initialized");

		BucketType& bucket = buckets[index];
		if (bucket.HasValue() == false || bucket.CachedHash == hash)
		{
			return &bucket;
		}

		return nullptr;
	}

	/**
	 * @brief Finds the bucket for a given hash.
	 *
	 * @param hash The hash.
	 * @return The bucket for \p hash, or nullptr if there is no bucket for \p hash.
	 */
	[[nodiscard]] const BucketType* FindBucketForHash(const uint64 hash) const
	{
		return FindBucketByPredicate(hash, [](const BucketType& bucket, const uint64 bucketHash)
		{
			return bucket.CachedHash == bucketHash;
		});
	}

	/**
	 * @brief Finds a bucket based on an initial hash and a predicate.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param initialHash The initial hash.
	 * @param predicate The predicate.
	 * @return The bucket determined by the predicate.
	 */
	template<typename PredicateType>
	[[nodiscard]] const BucketType* FindBucketByPredicate(const uint64 initialHash, PredicateType predicate) const
		requires TIsCallable<bool, PredicateType, const BucketType&, uint64>::Value
	{
		if (m_Buckets.IsEmpty())
		{
			return nullptr;
		}

		uint64 bucketHash = initialHash;
		int32 bucketIndex = GetBucketIndexForHash(m_Buckets, bucketHash);
		if (bucketIndex == INDEX_NONE)
		{
			return nullptr;
		}

		// HACK The most common fail case for successfully adding something but then failing the contains check is
		//      hitting the original bucket index with a different hash value. The extra probe count allows us to
		//      iterate this loop at least once more in an attempt to find the correct bucket. This is labelled as
		//      a hack, though, because that feels wrong...
		int32 extraProbeCount = 1;
		const int32 originalBucketIndex = bucketIndex;
		do
		{
			const BucketType& bucket = m_Buckets[bucketIndex];
			if (predicate(bucket, bucketHash))
			{
				return &bucket;
			}

			bucketHash = RehashHashCode(bucketHash);
			bucketIndex = GetBucketIndexForHash(m_Buckets, bucketHash);

			if (originalBucketIndex == bucketIndex && extraProbeCount > 0)
			{
				bucketIndex = (bucketIndex + 1) % m_Buckets.GetCapacity();
				--extraProbeCount;
			}
		}
		while (originalBucketIndex != bucketIndex);

		return nullptr;
	}

	/**
	 * @brief Finds the bucket for a given value.
	 *
	 * @param value The value.
	 * @return The bucket for \p value, or nullptr if one could not be found.
	 */
	[[nodiscard]] const BucketType* FindBucketForValue(const ElementType& value) const
	{
		return FindBucketByPredicate(GetHashCode(value), [&value](const BucketType& bucket, const uint64 bucketHash)
		{
			return bucket.CachedHash == bucketHash && bucket.HasValue() && TraitsType::Equals(bucket.GetValue(), value);
		});
	}

	/**
	 * @brief Finds the bucket for a given value.
	 *
	 * @param value The value.
	 * @return The bucket for \p value, or nullptr if one could not be found.
	 */
	[[nodiscard]] BucketType* FindBucketForValue(const ElementType& value)
	{
		return const_cast<BucketType*>(const_cast<const THashTable*>(this)->FindBucketForValue(value));
	}

	/**
	 * @brief Gets the bucket index for the given hash.
	 *
	 * @param buckets The bucket array.
	 * @param hash The hash.
	 * @return The bucket index for \p hash.
	 */
	[[nodiscard]] static int32 GetBucketIndexForHash(const TArray<BucketType>& buckets, const uint64 hash)
	{
		const SizeType bucketCapacity = buckets.GetCapacity();
		UM_ASSERT(bucketCapacity > 2, "Hash table has not yet been initialized");

		return static_cast<int32>(hash % static_cast<uint64>(bucketCapacity));
	}

	/**
	 * @brief Grows a bucket array.
	 *
	 * @param buckets The bucket array.
	 */
	static void GrowBucketsArray(TArray<BucketType>& buckets)
	{
		const SizeType previousCapacity = buckets.GetCapacity();
		const SizeType nextCapacity = Private::GetNextPrime(previousCapacity * 2);
		UM_ASSERT(nextCapacity > previousCapacity, "Attempting to grow hash table too large");

		GrowBucketsArrayToCapacity(buckets, nextCapacity);
	}

	/**
	 * @brief Grows a bucket array.
	 *
	 * @param buckets The bucket array.
	 */
	static void GrowBucketsArrayToCapacity(TArray<BucketType>& buckets, const SizeType newCapacity)
	{
		UM_ASSERT(newCapacity > buckets.GetCapacity(), "Attempting to grow hash table with smaller capacity");

		// We do in fact want to SetNum here so that all the elements are default-constructed
		TArray<BucketType> newBuckets;
		newBuckets.SetNum(newCapacity);

		for (const BucketType& oldBucket : buckets)
		{
			if (oldBucket.HasValue() == false)
			{
				continue;
			}

			(void)AddValueToBucketsArray(newBuckets, oldBucket.GetValue(), []
			{
				// Why shouldn't this array be growing you may ask?
				// Because this one is at least 2x+1 times the size of our current buckets array :)
				UM_ASSERT_NOT_REACHED_MSG("This bucket array should not be growing!");
			});
		}

		buckets = MoveTemp(newBuckets);
	}

	/**
	 * @brief Re-hashes a hash code.
	 *
	 * @param hash The hash code to re-hash.
	 * @return The re-hashed hash code.
	 */
	[[nodiscard]] static uint64 RehashHashCode(const uint64 hash)
	{
		return Private::HashBytes({ reinterpret_cast<const uint8*>(&hash), sizeof(hash) });
	}

private:

	TArray<BucketType> m_Buckets;
	SizeType m_NumItems = 0;
};
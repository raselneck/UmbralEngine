#pragma once

#include "Engine/Hashing.h"
#include "Templates/ComparisonTraits.h"

/**
 * @brief Defines a pair.
 *
 * @tparam FirstType The type of the first item in the pair.
 * @tparam SecondType The type of the second item in the pair.
 */
template<typename FirstType, typename SecondType>
class TPair
{
public:

	/**
	 * @brief The first item in the pair.
	 */
	FirstType First;

	/**
	 * @brief The second item in the pair.
	 */
	SecondType Second;
};

/**
 * @brief Gets the hash code for the given pair.
 *
 * @tparam FirstType The first type in the pair.
 * @tparam SecondType The second type in the pair.
 * @param pair The pair.
 * @return The hash code.
 */
template<typename FirstType, typename SecondType>
uint64 GetHashCode(const TPair<FirstType, SecondType>& pair)
{
	return HashItems(pair.First, pair.Second);
}

/**
 * @brief Defines a key-value pair.
 *
 * @tparam KeyType The type of the key.
 * @tparam ValueType The type of the value.
 */
template<typename KeyType, typename ValueType>
class TKeyValuePair
{
public:

	/**
	 * @brief The key.
	 */
	KeyType Key;

	/**
	 * @brief The value.
	 */
	ValueType Value;
};

/**
 * @brief Defines comparison traits for a key-value pair.
 *
* @tparam KeyType The type of the key.
* @tparam ValueType The type of the value.
 */
template<typename KeyType, typename ValueType>
class TComparisonTraits<TKeyValuePair<KeyType, ValueType>>
{
public:

	using Type = TKeyValuePair<KeyType, ValueType>;

	/**
	 * @brief Compares two key-value pairs.
	 *
	 * @param left The left value.
	 * @param right The right value.
	 * @return The result of the comparison.
	 */
	static inline ECompareResult Compare(const Type& left, const Type& right)
	{
		return TComparisonTraits<KeyType>::Compare(left.Key, right.Key);
	}

	/**
	 * @brief Checks to see if two key-value pairs are equal.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return True if the two values are equal, otherwise false.
	 */
	static inline bool Equals(const Type& first, const Type& second)
	{
		return TComparisonTraits<KeyType>::Equals(first.Key, second.Key);
	}
};

/**
 * @brief Gets the hash code for the given key-value pair.
 *
 * @tparam KeyType The key type.
 * @tparam ValueType The value type.
 * @param pair The key-value pair.
 * @return The hash code.
 */
template<typename KeyType, typename ValueType>
uint64 GetHashCode(const TKeyValuePair<KeyType, ValueType>& pair)
{
	return GetHashCode(pair.Key);
}
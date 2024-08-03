#pragma once

#if __has_include(<compare>)
#	include <compare> /* For std::strong_ordering */
#	define WITH_STRONG_ORDERING 1
#else
#	define WITH_STRONG_ORDERING 0
#endif

/**
 * @brief An enumeration of possible comparison results.
 */
enum class ECompareResult
{
	LessThan = -1,
	Equals,
	GreaterThan
};

/**
 * @brief Defines comparison traits for a type.
 *
 * @tparam T The type.
 */
template<typename T>
class TComparisonTraits
{
public:

	using Type = T;

	/**
	 * @brief Compares two values.
	 *
	 * @param left The left value.
	 * @param right The right value.
	 * @return The result of the comparison.
	 */
	static inline ECompareResult Compare(const Type& left, const Type& right)
	{
#if WITH_STRONG_ORDERING
		const std::strong_ordering result = left <=> right;
		if (result == std::strong_ordering::less)
		{
			return ECompareResult::LessThan;
		}
		if (result == std::strong_ordering::greater)
		{
			return ECompareResult::GreaterThan;
		}
		return ECompareResult::Equals;
#else
		if (left < right)
		{
			return ECompareResult::LessThan;
		}
		if (left > right)
		{
			return ECompareResult::GreaterThan;
		}
		return ECompareResult::Equals;
#endif
	}

	/**
	 * @brief Checks to see if two values are equal.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return True if the two values are equal, otherwise false.
	 */
	static inline bool Equals(const Type& first, const Type& second)
	{
		return Compare(first, second) == ECompareResult::Equals;
	}
};
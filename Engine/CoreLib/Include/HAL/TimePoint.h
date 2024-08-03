#pragma once

#include "HAL/TimeSpan.h"

/**
 * @brief Defines a time point in terms of ticks (similar to FDateTime and FTimeSpan).
 */
class FTimePoint final
{
public:

	/**
	 * @brief Sets default values for this time point's properties.
	 */
	FTimePoint() = default;

	/**
	 * @brief Sets default values for this time point's properties.
	 *
	 * @param ticks The ticks.
	 */
	explicit FTimePoint(int64 ticks);

	/**
	 * @brief Gets this time point as a time span.
	 *
	 * @return This time point as a time span.
	 */
	[[nodiscard]] FTimeSpan AsTimeSpan() const;

	/**
	 * @brief Compares this time point to another time point.
	 *
	 * @param other The other time point.
	 * @return The comparison result.
	 */
	[[nodiscard]] constexpr ECompareResult Compare(const FTimePoint other) const
	{
		if (m_Ticks > other.m_Ticks)
		{
			return ECompareResult::GreaterThan;
		}
		if (m_Ticks < other.m_Ticks)
		{
			return ECompareResult::LessThan;
		}
		return ECompareResult::Equals;
	}

	/**
	 * @brief Gets this time point's ticks.
	 *
	 * @return This time point's ticks.
	 */
	[[nodiscard]] constexpr int64 GetTicks() const
	{
		return m_Ticks;
	}

	/**
	 * @brief Gets the time point for right now.
	 *
	 * @return The time point for right now.
	 */
	static FTimePoint Now();

	/**
	 * @brief Gets the difference in time between this time point and another as a time span.
	 *
	 * @param other The other time point.
	 * @return The duration between this time span and \p other as a time span.
	 */
	[[nodiscard]] FTimeSpan operator-(const FTimePoint& other) const;

	// BEGIN STD COMPATIBILITY
	[[nodiscard]] constexpr bool operator< (const FTimePoint other) const { return Compare(other) == ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator<=(const FTimePoint other) const { return Compare(other) != ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator> (const FTimePoint other) const { return Compare(other) == ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator>=(const FTimePoint other) const { return Compare(other) != ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator==(const FTimePoint other) const { return Compare(other) == ECompareResult::Equals; }
	[[nodiscard]] constexpr bool operator!=(const FTimePoint other) const { return Compare(other) != ECompareResult::Equals; }
	// END STD COMPATIBILITY

private:

	int64 m_Ticks = 0;
};
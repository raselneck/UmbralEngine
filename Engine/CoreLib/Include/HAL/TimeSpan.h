#pragma once

#include "Engine/IntTypes.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/NumericLimits.h"

/**
 * @brief Defines a time span, which is a tick-relative duration.
 */
class FTimeSpan final
{
	/**
	 * @brief The minimum number of ticks allowed for time spans.
	 */
	static constexpr int64 MinTimeSpanTicks = TNumericLimits<int64>::MinValue;

	/**
	 * @brief The maximum number of ticks allowed for time spans.
	 */
	static constexpr int64 MaxTimeSpanTicks = TNumericLimits<int64>::MaxValue;

public:

	/**
	 * @brief The number of ticks in one microsecond. (1 tick = 100 nanoseconds)
	 */
	static constexpr int64 TicksPerMicrosecond = 10;

	/**
	 * @brief The number of ticks in one millisecond. (1 tick = 100 nanoseconds)
	 */
	static constexpr int64 TicksPerMillisecond = TicksPerMicrosecond * 1000;

	/**
	 * @brief The number of ticks in one second.
	 */
	static constexpr int64 TicksPerSecond = TicksPerMillisecond * 1000;

	/**
	 * @brief The number of ticks in one minute.
	 */
	static constexpr int64 TicksPerMinute = TicksPerSecond * 60;

	/**
	 * @brief The number of ticks in one hour.
	 */
	static constexpr int64 TicksPerHour = TicksPerMinute * 60;

	/**
	 * @brief The number of ticks in one day.
	 */
	static constexpr int64 TicksPerDay = TicksPerHour * 24;

	/**
	 * @brief The maximum time span value.
	 */
	static const FTimeSpan MaxValue;

	/**
	 * @brief The minimum time span value.
	 */
	static const FTimeSpan MinValue;

	/**
	 * @brief A time span with no actual time.
	 */
	static const FTimeSpan Zero;

	/**
	 * @brief Sets default values for this time span's properties.
	 */
	constexpr FTimeSpan() = default;

	/**
	 * @brief Creates a new time span object.
	 *
	 * @param ticks The number of ticks
	 */
	constexpr explicit FTimeSpan(const int64 ticks)
		: m_Ticks { ticks }
	{
	}

	/**
	 * @brief Creates a new time span object.
	 *
	 * @param hours The time span's hours.
	 * @param minutes The time span's minutes.
	 * @param seconds The time span's seconds.
	 */
	FTimeSpan(int32 hours, int32 minutes, int32 seconds);

	/**
	 * @brief Creates a new time span object.
	 *
	 * @param hours The time span's hours.
	 * @param minutes The time span's minutes.
	 * @param seconds The time span's seconds.
	 * @param millis The time span's milliseconds.
	 */
	FTimeSpan(int32 hours, int32 minutes, int32 seconds, int32 millis);

	/**
	 * @brief Compares this time span to another time span.
	 *
	 * @param other The other time span.
	 * @return The comparison result.
	 */
	[[nodiscard]] constexpr ECompareResult Compare(const FTimeSpan other) const
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
	 * @brief Creates a time span from an amount of minutes.
	 *
	 * @param minutes The total minutes representing the time span.
	 * @returns The time span.
	 */
	static constexpr FTimeSpan FromMinutes(const double minutes)
	{
		return FromMilliseconds(minutes * 60'000.0);
	}

	/**
	 * @brief Creates a time span from an amount of milliseconds.
	 *
	 * @param millis The total milliseconds representing the time span.
	 * @returns The time span.
	 */
	static constexpr FTimeSpan FromMilliseconds(const double millis)
	{
		const int64 ticks = RoundDoubleToInt(millis * TicksPerMillisecond);
		return FTimeSpan { ticks };
	}

	/**
	 * @brief Creates a time span from an amount of seconds.
	 *
	 * @param seconds The total seconds representing the time span.
	 * @returns The time span.
	 */
	static constexpr FTimeSpan FromSeconds(const double seconds)
	{
		return FromMilliseconds(seconds * 1000.0);
	}

	/**
	 * @brief Gets the number of whole days represented by this time span.
	 *
	 * @return The number of whole days represented by this time span.
	 */
	[[nodiscard]] int32 GetDays() const
	{
		return static_cast<int32>(m_Ticks / TicksPerDay);
	}

	/**
	 * @brief Gets the number of whole hours represented by this time span.
	 *
	 * @return The number of whole hours represented by this time span.
	 */
	[[nodiscard]] int32 GetHours() const
	{
		return static_cast<int32>(m_Ticks / TicksPerHour) % 24;
	}

	/**
	 * @brief Gets the number of whole milliseconds represented by this time span.
	 *
	 * @return The number of whole milliseconds represented by this time span.
	 */
	[[nodiscard]] int32 GetMilliseconds() const
	{
		return static_cast<int32>(m_Ticks / TicksPerMillisecond) % 1000;
	}

	/**
	 * @brief Gets the number of whole minutes represented by this time span.
	 *
	 * @return The number of whole minutes represented by this time span.
	 */
	[[nodiscard]] int32 GetMinutes() const
	{
		return static_cast<int32>(m_Ticks / TicksPerMinute) % 60;
	}

	/**
	 * @brief Gets the number of whole seconds represented by this time span.
	 *
	 * @return The number of whole seconds represented by this time span.
	 */
	[[nodiscard]] int32 GetSeconds() const
	{
		return static_cast<int32>(m_Ticks / TicksPerSecond) % 60;
	}

	/**
	 * @brief Gets the total number of ticks in this time span.
	 *
	 * @return The total number of ticks in this time span.
	 */
	[[nodiscard]] int64 GetTicks() const
	{
		return m_Ticks;
	}

	/**
	 * @brief Gets the total number of days represented by this time span.
	 *
	 * @return The total number of days represented by this time span.
	 */
	[[nodiscard]] double GetTotalDays() const;

	/**
	 * @brief Gets the total number of hours represented by this time span.
	 *
	 * @return The total number of hours represented by this time span.
	 */
	[[nodiscard]] double GetTotalHours() const;

	/**
	 * @brief Gets the total number of milliseconds represented by this time span.
	 *
	 * @return The total number of milliseconds represented by this time span.
	 */
	[[nodiscard]] double GetTotalMilliseconds() const;

	/**
	 * @brief Gets the total number of minutes represented by this time span.
	 *
	 * @return The total number of minutes represented by this time span.
	 */
	[[nodiscard]] double GetTotalMinutes() const;

	/**
	 * @brief Gets the total number of seconds represented by this time span.
	 *
	 * @return The total number of seconds represented by this time span.
	 */
	[[nodiscard]] double GetTotalSeconds() const;

	// TODO ToString methods

	// BEGIN STD COMPATIBILITY
	[[nodiscard]] constexpr bool operator< (const FTimeSpan other) const { return Compare(other) == ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator<=(const FTimeSpan other) const { return Compare(other) != ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator> (const FTimeSpan other) const { return Compare(other) == ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator>=(const FTimeSpan other) const { return Compare(other) != ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator==(const FTimeSpan other) const { return Compare(other) == ECompareResult::Equals; }
	[[nodiscard]] constexpr bool operator!=(const FTimeSpan other) const { return Compare(other) != ECompareResult::Equals; }
	// END STD COMPATIBILITY

	/**
	 * @brief Adds another time span to this time span.
	 *
	 * @param other The other time span to add to this one.
	 * @returns This time span.
	 */
	constexpr FTimeSpan& operator+=(const FTimeSpan other)
	{
		m_Ticks += other.m_Ticks;
		return *this;
	}

	/**
	 * @brief Adds a time span to another time span, and returns the result.
	 *
	 * @param other The other time span.
	 * @returns The resulting time span.
	 */
	constexpr friend FTimeSpan operator+(FTimeSpan first, const FTimeSpan second)
	{
		first += second;
		return first;
	}

	/**
	 * @brief Subtracts another time span from this time span.
	 *
	 * @param other The other time span to subtract from this one.
	 * @returns This time span.
	 */
	constexpr FTimeSpan& operator-=(const FTimeSpan other)
	{
		m_Ticks -= other.m_Ticks;
		return *this;
	}

	/**
	 * @brief Subtracts a time span from another time span, and returns the result.
	 *
	 * @param other The other time span.
	 * @returns The resulting time span.
	 */
	constexpr friend FTimeSpan operator-(FTimeSpan first, const FTimeSpan second)
	{
		first -= second;
		return first;
	}

private: // Functions

	/**
	 * @brief Rounds a double to an integer.
	 *
	 * @param value The double value to round.
	 * @return \p value rounded to an integer.
	 */
	static constexpr int64 RoundDoubleToInt(const double value)
	{
		return static_cast<int64>(value + (value >= 0.0 ? 0.5 : -0.5));
	}

	// TODO Add some kind of TChecked type that can detect potential overflows and use it here
	int64 m_Ticks = 0;
};

template<>
struct TIsZeroConstructible<FTimeSpan> : FTrueType
{
};
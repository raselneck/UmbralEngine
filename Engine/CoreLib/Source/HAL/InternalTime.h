#pragma once

#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "HAL/DateTime.h"
#include "HAL/TimeSpan.h"
#include <limits>

namespace Time
{
	/**
	 * @brief Represents the number of milliseconds per second.
	 */
	constexpr int64 MillisPerSecond = 1000;

	/**
	 * @brief Represents the number of milliseconds per minute.
	 */
	constexpr int64 MillisPerMinute = MillisPerSecond * 60;

	/**
	 * @brief Represents the number of milliseconds per hour.
	 */
	constexpr int64 MillisPerHour = MillisPerMinute * 60;

	/**
	 * @brief Represents the number of milliseconds per day.
	 */
	constexpr int64 MillisPerDay = MillisPerHour * 24;

	/**
	 * @brief Represents the fractional amount of a day that passes per tick.
	 */
	constexpr double DaysPerTick = 1.0 / FTimeSpan::TicksPerDay;

	/**
	 * @brief Represents the fractional amount of an hour that passes per tick.
	 */
	constexpr double HoursPerTick = 1.0 / FTimeSpan::TicksPerHour;

	/**
	 * @brief Represents the fractional amount of a millisecond that passes per tick.
	 */
	constexpr double MillisPerTick = 1.0 / FTimeSpan::TicksPerMillisecond;

	/**
	 * @brief Represents the fractional amount of a minute that passes per tick.
	 */
	constexpr double MinutesPerTick = 1.0 / FTimeSpan::TicksPerMinute;

	/**
	 * @brief Represents the fractional amount of a second that passes per tick.
	 */
	constexpr double SecondsPerTick = 1.0 / FTimeSpan::TicksPerSecond;

	/**
	 * @brief Represents the number of days per non-leap year.
	 */
	constexpr int64 DaysPerYear = 365;

	/**
	 * @brief Represents the number of days in four years.
	 */
	constexpr int64 DaysPer4Years = DaysPerYear * 4 + 1;

	/**
	 * @brief Represents the number of days in 100 years.
	 */
	constexpr int64 DaysPer100Years = DaysPer4Years * 25 - 1;

	/**
	 * @brief Represents the number of days in 400 years.
	 */
	constexpr int64 DaysPer400Years = DaysPer100Years * 4 + 1;

	/**
	 * @brief Represents the number of days from 01/01/0001 to 12/31/9999.
	 */
	constexpr int64 DaysTo10000 = DaysPer400Years * 25 - 366;

	/**
	 * @brief Contains the number of days per month. (Index zero is not a month, January is at index one.)
	 */
	extern const TStaticArray<int32, 13> DaysPerMonth;

	/**
	 * @brief Contains the number of days until the beginning of a month in a non-leap year. (Index zero is not a month, January is at index one.)
	 */
	extern const TStaticArray<int32, 13> DaysToMonth365;

	/**
	 * @brief Contains the number of days until the beginning of a month in a leap year. (Index zero is not a month, January is at index one.)
	 */
	extern const TStaticArray<int32, 13> DaysToMonth366;

	/**
	 * @brief Converts nanoseconds to ticks.
	 *
	 * @param nanoseconds The number of nanoseconds to convert.
	 * @return The number of ticks equivalent to the given number of nanoseconds.
	 */
	constexpr int64 NanosecondsToTicks(const uint64 nanoseconds)
	{
		// 1 nanosecond  = 1000 microseconds
		// 1 microsecond = 10 ticks (see FTimeSpan::TicksPerMicrosecond)
		// 1 tick = 1 microsecond / 10
		// 1 tick = (1 microsecond / 10) * (1000 microseconds / nanoseconds)
		// 1 tick = (1 / 10 * 1000) nanoseconds
		// 1 tick = 100 nanoseconds
		constexpr uint64 nanosToTicks = 1000 / static_cast<uint64>(FTimeSpan::TicksPerMicrosecond);
		static_assert(nanosToTicks == 100);

		return static_cast<int64>(nanoseconds / nanosToTicks);
	}

	/**
	 * @brief Converts nanoseconds to a time span.
	 *
	 * @param nanoseconds The number of nanoseconds to convert.
	 * @return The time span equivalent to the given number of nanoseconds.
	 */
	constexpr FTimeSpan NanosecondsToTimeSpan(const uint64 nanoseconds)
	{
		return FTimeSpan { NanosecondsToTicks(nanoseconds) };
	}

	/**
	 * @brief Converts the given number of ticks to nanoseconds.
	 *
	 * @param ticks The ticks.
	 * @return The equivalent number of nanoseconds.
	 */
	constexpr int64 TicksToNanoseconds(const int64 ticks)
	{
		// 1 nanosecond  = 1000 microseconds
		// 1 microsecond = 10 ticks (see FTimeSpan::TicksPerMicrosecond)
		// 1 tick = 1 microsecond / 10
		// 1 tick = (1 microsecond / 10) * (1000 microseconds / nanoseconds)
		// 1 tick = (1 / 10 * 1000) nanoseconds
		// 1 tick = 100 nanoseconds

		return ticks * 100;
	}

	/**
	 * @brief Get the total number of ticks that make up a given date.
	 *
	 * @param year The date's year.
	 * @param month The date's month.
	 * @param day The date's day.
	 * @returns The total number of ticks that make up the specified date.
	 */
	int64 DateToTicks(int32 year, int32 month, int32 day);

	/**
	 * @brief Gets the offset, in ticks, from local time to UTC time.
	 *
	 * @return The offset, in ticks, from local time to UTC time.
	 */
	int64 GetLocalTimeToUtcTimeOffset();

	/**
	 * @brief Checks to see if the given year is a leap year.
	 *
	 * @param year The year to check.
	 * @return True if \p year is a leap year, otherwise false.
	 */
	bool IsLeapYear(int32 year);

	/**
	 * @brief Get the total number of ticks that make up a given time.
	 *
	 * @param hour The number of hours.
	 * @param minute The number of minutes.
	 * @param second The number of seconds.
	 * @param millis The number of milliseconds.
	 * @returns The total number of ticks that make up the specified time.
	 */
	int64 TimeToTicks(int32 hour, int32 minute, int32 second, int32 millis);
}
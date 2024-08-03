#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "HAL/TimeSpan.h"
#include "Misc/StringFormatting.h"
#include "Templates/IsZeroConstructible.h"

/**
 * @brief An enumeration of the days of a week.
 */
enum class EDayOfWeek : uint8
{
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

/**
 * @brief An enumeration of the months of a year.
 */
enum class EMonthOfYear : uint8
{
	January = 1,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
};

/**
 * @brief Defines a combination of a date and a time.
 */
class FDateTime final
{
public:

	/**
	 * @brief The default format string used by ToString.
	 */
	static constexpr FStringView DefaultFormat = "%Y/%m/%d %H:%M:%S"_sv;

	/**
	 * @brief The date-time representing the Unix epoch.
	 */
	static const FDateTime Epoch;

	/**
	 * @brief The maximum date-time value.
	 */
	static const FDateTime MaxValue;

	/**
	 * @brief The minimum date-time value.
	 */
	static const FDateTime MinValue;

	/**
	 * @brief Sets default values for this date-time's properties.
	 */
	constexpr FDateTime() = default;

	/**
	 * @brief Creates a new date-time object.
	 *
	 * @param ticks The amount of ticks representing this date-time.
	 */
	explicit FDateTime(int64 ticks);

	/**
	 * @brief Creates a new date-time object.
	 *
	 * @param year The date's year.
	 * @param month The date's month.
	 * @param day The date's day.
	 */
	FDateTime(int32 year, int32 month, int32 day)
		: FDateTime(year, month, day, 0, 0, 0, 0)
	{
	}

	/**
	 * @brief Creates a new date-time object.
	 *
	 * @param year The date's year.
	 * @param month The date's month.
	 * @param day The date's day.
	 * @param hour The time's hour.
	 * @param minute The time's minute.
	 * @param second The time's second.
	 */
	FDateTime(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second)
		: FDateTime(year, month, day, hour, minute, second, 0)
	{
	}

	/**
	 * @brief Creates a new date-time object.
	 *
	 * @param year The date's year.
	 * @param month The date's month.
	 * @param day The date's day.
	 * @param hour The time's hour.
	 * @param minute The time's minute.
	 * @param second The time's second.
	 * @param millis The time's milliseconds.
	 */
	FDateTime(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millis);

	/**
	 * @brief Compares this date-time to another date-time.
	 *
	 * @param other The other date-time.
	 * @return The comparison result.
	 */
	[[nodiscard]] constexpr ECompareResult Compare(const FDateTime other) const
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
	 * @brief Gets the total number of days in a month.
	 *
	 * @param year The year.
	 * @param month The month.
	 * @returns The total number of days in a month.
	 */
	static int32 DaysInMonth(int32 year, int32 month);

	/**
	 * @brief Gets the total number of days in a month.
	 *
	 * @param year The year.
	 * @param month The month.
	 * @returns The total number of days in a month.
	 */
	static int32 DaysInMonth(int32 year, EMonthOfYear month);

	/**
	 * @brief Gets a date-time object with the same date as this date-time, but with the time set to midnight.
	 */
	[[nodiscard]] FDateTime GetDate() const;

	/**
	 * @brief Gets each part of the date represented by this date-time individually.
	 *
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 */
	void GetDate(int32& year, int32& month, int32& day) const;

	/**
	 * @brief Gets the day of the month (1 to 31) represented by this date-time.
	 *
	 * @returns
	 */
	[[nodiscard]] int32 GetDay() const;

	/**
	 * @brief Gets the day of the week this date-time falls on.
	 */
	[[nodiscard]] EDayOfWeek GetDayOfWeek() const;

	/**
	 * @brief Gets the day of the year (1 - 365/366) represented by this date-time object.
	 *
	 * @returns The day of the year represented by this date-time object.
	 */
	[[nodiscard]] int32 GetDayOfYear() const;

	/**
	 * @brief Gets the hour part of the time (0 - 23) represented by this date-time object.
	 *
	 * @returns The hour part of the time represented by this date-time object.
	 */
	[[nodiscard]] int32 GetHour() const;

	/**
	 * @brief Gets the hour part of the time represented by this date-time object in 12-hour format (1 - 12).
	 *
	 * @returns The hour part of the time represented by this date-time object in 12-hour format.
	 */
	[[nodiscard]] int32 GetHour12() const;

	/**
	 * @brief Gets the milliseconds part of the time represented by this date-time object.
	 *
	 * @returns The milliseconds part of the time represented by this date-time object.
	 */
	[[nodiscard]] int32 GetMillisecond() const;

	/**
	 * @brief Gets the minutes part of the time represented by this date-time object.
	 *
	 * @returns The minutes part of the time represented by this date-time object.
	 */
	[[nodiscard]] int32 GetMinute() const;

	/**
	 * @brief Gets the month of the year (1 to 12) represented by this date-time.
	 *
	 * @returns The month of the year represented by this date-time.
	 */
	[[nodiscard]] int32 GetMonth() const;

	/**
	 * @brief Gets the month of the year this date-time is in.
	 *
	 * @returns The month of the year this date-time is in.
	 */
	[[nodiscard]] EMonthOfYear GetMonthOfYear() const;

	/**
	 * @brief Gets the seconds part of the time represented by this date-time object.
	 *
	 * @returns The seconds part of the time represented by this date-time object.
	 */
	[[nodiscard]] int32 GetSecond() const;

	/**
	 * @brief Gets the total number of ticks in this date-time object.
	 *
	 * @returns The total number of ticks in this date-time object.
	 */
	[[nodiscard]] int64 GetTicks() const
	{
		return m_Ticks;
	}

	/**
	 * @brief Gets each part of the time represented by this date-time individually.
	 *
	 * @param hour The hour.
	 * @param minute The minute.
	 * @param second The second.
	 */
	void GetTime(int32_t& hour, int32_t& minute, int32_t& second) const
	{
		hour = GetHour();
		minute = GetMinute();
		second = GetSecond();
	}

	/**
	 * @brief Gets each part of the time represented by this date-time individually.
	 *
	 * @param hour The hour.
	 * @param minute The minute.
	 * @param second The second.
	 * @param millisecond The millisecond.
	 */
	void GetTime(int32_t& hour, int32_t& minute, int32_t& second, int32_t& millisecond) const
	{
		hour = GetHour();
		minute = GetMinute();
		second = GetSecond();
		millisecond = GetMillisecond();
	}

	/**
	 * @brief Gets the time represented by this date-time object as a time span.
	 *
	 * @return The time represented by this date-time object as a time span.
	 */
	[[nodiscard]] FTimeSpan GetTimeAsTimeSpan() const
	{
		return FTimeSpan { GetHour(), GetMinute(), GetSecond(), GetMillisecond() };
	}

	/**
	 * @brief Gets the year represented by this date-time.
	 *
	 * @returns The year represented by this date-time.
	 */
	[[nodiscard]] int32 GetYear() const;

	/**
	 * @brief Checks to see if the time represented in this date-time object is in the afternoon.
	 *
	 * @returns True if the result of calling GetHour() is greater than or equal to 12.
	 */
	[[nodiscard]] bool IsAfternoon() const
	{
		return GetHour() >= 12;
	}

	/**
	 * @brief Checks to see if the year represented by this date-time is a leap year.
	 *
	 * @returns True if this date-time's year is a leap year, otherwise false.
	 */
	[[nodiscard]] bool IsLeapYear() const;

	/**
	 * @brief Checks to see if the given year is a leap year.
	 *
	 * @param year The year.
	 * @returns True if the given year is a leap year, otherwise false.
	 */
	[[nodiscard]] static bool IsLeapYear(int32 year);

	/**
	 * @brief Checks to see if the time represented in this date-time object is before noon.
	 *
	 * @returns True if the result of calling GetHour() is less than 12.
	 */
	[[nodiscard]] bool IsMorning() const
	{
		return GetHour() < 12;
	}

	/**
	 * @brief Checks to see if this date-time object is valid.
	 *
	 * @returns True if this date-time object is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Gets a date-time object that represents the current date and time expressed in local time.
	 *
	 * @returns The local date-time object for right now.
	 */
	static FDateTime Now();

	/**
	 * @brief Converts this date-time to a string using the given format. The available format specifiers are:
	 *   - `%a`: "am" or "pm".
	 *   - `%A`: "AM" or "PM".
	 *   - `%d`: Day of the month (1 - 31).
	 *   - `%D`: Day of the year (1 - 365/366).
	 *   - `%m`: Month of the year (1 - 12).
	 *   - `%y`: Last two digits of the year (00 - 99).
	 *   - `%Y`: Full year.
	 *   - `%h`: Hour of the day (12-hour format).
	 *   - `%H`: Hour of the day (24-hour format).
	 *   - `%M`: Minute of the hour.
	 *   - `%S`: Second of the minute.
	 *   - `%s`: Millisecond of the second.
	 *
	 * @param format The format string.
	 * @param result The string to populate with this date-time object's representation.
	 */
	[[nodiscard]] FString ToString(FStringView format) const;

	/**
	 * @brief Converts this date-time to a string using the default format.
	 *
	 * @returns This date-time object as a string.
	 */
	[[nodiscard]] FString ToString() const
	{
		return ToString(FDateTime::DefaultFormat);
	}

	/**
	 * @brief Converts this date-time object to a Unix epoch-relative timestamp.
	 *
	 * @returns This date-time object as a Unix timestamp.
	 */
	[[nodiscard]] int64 ToUnixTimestamp() const;

	/**
	 * @brief Converts this date-time into its equivalent local date-time. (Assumes this date-time is UTC.)
	 *
	 * @returns The local time equivalent of this date-time object.
	 */
	[[nodiscard]] FDateTime ToLocalTime() const;

	/**
	 * @brief Converts this date-time into its equivalent UTC date-time. (Assumes this date-time is local.)
	 *
	 * @returns The UTC equivalent of this date-time object.
	 */
	[[nodiscard]] FDateTime ToUtcTime() const;

	/**
	 * @brief Gets a date-time object that represents the current date and time expressed in Coordinated Universal Time (UTC).
	 *
	 * @returns The UTC date-time object for right now.
	 */
	static FDateTime UtcNow();

	/**
	 * @brief Adds a time span to this date time, creating a new date time in the future.
	 *
	 * @param timeSpan The time span.
	 * @returns This date-time object plus the given amount of time.
	 */
	FDateTime operator +(const FTimeSpan timeSpan) const
	{
		return FDateTime { m_Ticks + timeSpan.GetTicks() };
	}

	/**
	 * @brief Subtracts a time span from this date time, creating a new date time in the past.
	 *
	 * @param timeSpan The time span.
	 * @returns This date-time object minus the given amount of time.
	 */
	FDateTime operator -(const FTimeSpan timeSpan) const
	{
		return FDateTime { m_Ticks - timeSpan.GetTicks() };
	}

	/**
	 * @brief Subtracts one date-time from another, creating a time span.
	 *
	 * @param other The other date-time object.
	 * @returns The time span representing the difference between this date-time object and \p other.
	 */
	FTimeSpan operator -(const FDateTime other) const
	{
		return FTimeSpan { m_Ticks - other.m_Ticks };
	}

	// BEGIN STD COMPATIBILITY
	[[nodiscard]] constexpr bool operator< (const FDateTime other) const { return Compare(other) == ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator<=(const FDateTime other) const { return Compare(other) != ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator> (const FDateTime other) const { return Compare(other) == ECompareResult::GreaterThan; }
	[[nodiscard]] constexpr bool operator>=(const FDateTime other) const { return Compare(other) != ECompareResult::LessThan; }
	[[nodiscard]] constexpr bool operator==(const FDateTime other) const { return Compare(other) == ECompareResult::Equals; }
	[[nodiscard]] constexpr bool operator!=(const FDateTime other) const { return Compare(other) != ECompareResult::Equals; }
	// END STD COMPATIBILITY

private: // Properties

	int64 m_Ticks { 0 };
};

template<>
struct TIsZeroConstructible<FDateTime> : FTrueType
{
};

template<>
class TFormatter<FDateTime>
{
public:

	void BuildString(const FDateTime& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);

private:

	FStringView m_FormatString;
};
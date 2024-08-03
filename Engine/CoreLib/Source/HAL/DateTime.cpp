#include "Containers/String.h"
#include "Containers/InternalString.h"
#include "Containers/StringView.h"
#include "Engine/Assert.h"
#include "HAL/DateTime.h"
#include "HAL/InternalTime.h"
#include "Math/Math.h"
#include "Misc/StringBuilder.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsTime.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleTime.h"
#else
#	include "HAL/Linux/LinuxTime.h"
#endif

// Minimum number of (signed) ticks that can represent a date-time
constexpr int64 DateTimeMinTicks = 0;

// Maximum number of (signed) ticks that can represent a date-time
constexpr int64 DateTimeMaxTicks = Time::DaysTo10000 * FTimeSpan::TicksPerDay - 1;

// Maximum number of milliseconds in a date-time
//constexpr int64 DateTimeMaxMillis = Time::DaysTo10000 * Time::MillisPerDay;

const FDateTime FDateTime::Epoch { 1970, 1, 1 };
const FDateTime FDateTime::MaxValue { DateTimeMaxTicks };
const FDateTime FDateTime::MinValue { DateTimeMinTicks };

FDateTime::FDateTime(const int64 ticks)
	: m_Ticks { ticks }
{
	UM_ASSERT(IsValid(), "Ticks parameter is out of range");
}

FDateTime::FDateTime(const int32 year, const int32 month, const int32 day, const int32 hour, const int32 minute, const int32 second, const int32 millisecond)
{
	const int64 dateTicks = Time::DateToTicks(year, month, day);
	const int64 timeTicks = Time::TimeToTicks(hour, minute, second, millisecond);
	const int64 totalTicks = dateTicks + timeTicks;

	m_Ticks = totalTicks;

	UM_ASSERT(IsValid(), "Specified date time is out of range");
}

int32 FDateTime::DaysInMonth(const int32 year, const int32 month)
{
	if (month >= 1 && month <= 12)
	{
		const bool isLeapYear = IsLeapYear(year);
		return (month == 2 && isLeapYear) ? 29 : Time::DaysPerMonth[month];
	}

	return 0;
}

int32 FDateTime::DaysInMonth(const int32 Year, const EMonthOfYear month)
{
	return DaysInMonth(Year, static_cast<int32>(month));
}

FDateTime FDateTime::GetDate() const
{
	// This is a shortcut that removes all of the ticks representing the time
	return FDateTime {m_Ticks - (m_Ticks % FTimeSpan::TicksPerDay) };
}

void FDateTime::GetDate(int32& year, int32& month, int32& day) const
{
	// NOTE: This is a slightly modified copy-paste from the .NET reference source:
	// https://referencesource.microsoft.com/#mscorlib/system/datetime.cs,28f94ab932951fd7,references

	// N = number of days since 1/1/0001
	int64 N = m_Ticks / FTimeSpan::TicksPerDay;
	// Y400 = number of whole 400-year periods since 1/1/0001
	int64 Y400 = N / Time::DaysPer400Years;
	// N = day number within 400-year period
	N -= Y400 * Time::DaysPer400Years;
	// Y100 = number of whole 100-year periods within 400-year period
	int64 Y100 = N / Time::DaysPer100Years;
	// Last 100-year period has an extra day, so decrement result if 4
	if (Y100 == 4) Y100 = 3;
	// N = day number within 100-year period
	N -= Y100 * Time::DaysPer100Years;
	// Y4 = number of whole 4-year periods within 100-year period
	const int64 Y4 = N / Time::DaysPer4Years;
	// N = day number within 4-year period
	N -= Y4 * Time::DaysPer4Years;
	// Y1 = number of whole years within 4-year period
	int64 Y1 = N / Time::DaysPerYear;
	// Last year has an extra day, so decrement result if 4
	if (Y1 == 4) Y1 = 3;
	// compute year
	year = static_cast<int32>(Y400 * 400 + Y100 * 100 + Y4 * 4 + Y1 + 1);

	// N = day number within year
	N -= Y1 * Time::DaysPerYear;
	// dayOfYear = N + 1;
	// Leap year calculation looks different from IsLeapYear since Y1, Y4, and Y100 are relative to year 1, not year 0
	const bool  isLeapYear = (Y1 == 3) && (Y4 != 24 || Y100 == 3);
	const auto& daysToMonth = isLeapYear ? Time::DaysToMonth366 : Time::DaysToMonth365;
	// All months have less than 32 days, so N >> 5 is a good conservative estimate for the month
	int64 M = FMath::Max<int64>(N >> 5, 0) + 1;
	// M = 1-based month number
	while (N >= daysToMonth[static_cast<int32>(M)]) ++M;
	// compute month and day
	month = static_cast<int32>(M);
	day = static_cast<int32>(N - daysToMonth[month - 1] + 1);
}

int32 FDateTime::GetDay() const
{
	int32 year, month, day;
	GetDate(year, month, day);

	return day;
}

EDayOfWeek FDateTime::GetDayOfWeek() const
{
	return static_cast<EDayOfWeek>((m_Ticks / FTimeSpan::TicksPerDay) % 7);
}

int32 FDateTime::GetDayOfYear() const
{
	int32 year, month, day;
	GetDate(year, month, day);

	for (int32 previousMonth = 1; previousMonth < month; ++previousMonth)
	{
		day += DaysInMonth(year, previousMonth);
	}

	return day;
}

int32 FDateTime::GetHour() const
{
	return static_cast<int32>((m_Ticks / FTimeSpan::TicksPerHour) % 24);
}

int32 FDateTime::GetHour12() const
{
	const int32 Hour = GetHour();

	if (Hour == 0)
	{
		return 1;
	}
	else if (Hour > 12)
	{
		return Hour - 12;
	}

	return Hour;
}

int32 FDateTime::GetMillisecond() const
{
	return static_cast<int32>((m_Ticks / FTimeSpan::TicksPerMillisecond) % 1000);
}

int32 FDateTime::GetMinute() const
{
	return static_cast<int32>((m_Ticks / FTimeSpan::TicksPerMinute) % 60);
}

int32 FDateTime::GetMonth() const
{
	int32 year, month, day;
	GetDate(year, month, day);

	return month;
}

EMonthOfYear FDateTime::GetMonthOfYear() const
{
	return static_cast<EMonthOfYear>(GetMonth());
}

int32 FDateTime::GetSecond() const
{
	return static_cast<int32>((m_Ticks / FTimeSpan::TicksPerSecond) % 60);
}

int32 FDateTime::GetYear() const
{
	int32 year, month, day;
	GetDate(year, month, day);

	return year;
}

bool FDateTime::IsLeapYear() const
{
	return IsLeapYear(GetYear());
}

bool FDateTime::IsLeapYear(const int32 year)
{
	return Time::IsLeapYear(year);
}

bool FDateTime::IsValid() const
{
	if (m_Ticks < DateTimeMinTicks || m_Ticks > DateTimeMaxTicks)
	{
		return false;
	}

	const int32 year = GetYear();
	const int32 month = GetMonth();
	const int32 day = GetDay();
	const int32 hour = GetHour();
	const int32 minute = GetMinute();
	const int32 second = GetSecond();
	const int32 millisecond = GetMillisecond();

	return (year >= 1 && year <= 9999) &&
	       (month >= 1 && month <= 12) &&
	       (day >= 1 && day <= DaysInMonth(year, month)) &&
	       (hour >= 0 && hour <= 23) &&
	       (minute >= 0 && minute <= 59) &&
	       (second >= 0 && second <= 59) &&
	       (millisecond >= 0 && millisecond <= 999);
}

FDateTime FDateTime::Now()
{
	return FPlatformTime::GetLocalTime();
}

FDateTime FDateTime::ToLocalTime() const
{
	const int64 offset = Time::GetLocalTimeToUtcTimeOffset();
	return FDateTime {m_Ticks - offset };
}

static void BuildDateTimeString(const FDateTime& dateTime, const FStringView format, FStringBuilder& builder)
{
	/**
	 * The available format specifiers are:
	 * %a - "am" or "pm"
	 * %A - "AM" or "PM"
	 * %d - Day of the month (1 - 31)
	 * %D - Day of the year (1 - 365/366)
	 * %m - Month of the year (1 - 12)
	 * %y - Last two digits of the year (00 - 99)
	 * %Y - Full year
	 * %h - Hour of the day (12-hour format)
	 * %H - Hour of the day (24-hour format)
	 * %M - Minute of the hour
	 * %S - Second of the minute
	 * %s - Millisecond of the second
	 */

	if (format.IsEmpty())
	{
		return;
	}

	for (int32 idx = 0; idx < format.Length(); ++idx)
	{
		char ch = format[idx];
		if (ch != '%')
		{
			builder.Append(ch, 1);
			continue;
		}

		if (idx == format.Length() - 1)
		{
			builder.Append("%"_sv);
			break;
		}

		++idx;
		ch = format[idx];

		// TODO Implement everything supported by https://en.cppreference.com/w/c/chrono/strftime
		static const FToCharsArgs padLeftWithZero2 = FToCharsArgs::PadLeft('0', 2);
		static const FToCharsArgs padLeftWithZero3 = FToCharsArgs::PadLeft('0', 3);
		static const FToCharsArgs padLeftWithZero4 = FToCharsArgs::PadLeft('0', 4);
		switch (ch)
		{
		case 'a':
			builder.Append(dateTime.IsMorning() ? "am"_sv : "pm"_sv);
			break;
		case 'A':
			builder.Append(dateTime.IsMorning() ? "AM"_sv : "PM"_sv);
			break;
		case 'd':
			Private::AppendCharsForSignedInt(builder, dateTime.GetDay(), padLeftWithZero2);
			break;
		case 'D':
			Private::AppendCharsForSignedInt(builder, dateTime.GetDayOfYear(), padLeftWithZero3);
			break;
		case 'm':
			Private::AppendCharsForSignedInt(builder, dateTime.GetMonth(), padLeftWithZero2);
			break;
		case 'y':
			Private::AppendCharsForSignedInt(builder, dateTime.GetYear() % 100, padLeftWithZero2);
			break;
		case 'Y':
			Private::AppendCharsForSignedInt(builder, dateTime.GetYear(), padLeftWithZero4);
			break;
		case 'h':
			Private::AppendCharsForSignedInt(builder, dateTime.GetHour12(), padLeftWithZero2);
			break;
		case 'H':
			Private::AppendCharsForSignedInt(builder, dateTime.GetHour(), padLeftWithZero2);
			break;
		case 'M':
			Private::AppendCharsForSignedInt(builder, dateTime.GetMinute(), padLeftWithZero2);
			break;
		case 'S':
			Private::AppendCharsForSignedInt(builder, dateTime.GetSecond(), padLeftWithZero2);
			break;
		case 's':
			Private::AppendCharsForSignedInt(builder, dateTime.GetMillisecond(), padLeftWithZero3);
			break;
		default:
			builder.Append(ch, 1);
			break;
		}
	}
}

FString FDateTime::ToString(const FStringView format) const
{
	FStringBuilder builder;
	BuildDateTimeString(*this, format, builder);
	return builder.ReleaseString();
}

int64 FDateTime::ToUnixTimestamp() const
{
	UM_ASSERT(m_Ticks >= Epoch.m_Ticks, "Date time must be after the epoch to be converted to Unix timestamp");
	return (m_Ticks - Epoch.m_Ticks) / FTimeSpan::TicksPerSecond;
}

FDateTime FDateTime::ToUtcTime() const
{
	const int64 offset = Time::GetLocalTimeToUtcTimeOffset();
	return FDateTime {m_Ticks + offset };
}

FDateTime FDateTime::UtcNow()
{
	return FPlatformTime::GetUtcTime();
}

void TFormatter<FDateTime>::BuildString(const FDateTime& value, FStringBuilder& builder)
{
	BuildDateTimeString(value, m_FormatString, builder);
}

bool TFormatter<FDateTime>::Parse(const FStringView formatString)
{
	m_FormatString = formatString;

	if (m_FormatString.IsEmpty())
	{
		m_FormatString = FDateTime::DefaultFormat;
	}

	return true;
}
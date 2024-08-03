#include "Engine/Assert.h"
#include "Engine/Platform.h"
#include "HAL/DateTime.h"
#include "HAL/InternalTime.h"
#include "HAL/TimeSpan.h"

namespace Time
{
	// Contains the number of days per month
	const TStaticArray<int32, 13> DaysPerMonth {{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

	// Contains the number of days until the beginning of a month in a non-leap year
	const TStaticArray<int32, 13> DaysToMonth365 {{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 }};

	// Contains the number of days until the beginning of a month in a leap year
	const TStaticArray<int32, 13> DaysToMonth366 {{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }};

	// Get the total number of ticks that make up a given date
	int64 DateToTicks(const int32 year, const int32 month, const int32 day)
	{
		UM_ASSERT(year >= 1 && year <= 9999, "Given year is out of range");
		UM_ASSERT(month >= 1 && month <= 12, "Given month is out of range");

		const auto& daysToMonth = IsLeapYear(year) ? DaysToMonth366 : DaysToMonth365;
		UM_ASSERT(day >= 1 && day <= (daysToMonth[month] - daysToMonth[month - 1]), "Given day is invalid for the given month");

		const int32 rawYear = year - 1;
		const int32 totalDays = (rawYear * 365) + (rawYear / 4) - (rawYear / 100) + (rawYear / 400) + daysToMonth[month - 1] + day - 1;

		return totalDays * FTimeSpan::TicksPerDay;
	}

	int64 GetLocalTimeToUtcTimeOffset()
	{
		const FDateTime localTime = FDateTime::Now();
		const FDateTime utcTime = FDateTime::UtcNow();

		const int32 localHour = localTime.GetHour();
		const int32 localMinute = localTime.GetMinute();
		const int32 utcHour = utcTime.GetHour();
		const int32 utcMinute = utcTime.GetMinute();

		// Hopefully we only have to care about hours and minutes :shrug:
		const int64_t localTicks = (FTimeSpan::TicksPerHour * localHour) + (FTimeSpan::TicksPerMinute * localMinute);
		const int64_t utcTicks = (FTimeSpan::TicksPerHour * utcHour) + (FTimeSpan::TicksPerMinute * utcMinute);

		return utcTicks - localTicks;
	}

	bool IsLeapYear(int32 year)
	{
		UM_ASSERT(year >= 1 && year <= 9999, "Given year is invalid!");

		return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
	}

	int64 TimeToTicks(const int32 hour, const int32 minute, const int32 second, const int32 millis)
	{
		UM_ASSERT(hour >= 0 && hour < 24, "Given hour is invalid!");
		UM_ASSERT(minute >= 0 && minute < 60, "Given minute is invalid!");
		UM_ASSERT(second >= 0 && second < 60, "Given second is invalid!");
		UM_ASSERT(millis >= 0 && millis < 1000, "Given millisecond is invalid!");

		return (hour * FTimeSpan::TicksPerHour) +
		       (minute * FTimeSpan::TicksPerMinute) +
		       (second * FTimeSpan::TicksPerSecond) +
		       (millis * FTimeSpan::TicksPerMillisecond);
	}
}
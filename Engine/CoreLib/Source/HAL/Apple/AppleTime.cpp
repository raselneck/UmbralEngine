#include "HAL/InternalTime.h"
#include "HAL/Apple/AppleTime.h"
#include <ctime> /* For clock_gettime_nsec_np, localtime_r, gmtime_r */
#include <sys/time.h> /* For gettimeofday */

static uint64 GReferenceStartTimeNanos = 0;

FDateTime FAppleTime::GetLocalTime()
{
	timeval tv { };
	gettimeofday(&tv, nullptr);

	tm tm { };
	localtime_r(&tv.tv_sec, &tm);

	const int32 year        = tm.tm_year + 1900;
	const int32 month       = tm.tm_mon + 1;
	const int32 day         = tm.tm_mday;
	const int32 hour        = tm.tm_hour;
	const int32 minute      = tm.tm_min;
	const int32 second      = tm.tm_sec;
	const int32 millisecond = static_cast<int32>(tv.tv_usec / 1000);

	return FDateTime { year, month, day, hour, minute, second, millisecond };
}

int64 FAppleTime::GetSystemTimeInTicks()
{
	// See https://www.manpagez.com/man/3/clock_gettime_nsec_np/

	constexpr clockid_t clockId = CLOCK_UPTIME_RAW;

	if (GReferenceStartTimeNanos == 0)
	{
		GReferenceStartTimeNanos = clock_gettime_nsec_np(clockId);
	}

	const uint64 currentTimeNanos = clock_gettime_nsec_np(clockId);
	const uint64 deltaNanos = currentTimeNanos - GReferenceStartTimeNanos;
	return Time::NanosecondsToTicks(deltaNanos);
}

FDateTime FAppleTime::GetUtcTime()
{
	timeval tv { };
	gettimeofday(&tv, nullptr);

	tm tm { };
	gmtime_r(&tv.tv_sec, &tm);

	const int32 year        = tm.tm_year + 1900;
	const int32 month       = tm.tm_mon + 1;
	const int32 day         = tm.tm_mday;
	const int32 hour        = tm.tm_hour;
	const int32 minute      = tm.tm_min;
	const int32 second      = tm.tm_sec;
	const int32 millisecond = static_cast<int32>(tv.tv_usec / 1000);

	return FDateTime { year, month, day, hour, minute, second, millisecond };
}
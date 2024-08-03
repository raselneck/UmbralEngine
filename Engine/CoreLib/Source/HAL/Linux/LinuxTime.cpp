#include "Engine/Assert.h"
#include "HAL/InternalTime.h"
#include "HAL/Linux/LinuxTime.h"
#include <ctime> /* For clock_gettime, localtime_r, gmtime_r */
#include <sys/time.h> /* For gettimeofday */

static timespec GReferenceStartTime { .tv_sec = 0, .tv_nsec = 0 };

/**
 * @brief Converts the given timespec struct into a whole number of nanoseconds.
 *
 * @param time The timespec.
 * @return The number of nanoseconds represented by \p time.
 */
static constexpr uint64 TimespecToNanos(const timespec& time)
{
	// 1 second = 1000 milliseconds
	// 1 millisecond = 1000 microseconds
	// 1 microsecond = 1000 nanoseconds
	// 1 second = 1,000,000,000 nanoseconds
	return time.tv_sec * 1'000'000'000 + time.tv_nsec;
}

/**
 * @brief Gets the delta of two timespec structs in ticks.
 *
 * @param recent The more recent timespec.
 * @param older The older timespec.
 * @return
 */
static constexpr int64 GetTimespecDeltaInTicks(const timespec& recent, const timespec& older)
{
	const uint64 recentNanos = TimespecToNanos(recent);
	const uint64 olderNanos = TimespecToNanos(older);
	const uint64 deltaNanos = recentNanos - olderNanos;
	return Time::NanosecondsToTicks(deltaNanos);
}

FDateTime FLinuxTime::GetLocalTime()
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

int64 FLinuxTime::GetSystemTimeInTicks()
{
	// See https://linux.die.net/man/3/clock_gettime

	constexpr clockid_t clockId = CLOCK_MONOTONIC;

	if (GReferenceStartTime.tv_sec == 0 && GReferenceStartTime.tv_nsec == 0)
	{
		UM_ENSURE(clock_gettime(clockId, &GReferenceStartTime) == 0);
	}

	timespec currentTime {};
	UM_ENSURE(clock_gettime(clockId, &currentTime) == 0);

	return GetTimespecDeltaInTicks(currentTime, GReferenceStartTime);
}

FDateTime FLinuxTime::GetUtcTime()
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
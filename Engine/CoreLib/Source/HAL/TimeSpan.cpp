#include "HAL/TimeSpan.h"
#include "HAL/InternalTime.h"
#include "Math/Math.h"

const FTimeSpan FTimeSpan::MaxValue { FTimeSpan::MaxTimeSpanTicks };
const FTimeSpan FTimeSpan::MinValue { FTimeSpan::MinTimeSpanTicks };
const FTimeSpan FTimeSpan::Zero { 0 };

FTimeSpan::FTimeSpan(const int32 hours, const int32 minutes, const int32 seconds)
	: m_Ticks { Time::TimeToTicks(hours, minutes, seconds, 0) }
{
}

FTimeSpan::FTimeSpan(const int32 hours, const int32 minutes, const int32 seconds, const int32 millis)
	: m_Ticks { Time::TimeToTicks(hours, minutes, seconds, millis) }
{
}

double FTimeSpan::GetTotalDays() const
{
	return static_cast<double>(m_Ticks) * Time::DaysPerTick;
}

double FTimeSpan::GetTotalHours() const
{
	return static_cast<double>(m_Ticks) * Time::HoursPerTick;
}

double FTimeSpan::GetTotalMilliseconds() const
{
	constexpr double MaxMilliseconds = static_cast<double>(MaxTimeSpanTicks / FTimeSpan::TicksPerMillisecond);
	constexpr double MinMilliseconds = static_cast<double>(MinTimeSpanTicks / FTimeSpan::TicksPerMillisecond);

	const double totalMillis = static_cast<double>(m_Ticks) * Time::MillisPerTick;
	return FMath::Clamp(totalMillis, MinMilliseconds, MaxMilliseconds);
}

double FTimeSpan::GetTotalMinutes() const
{
	constexpr double millisToMinutes = 1.0 / 60'000.0;
	return GetTotalMilliseconds() * millisToMinutes;
}

double FTimeSpan::GetTotalSeconds() const
{
	constexpr double millisToSeconds = 1.0 / 1000.0;
	return GetTotalMilliseconds() * millisToSeconds;
}
#include "HAL/InternalTime.h"
#include "HAL/TimePoint.h"
#include "HAL/TimeSpan.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "HAL/Windows/WindowsTime.h"
#elif UMBRAL_PLATFORM_IS_APPLE
#	include "HAL/Apple/AppleTime.h"
#else
#	include "HAL/Linux/LinuxTime.h"
#endif

FTimePoint::FTimePoint(int64 ticks)
	: m_Ticks { ticks }
{
}

FTimeSpan FTimePoint::AsTimeSpan() const
{
	return FTimeSpan { m_Ticks };
}

FTimePoint FTimePoint::Now()
{
	return FTimePoint { FPlatformTime::GetSystemTimeInTicks() };
}

FTimeSpan FTimePoint::operator-(const FTimePoint& other) const
{
	return FTimeSpan { m_Ticks - other.m_Ticks };
}
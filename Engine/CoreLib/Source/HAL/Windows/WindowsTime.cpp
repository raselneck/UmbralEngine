#include "HAL/Windows/WindowsTime.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static double GPerformanceCounterToMillis = -1.0;
static uint64 GPerformanceCounterReference = 0;

FDateTime FWindowsTime::GetLocalTime()
{
	SYSTEMTIME st { };
	::GetLocalTime(&st);

	const int32 year        = st.wYear;
	const int32 month       = st.wMonth;
	const int32 day         = st.wDay;
	const int32 hour        = st.wHour;
	const int32 minute      = st.wMinute;
	const int32 second      = st.wSecond;
	const int32 millisecond = st.wMilliseconds;

	return FDateTime { year, month, day, hour, minute, second, millisecond };
}

int64 FWindowsTime::GetSystemTimeInTicks()
{
	if (GPerformanceCounterToMillis < 0.0)
	{
		LARGE_INTEGER performanceFrequency {};
		QueryPerformanceFrequency(&performanceFrequency);

		// NOTE Normally, inverting the performance frequency would give a multiplier for seconds. 1000x that is for milliseconds
		GPerformanceCounterToMillis = 1000.0 / static_cast<double>(performanceFrequency.QuadPart);

		LARGE_INTEGER performanceCounter {};
		QueryPerformanceCounter(&performanceCounter);

		GPerformanceCounterReference = static_cast<uint64>(performanceCounter.QuadPart);
	}

	LARGE_INTEGER currentCounter {};
	QueryPerformanceCounter(&currentCounter);

	const uint64 deltaCounter = static_cast<uint64>(currentCounter.QuadPart) - GPerformanceCounterReference;
	return FTimeSpan::FromMilliseconds(static_cast<double>(deltaCounter) * GPerformanceCounterToMillis).GetTicks();
}

FDateTime FWindowsTime::GetUtcTime()
{
	SYSTEMTIME st { };
	::GetSystemTime(&st);

	const int32 year        = st.wYear;
	const int32 month       = st.wMonth;
	const int32 day         = st.wDay;
	const int32 hour        = st.wHour;
	const int32 minute      = st.wMinute;
	const int32 second      = st.wSecond;
	const int32 millisecond = st.wMilliseconds;

	return FDateTime { year, month, day, hour, minute, second, millisecond };
}
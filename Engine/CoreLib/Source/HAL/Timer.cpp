#include "HAL/Timer.h"

FTimeSpan FTimer::GetElapsedTime() const
{
	if (IsRunning())
	{
		const FTimePoint now = FTimePoint::Now();
		return now - m_StartTimePoint;
	}

	return m_StopTimePoint - m_StartTimePoint;
}

void FTimer::Restart()
{
	m_IsRunning = true;
	m_StopTimePoint = {};
	m_StartTimePoint = FTimePoint::Now();
}

FTimer FTimer::Start()
{
	FTimer result;
	result.m_StartTimePoint = FTimePoint::Now();
	result.m_IsRunning = true;
	return result;
}

FTimeSpan FTimer::Stop()
{
	if (m_IsRunning)
	{
		m_StopTimePoint = FTimePoint::Now();
		m_IsRunning = false;
	}

	return GetElapsedTime();
}
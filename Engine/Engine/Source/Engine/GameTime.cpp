#include "Engine/Assert.h"
#include "Engine/GameTime.h"
#include "Math/Math.h"
#include <SDL2/SDL.h>

FTimeSpan FGameTime::GetDeltaTime() const
{
	return m_CurrentFrameTime - m_PreviousFrameTime;
}

float FGameTime::GetDeltaSeconds() const
{
	return static_cast<float>(GetDeltaTime().GetTotalSeconds());
}

FTimeSpan FGameTime::GetTotalTime() const
{
	return m_CurrentFrameTime - m_StartTime;
}

float FGameTime::GetTotalSeconds() const
{
	return static_cast<float>(GetTotalTime().GetTotalSeconds());
}

void FGameTime::Initialize(TBadge<class UEngineLoop>)
{
	m_FrameCount = 0;

	m_StartTime = FTimePoint::Now();
	m_CurrentFrameTime = m_StartTime;
	m_PreviousFrameTime = m_StartTime;
}

void FGameTime::Update(TBadge<class UEngineLoop>)
{
	++m_FrameCount;

	m_PreviousFrameTime = m_CurrentFrameTime;
	m_CurrentFrameTime = FTimePoint::Now();
}
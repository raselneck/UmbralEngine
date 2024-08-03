#pragma once

#include "HAL/TimePoint.h"
#include "HAL/TimeSpan.h"
#include "Misc/Badge.h"

// TODO Maybe rename to FEngineTime and then just use it to generically update engines?

/**
 * @brief Defines a representation of game time.
 */
class FGameTime final
{
	friend class UEngineLoop;

public:

	/**
	 * @brief Gets the delta time that has passed since the last frame.
	 *
	 * @return The delta time that has passed since the last frame.
	 */
	[[nodiscard]] FTimeSpan GetDeltaTime() const;

	/**
	 * @brief Gets the delta seconds that have passed since the last frame.
	 *
	 * @return The delta seconds that have passed since the last frame.
	 */
	[[nodiscard]] float GetDeltaSeconds() const;

	/**
	 * @brief Gets the total number of frames that have been run.
	 *
	 * @return The total number of frames that have been run.
	 */
	[[nodiscard]] int64 GetFrameCount() const
	{
		return m_FrameCount;
	}

	/**
	 * @brief Gets the total time that has passed since the game started.
	 *
	 * @return The total time that has passed since the game started.
	 */
	[[nodiscard]] FTimeSpan GetTotalTime() const;

	/**
	 * @brief Gets the total seconds that have passed since the game started.
	 *
	 * @return The total seconds that have passed since the game started.
	 */
	[[nodiscard]] float GetTotalSeconds() const;

private:

	/**
	 * @brief Initializes this game time.
	 */
	void Initialize(TBadge<class UEngineLoop>);

	/**
	 * @brief Updates this game time.
	 */
	void Update(TBadge<class UEngineLoop>);

	int64 m_FrameCount = 0;
	FTimePoint m_StartTime;
	FTimePoint m_PreviousFrameTime;
	FTimePoint m_CurrentFrameTime;
};
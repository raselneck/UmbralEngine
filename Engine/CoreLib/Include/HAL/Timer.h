#pragma once

#include "HAL/TimePoint.h"
#include "HAL/TimeSpan.h"

/**
 * @brief Defines a simple timer.
 */
class FTimer
{
public:

	/**
	 * @brief Gets the elapsed time since this timer was started.
	 *
	 * @return The elapsed time since this timer was started.
	 */
	[[nodiscard]] FTimeSpan GetElapsedTime() const;

	/**
	 * @brief Gets a value indicating whether or not this timer is running.
	 *
	 * @return True if this timer is running, otherwise false.
	 */
	[[nodiscard]] bool IsRunning() const
	{
		return m_IsRunning;
	}

	/**
	 * @brief Restarts this timer.
	 */
	void Restart();

	/**
	 * @brief Starts a new timer.
	 *
	 * @return The timer.
	 */
	[[nodiscard]] static FTimer Start();

	/**
	 * @brief Stops this timer and returns the elapsed time since it started.
	 *
	 * @return The elapsed time since this timer was started.
	 */
	[[nodiscard]] FTimeSpan Stop();

private:

	FTimePoint m_StartTimePoint;
	FTimePoint m_StopTimePoint;
	bool m_IsRunning = false;
};
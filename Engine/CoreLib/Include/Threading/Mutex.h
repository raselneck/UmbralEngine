#pragma once

#include "Engine/Error.h"
#include "Engine/MiscMacros.h"
#include "HAL/TimeSpan.h"

namespace Private
{
	class FMutexImpl;
}

/**
 * @brief An enumeration of possible mutex types.
 */
enum class EMutexType : uint8
{
	Plain,
	Timed,
	Recursive
};

/**
 * @brief Defines a mutex (mutually exclusive) lock.
 */
class FMutex final
{
public:

	UM_DISABLE_COPY(FMutex);
	UM_DEFAULT_MOVE(FMutex);

	/**
	 * @brief Creates a new, plain mutex.
	 */
	FMutex();

	/**
	 * @brief Creates a new mutex.
	 *
	 * @param type The type of the mutex.
	 */
	explicit FMutex(EMutexType type);

	/**
	 * @brief Checks to see if this mutex is locked.
	 *
	 * @return True if this mutex is locked, otherwise false.
	 */
	[[nodiscard]] bool IsLocked() const;

	/**
	 * @brief Attempts to lock this mutex.
	 */
	void Lock();

	/**
	 * @brief Attempts to lock this mutex. If the mutex is already locked, then this function
	 *        will return without blocking.
	 */
	void TryLock();

	/**
	 * @brief Attempts to unlock this mutex.
	 */
	void Unlock();

private:

	TUniquePtr<Private::FMutexImpl> m_Impl;
};
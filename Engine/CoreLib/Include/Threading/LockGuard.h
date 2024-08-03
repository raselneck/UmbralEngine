#pragma once

class FMutex;

/**
 * @brief Defines a scoped lock guard for a mutex.
 *
 * The mutex will be locked when the lock guard is created, and unlocked when the lock guard is destroyed.
 *
 * @remark The mutex must exist for at least as long as this lock guard.
 */
class FScopedLockGuard final
{
public:

	/**
	 * @brief Creates a scoped mutex lock guard.
	 *
	 * @param mutex The mutex to be locked and unlocked.
	 */
	explicit FScopedLockGuard(FMutex& mutex);

	/**
	 * @brief Destroys this scoped mutex lock guard, unlocking the mutex.
	 */
	~FScopedLockGuard();

private:

	FMutex& m_Mutex;
};
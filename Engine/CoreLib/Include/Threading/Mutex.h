#pragma once

#include "Engine/Error.h"
#include "HAL/TimeSpan.h"
#include "Memory/UniquePtr.h"

/**
 * @brief Defines a mutex (mutually exclusive) lock.
 */
class FMutex final
{
	class FMutexImpl;

public:

	/**
	 * @brief Transfers ownership of another mutex's resources to this mutex.
	 *
	 * @param other The other mutex.
	 */
	FMutex(FMutex&& other) noexcept;

	/**
	 * @brief Destroys this mutex.
	 */
	~FMutex();

	/**
	 * @brief Attempts to create a mutex.
	 *
	 * @return The created mutex, or the error that was encountered while creating the mutex.
	 */
	static TErrorOr<FMutex> Create();

	/**
	 * @brief Checks to see if this mutex is locked.
	 *
	 * @return True if this mutex is locked, otherwise false.
	 */
	[[nodiscard]] bool IsLocked() const;

	/**
	 * @brief Checks to see if this mutex is valid.
	 *
	 * @return True if this mutex is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Attempts to lock this mutex.
	 */
	void Lock();

	/**
	 * @brief Attempts to unlock this mutex.
	 */
	void Unlock();

	/**
	 * @brief Transfers ownership of another mutex's resources to this mutex.
	 *
	 * @param other The other mutex.
	 * @return This mutex.
	 */
	FMutex& operator=(FMutex&& other) noexcept;

	/**
	 * @brief Checks to see if this mutex is valid.
	 *
	 * @return True if this mutex is valid, otherwise false.
	 */
	explicit operator bool() const
	{
		return IsValid();
	}

private:

	/**
	 * @brief Sets default values for this mutex, which sets it up to be invalid. Use FMutex::Create() to create a valid mutex.
	 */
	FMutex();

	TUniquePtr<FMutexImpl> m_Impl;
};

template<>
struct TIsZeroConstructible<FMutex> : FTrueType
{
};
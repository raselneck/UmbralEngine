#pragma once

#include "Containers/Function.h"
#include "HAL/TimeSpan.h"
#include "Memory/UniquePtr.h"

/** The function type for running a thread without any arguments. */
using FThreadFunction = TFunction<void()>;

/** The function type for running a thread with a user-defined argument. */
using FParameterizedThreadFunction = TFunction<void(void*)>;

/**
 * @brief Defines a way to launch threads or manipulate the calling thread.
 */
class FThread final
{
	UM_DISABLE_COPY(FThread);

	class FThreadImpl;

public:

	/**
	 * @brief Moves ownership of another thread's resources.
	 *
	 * @param other The other thread.
	 */
	FThread(FThread&& other) noexcept;

	/**
	 * @brief Destroys this thread.
	 */
	~FThread();

	/**
	 * @brief Creates a new thread.
	 *
	 * @param function The function to run on the thread.
	 * @return The thread.
	 */
	static FThread Create(FThreadFunction function);

	/**
	 * @brief Creates a new thread.
	 *
	 * @param function The parameterized function to run on the thread.
	 * @param parameter The parameter to pass along to \p function.
	 * @return The thread.
	 */
	static FThread Create(FParameterizedThreadFunction function, void* parameter);

	/**
	 * @brief Checks to see if this thread is valid.
	 *
	 * @return True if this thread is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Blocks the calling thread until this thread terminates.
	 */
	void Join();

	/**
	 * @brief Sleeps the calling thread for the given duration.
	 *
	 * @param duration The duration to sleep for.
	 */
	static void Sleep(FTimeSpan duration);

	/**
	 * @brief Moves ownership of another thread's resources.
	 *
	 * @param other The other thread.
	 * @return This thread.
	 */
	FThread& operator=(FThread&& other) noexcept;

private:

	/**
	 * @brief Sets default values for this thread's properties.
	 */
	FThread();

	TUniquePtr<FThreadImpl> m_Impl;
};

template<>
struct TIsZeroConstructible<FThread> : FTrueType
{
};
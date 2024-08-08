#pragma once

#include "Containers/Function.h"
#include "HAL/TimeSpan.h"
#include "Memory/UniquePtr.h"
#include "Templates/Forward.h"
#include "Templates/Move.h"

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
	[[nodiscard]] static FThread Create(TFunction<void()> function);

	/**
	 * @brief Creates a new thread.
	 *
	 * @tparam ArgTypes The types of the arguments to pass along to the function.
	 * @param function The function to run on the thread.
	 * @param parameters The arguments to pass along to the function.
	 * @return The thread.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] static FThread Create(TFunction<void(ArgTypes...)> function, ArgTypes... parameters)
	{
		TFunction<void()> voidFunction = [function = MoveTemp(function), ...parameters = Forward<ArgTypes>(parameters)]() mutable
		{
			function(MoveTemp(parameters)...);
		};

		return Create(MoveTemp(voidFunction));
	}

	/**
	 * @brief Creates a new thread.
	 *
	 * @tparam ArgTypes The types of the arguments to pass along to the function.
	 * @param function The function to run on the thread.
	 * @param parameters The arguments to pass along to the function.
	 * @return The thread.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] static FThread Create(void (*function)(ArgTypes...), ArgTypes... parameters)
	{
		TFunction<void()> voidFunction = [function = function, ...parameters = Forward<ArgTypes>(parameters)]() mutable
		{
			function(MoveTemp(parameters)...);
		};

		return Create(MoveTemp(voidFunction));
	}

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
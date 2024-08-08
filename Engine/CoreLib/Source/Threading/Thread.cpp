#include "Engine/Logging.h"
#include "HAL/InternalTime.h"
#include "Threading/Thread.h"
#include <pthread.h>

enum class EThreadState : uint8
{
	WaitingToRun,
	Running,
	Finished,
	Joined
};

class FThread::FThreadImpl final
{
public:

	/**
	 * @brief Creates a new thread implementation.
	 *
	 * @param function The function to run.
	 */
	explicit FThreadImpl(TFunction<void()> function)
		: m_Function { MoveTemp(function) }
	{
	}

	/**
	 * @brief Destroys this thread implementation.
	 */
	~FThreadImpl()
	{
		UM_ASSERT(m_State == EThreadState::Finished || m_State == EThreadState::Joined, "Destroying thread implementation without joining");
	}

	/**
	 * @brief Attempts to join this thread.
	 */
	void Join()
	{
		if (m_State != EThreadState::Running)
		{
			return;
		}

		void* status = nullptr;
		const int32 result = pthread_join(m_ThreadHandle, &status);

		if (result == 0)
		{
			m_State = EThreadState::Joined;
			return;
		}

		switch (result)
		{
		case EINVAL:  UM_LOG(Fatal, "Attempting to join thread that cannot be joined"); break;
		case ESRCH:   UM_LOG(Fatal, "Attempting to join invalid thread"); break;
		case EDEADLK: UM_LOG(Fatal, "Attempting to join a thread from within the thread"); break;
		default:      UM_LOG(Fatal, "Unknown error encountered while joining thread"); break;
		}
	}

	/**
	 * @brief Runs this thread implementation.
	 */
	void Run()
	{
		pthread_attr_t threadAttr {};
		pthread_attr_init(&threadAttr);
		pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);

		const int32 result = pthread_create(&m_ThreadHandle, &threadAttr, RunThreadFunction, this);
		if (result == 0)
		{
			m_State = EThreadState::Running;
			return;
		}

		switch (result)
		{
		case EAGAIN: UM_LOG(Fatal, "The limit on the number of threads for this process has been reached"); break;
		case EINVAL: UM_LOG(Fatal, "Thread given invalid attributes during creation"); break;
		case EPERM:  UM_LOG(Fatal, "Insufficient permissions to create thread"); break;
		default:     UM_LOG(Fatal, "Unknown error encountered while creating thread"); break;
		}
	}

private:

	/**
	 * @brief Runs m_Function on a thread implementation.
	 *
	 * @param opaqueImpl The thread implementation.
	 * @return Nothing.
	 */
	static void* RunThreadFunction(void* opaqueImpl)
	{
		FThreadImpl* impl = reinterpret_cast<FThreadImpl*>(opaqueImpl);
		impl->m_Function.Invoke();
		impl->m_State = EThreadState::Finished;

		return nullptr;
	}

	pthread_t m_ThreadHandle {};
	TFunction<void()> m_Function;
	EThreadState m_State = EThreadState::WaitingToRun;
};

FThread::FThread() = default;

FThread::FThread(FThread&& other) noexcept
	: m_Impl { MoveTemp(other.m_Impl) }
{
	UM_ASSERT(other.m_Impl.IsNull(), "Failed to move thread resource ownership");
}

FThread::~FThread()
{
	if (m_Impl.IsValid())
	{
		m_Impl->Join();
	}
}

FThread FThread::Create(TFunction<void()> function)
{
	FThread thread;
	thread.m_Impl = MakeUnique<FThreadImpl>(MoveTemp(function));
	thread.m_Impl->Run();
	return thread;
}

bool FThread::IsValid() const
{
	return m_Impl.IsValid();
}

void FThread::Join()
{
	UM_ASSERT(m_Impl.IsValid(), "Attempting to join invalid thread");

	m_Impl->Join();
}

static struct timespec TimeSpanToTimeSpec(const FTimeSpan timeSpan)
{
	struct timespec duration {};
	duration.tv_sec = static_cast<time_t>(timeSpan.GetTotalSeconds());

	const FTimeSpan timeSpanWithoutSeconds = timeSpan - FTimeSpan::FromSeconds(static_cast<double>(duration.tv_sec));
	duration.tv_nsec = static_cast<decltype(duration.tv_nsec)>(Time::TicksToNanoseconds(timeSpanWithoutSeconds.GetTicks()));

	return duration;
}

void FThread::Sleep(const FTimeSpan duration)
{
	if (duration.GetTicks() <= 0)
	{
		return;
	}

	struct timespec delayDuration = TimeSpanToTimeSpec(duration);
	const int32 result = pthread_delay_np(&delayDuration);

	if (result == 0)
	{
		return;
	}

	switch (result)
	{
	case EINVAL: UM_LOG(Error, "Invalid sleep interval given ({} ticks)", duration.GetTicks()); break;
	default:     UM_LOG(Error, "Unknown error when attempting to sleep thread (error {})", result); break;
	}
}

FThread& FThread::operator=(FThread&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	m_Impl = MoveTemp(other.m_Impl);
	UM_ASSERT(other.m_Impl.IsNull(), "Failed to move thread resource ownership");

	return *this;
}
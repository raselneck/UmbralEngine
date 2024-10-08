#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Threading/Mutex.h"
#include <pthread.h>

enum class EMutexLockState : bool
{
	Unlocked,
	Locked
};

class FMutex::FMutexImpl final
{
public:

	/** @brief The mutex handle. */
	pthread_mutex_t MutexHandle = PTHREAD_MUTEX_INITIALIZER;

	/** @brief The mutex's lock state. */
	EMutexLockState LockState = EMutexLockState::Unlocked;

	/**
	 * @brief Creates a new mutex implementation.
	 *
	 * @param mutexHandle The mutex handle.
	 */
	explicit FMutexImpl(pthread_mutex_t&& mutexHandle)
		: MutexHandle { MoveTemp(mutexHandle) }
	{
	}

	/**
	 * @brief Destroys this mutex implementation.
	 */
	~FMutexImpl()
	{
		pthread_mutex_destroy(&MutexHandle);
	}
};

FMutex::FMutex()
{
	pthread_mutex_t mutexHandle = PTHREAD_MUTEX_INITIALIZER;
	const int32 initResult = pthread_mutex_init(&mutexHandle, nullptr);
	if (initResult == 0)
	{
		m_Impl = MakeUnique<FMutexImpl>(MoveTemp(mutexHandle));
		return;
	}

	switch (initResult)
	{
	case EAGAIN: UM_LOG(Fatal, "The system lacks the necessary resources to create another mutex");
	case ENOMEM: UM_LOG(Fatal, "Insufficient memory exists to initialize a mutex");
	case EPERM:  UM_LOG(Fatal, "The caller does not have the privilege to initialize a mutex");
	default:     UM_LOG(Fatal, "An unknown error ({}) occurred while initializing a mutex", initResult);
	}
}

FMutex::FMutex(FMutex&& other) noexcept
	: m_Impl { MoveTemp(other.m_Impl) }
{
	UM_ASSERT(other.m_Impl.IsNull(), "Failed to move mutex resource ownership");
}

FMutex::~FMutex()
{
	m_Impl.Reset();
}

bool FMutex::IsLocked() const
{
	return m_Impl.IsValid() && m_Impl->LockState == EMutexLockState::Locked;
}

bool FMutex::IsValid() const
{
	return m_Impl.IsValid();
}

void FMutex::Lock()
{
	UM_ASSERT(m_Impl.IsValid(), "Attempting to lock invalid mutex");
	UM_ASSERT(m_Impl->LockState != EMutexLockState::Locked, "Attempting to lock an already locked mutex");

	pthread_mutex_lock(&m_Impl->MutexHandle);
	m_Impl->LockState = EMutexLockState::Locked;
}

void FMutex::Unlock()
{
	UM_ASSERT(m_Impl.IsValid(), "Attempting to unlock invalid mutex");

	if (m_Impl->LockState == EMutexLockState::Unlocked)
	{
		return;
	}

	pthread_mutex_unlock(&m_Impl->MutexHandle);
	m_Impl->LockState = EMutexLockState::Unlocked;
}

FMutex& FMutex::operator=(FMutex&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	m_Impl = MoveTemp(other.m_Impl);
	UM_ASSERT(other.m_Impl.IsNull(), "Failed to move mutex resource ownership");

	return *this;
}
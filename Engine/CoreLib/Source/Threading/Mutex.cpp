#include "Engine/Assert.h"
#include "Threading/Mutex.h"
#include <pthread.h>

class FMutex::FMutexImpl final
{
public:

	/** @brief The mutex handle. */
	pthread_mutex_t MutexHandle = PTHREAD_MUTEX_INITIALIZER;

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

FMutex::FMutex() = default;

FMutex::FMutex(FMutex&& other) noexcept
	: m_Impl { MoveTemp(other.m_Impl) }
{
	UM_ASSERT(other.m_Impl.IsNull(), "Failed to move mutex resource ownership");
}

FMutex::~FMutex()
{
	m_Impl.Reset();
}

TErrorOr<FMutex> FMutex::Create()
{
	pthread_mutex_t mutexHandle = PTHREAD_MUTEX_INITIALIZER;
	const int32 initResult = pthread_mutex_init(&mutexHandle, nullptr);
	if (initResult == 0)
	{
		FMutex mutex;
		mutex.m_Impl = MakeUnique<FMutexImpl>(MoveTemp(mutexHandle));
	}

	switch (initResult)
	{
	case EAGAIN: return MAKE_ERROR("The system lacked the necessary resources to initialize another mutex");
	case ENOMEM: return MAKE_ERROR("Insufficient memory exists to initialize a mutex");
	case EPERM:  return MAKE_ERROR("The caller does not have the privilege to initialize a mutex");
	default:     return MAKE_ERROR("An unknown error occurred while initializing a mutex");
	}
}

bool FMutex::IsLocked() const
{
	return m_LockState == ELockState::Locked;
}

bool FMutex::IsValid() const
{
	return m_Impl.IsValid();
}

void FMutex::Lock()
{
	UM_ASSERT(m_Impl.IsValid(), "Attempting to lock invalid mutex");
	UM_ASSERT(m_LockState != ELockState::Locked, "Attempting to lock an already locked mutex");

	pthread_mutex_lock(&m_Impl->MutexHandle);
	m_LockState = ELockState::Locked;
}

void FMutex::Unlock()
{
	UM_ASSERT(m_Impl.IsValid(), "Attempting to unlock invalid mutex");
	UM_ASSERT(m_LockState != ELockState::Unlocked, "Attempting to unlock an already unlocked mutex");

	pthread_mutex_unlock(&m_Impl->MutexHandle);
	m_LockState = ELockState::Unlocked;
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
#include "Engine/Assert.h"
#include "Threading/Mutex.h"
#include <pthread.h>

#define GetMutexHandle() reinterpret_cast<pthread_mutex_t*>(&m_MutexHandle)

FMutex::FMutex(FMutex&& other) noexcept
	: m_MutexHandle { other.m_MutexHandle }
{
	other.m_MutexHandle = PTHREAD_MUTEX_INITIALIZER;
}

FMutex::~FMutex()
{
	Dispose();
}

TErrorOr<FMutex> FMutex::Create()
{
	FMutex mutex;

	const int32 initResult = pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(&mutex.m_MutexHandle), nullptr);
	if (initResult == 0)
	{
		return TErrorOr<FMutex> { MoveTemp(mutex) };
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
	return m_MutexHandle != PTHREAD_MUTEX_INITIALIZER;
}

void FMutex::Lock()
{
	UM_ASSERT(m_LockState != ELockState::Locked, "Attempting to lock an already locked mutex");

	pthread_mutex_lock(GetMutexHandle());
	m_LockState = ELockState::Locked;
}

void FMutex::Unlock()
{
	UM_ASSERT(m_LockState != ELockState::Unlocked, "Attempting to unlock an already unlocked mutex");

	pthread_mutex_unlock(GetMutexHandle());
	m_LockState = ELockState::Unlocked;
}

FMutex& FMutex::operator=(FMutex&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	Dispose();
	Swap(m_MutexHandle, other.m_MutexHandle);

	return *this;
}

FMutex::FMutex()
	: m_MutexHandle { PTHREAD_MUTEX_INITIALIZER }
{
}

void FMutex::Dispose()
{
	if (m_MutexHandle != PTHREAD_MUTEX_INITIALIZER)
	{
		pthread_mutex_destroy(GetMutexHandle());
		m_MutexHandle = PTHREAD_MUTEX_INITIALIZER;
	}

	m_LockState = ELockState::Unlocked;
}

#undef GetMutexHandle
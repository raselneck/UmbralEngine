#include "Engine/Assert.h"
#include "Threading/LockGuard.h"
#include "Threading/Mutex.h"

FScopedLockGuard::FScopedLockGuard(FMutex& mutex)
	: m_Mutex { mutex }
{
	UM_ASSERT(m_Mutex.IsLocked() == false, "Attempting to lock an already locked mutex");

	m_Mutex.Lock();
}

FScopedLockGuard::~FScopedLockGuard()
{
	m_Mutex.Unlock();
}
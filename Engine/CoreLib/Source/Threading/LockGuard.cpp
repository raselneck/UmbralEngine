#include "Threading/LockGuard.h"
#include "Threading/Mutex.h"

FScopedLockGuard::FScopedLockGuard(FMutex& mutex)
	: m_Mutex { mutex }
{
	m_Mutex.Lock();
}

FScopedLockGuard::~FScopedLockGuard()
{
	m_Mutex.Unlock();
}
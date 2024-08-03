#include "Threading/Thread.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "Threading/Windows/WindowsThread.h"
#else
#	include "Threading/Posix/PosixThread.h"
#endif

void FThread::Sleep(const FTimeSpan duration)
{
	FPlatformThread::Sleep(duration);
}
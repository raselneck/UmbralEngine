#include "Threading/Windows/WindowsThread.h"

void FWindowsThread::Sleep(FTimeSpan duration)
{
	// Windows nanosleep https://stackoverflow.com/a/60853407
	(void)duration;
}
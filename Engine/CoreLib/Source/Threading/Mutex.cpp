#include "Threading/Mutex.h"
#if UMBRAL_PLATFORM_IS_WINDOWS
#	include "Threading/Windows/WindowsMutex.h"
#else
#	include "Threading/Posix/PosixMutex.h"
#endif
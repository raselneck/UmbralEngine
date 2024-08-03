#include "Engine/Logging.h"
#include "HAL/InternalTime.h"
#include "Threading/Posix/PosixThread.h"
#include <ctime>

static struct timespec TimeSpanToTimeSpec(const FTimeSpan timeSpan)
{
	struct timespec duration {};
	duration.tv_sec = static_cast<time_t>(timeSpan.GetTotalSeconds());

	const FTimeSpan timeSpanWithoutSeconds = timeSpan - FTimeSpan::FromSeconds(static_cast<double>(duration.tv_sec));
	duration.tv_nsec = Time::TicksToNanoseconds(timeSpanWithoutSeconds.GetTicks());

	return duration;
}

void FPosixThread::Sleep(const FTimeSpan duration)
{
	struct timespec requestedDuration = TimeSpanToTimeSpec(duration);
	struct timespec remainingDuration {};
	if (::nanosleep(&requestedDuration, &remainingDuration) == 0)
	{
		return;
	}

	// TODO Move the file system errno string handling to FError and use that here

	UM_LOG(Error, "nanosleep interrupted with {}s {}ns remaining", remainingDuration.tv_sec, remainingDuration.tv_nsec);
}
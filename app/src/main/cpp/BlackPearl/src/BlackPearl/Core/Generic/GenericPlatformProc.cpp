//
// Created by DXT00 on 2025/4/13.
//
#include "pch.h"
#include "Core/Generic/GenericPlatformProc.h"

#if PLATFORM_HAS_BSD_TIME
#include <unistd.h>

void FGenericPlatformProc::Sleep( float Seconds )
{
//	SCOPE_CYCLE_COUNTER(STAT_Sleep);
//	FThreadIdleStats::FScopeIdle Scope;
	SleepNoStats(Seconds);
}

void FGenericPlatformProc::SleepNoStats( float Seconds )
{
	const int32_t usec = int(Seconds * 1000000.0f);
	if (usec > 0)
	{
		usleep(usec);
	}
	else
	{
		sched_yield();
	}
}

void FGenericPlatformProc::SleepInfinite()
{
	// stop this thread forever
	while (true)
	{
		pause();
	}
}

void FGenericPlatformProc::YieldThread()
{
	sched_yield();
}

#endif // PLATFORM_HAS_BSD_TIME
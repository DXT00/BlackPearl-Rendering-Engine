//
// Created by DXT00 on 2025/4/13.
//

#ifndef BLACKPEARL_ENGINE_GENERICPLATFORM_H
#define BLACKPEARL_ENGINE_GENERICPLATFORM_H
#include "../Platform.h"
class FGenericPlatformProc {

public:

#if PLATFORM_HAS_BSD_TIME

    /** Sleep this thread for Seconds.  0.0 means release the current time slice to let other threads get some attention. Uses stats.*/
	static void Sleep( float Seconds );
	/** Sleep this thread for Seconds.  0.0 means release the current time slice to let other threads get some attention. */
	static void SleepNoStats( float Seconds );
	/** Sleep this thread infinitely. */
	[[noreturn]] static  void SleepInfinite();
	/** Yield this thread so another may run for a while. */
	static void YieldThread();

#endif // PLATFORM_HAS_BSD_TIME

};

#endif //BLACKPEARL_ENGINE_GENERICPLATFORM_H

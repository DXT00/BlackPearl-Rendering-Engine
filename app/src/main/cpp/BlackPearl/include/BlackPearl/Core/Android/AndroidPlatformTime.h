//
// Created by DXT00 on 2025/4/14.
//

#pragma once

// Copyright Epic Games, Inc. All Rights Reserved.


/*=============================================================================================
	AndroidTime.h: Android platform Time functions
==============================================================================================*/

#pragma once
#include "BlackPearl/Core.h"
#include "../Platform.h"
#include <sys/time.h>

//@todo android: this entire file

/**
 * Android implementation of the Time OS functions
 */
struct FAndroidTime
{
    // android uses BSD time code from GenericPlatformTime
    static FORCEINLINE double Seconds()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ((double) ts.tv_sec) + (((double) ts.tv_nsec) / 1000000000.0);
    }

    static FORCEINLINE uint32_t Cycles()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint32_t) ((((uint64_t)ts.tv_sec) * 1000000ULL) + (((uint64_t)ts.tv_nsec) / 1000ULL));
    }

    static FORCEINLINE uint64_t Cycles64()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ((((uint64_t)ts.tv_sec) * 1000000ULL) + (((uint64_t)ts.tv_nsec) / 1000ULL));
    }
};

typedef FAndroidTime FPlatformTime;

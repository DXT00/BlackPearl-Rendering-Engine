//
// Created by DXT00 on 2025/4/15.
//

#pragma once
#include "stdint.h"
struct GenericPlatformMisc {

    /**
 * retrieves the current maximum refresh rate supported by the platform
 */
    static inline int32_t GetMaxRefreshRate()
    {
        return 60;
    }

/**
 * retrieves the maximum refresh rate supported by the platform
 */
    static inline int32_t GetMaxSupportedRefreshRate()
    {
        return GetMaxRefreshRate();
    }

/**
 * Returns the platform's maximum allowed value for rhi.SyncInterval
 */
    static inline int32_t GetMaxSyncInterval()
    {
        // Generic platform has no limit.
        return INT32_MAX;
    }
};

	
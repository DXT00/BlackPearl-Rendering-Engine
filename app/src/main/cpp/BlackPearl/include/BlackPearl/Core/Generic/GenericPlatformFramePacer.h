//
// Created by DXT00 on 2025/4/15.
//

#pragma once
#include "stdint.h"
namespace BlackPearl{

/**
 * Generic implementation for most platforms
 **/
    struct GenericPlatformRHIFramePacer
    {
        /**
         * The pace we are running at (30 = 30fps, 0 = unpaced)
         * The generic implementation returns a result based on rhi.SyncInterval and FPlatformMisc::GetMaxRefreshRate().
         */
        static inline int32_t GetFramePace()
        {
            return GetFramePaceFromSyncInterval();
        }

        /**
         * Sets the pace we would like to running at (30 = 30fps, 0 = unpaced).
         * The generic implementation sets the value for rhi.SyncInterval according to FPlatformMisc::GetMaxRefreshRate().
         *
         * @return the pace we will run at.
         */
        static inline int32_t SetFramePace(int32_t FramePace)
        {
            return SetFramePaceToSyncInterval(FramePace);
        }

        /**
         * Returns whether the hardware is able to frame pace at the specified frame rate
         */
        static  bool SupportsFramePace(int32_t QueryFramePace);

    protected:
        /**
         * The generic implementation returns a result based on rhi.SyncInterval and FPlatformMisc::GetMaxRefreshRate().
         */
        static  int32_t GetFramePaceFromSyncInterval();

        /**
         * The generic implementation sets rhi.SyncInterval based on FPlatformMisc::GetMaxRefreshRate().
         *
         * @return the pace we will run at.
         */
        static  int32_t SetFramePaceToSyncInterval(int32_t FramePace);
    };

}




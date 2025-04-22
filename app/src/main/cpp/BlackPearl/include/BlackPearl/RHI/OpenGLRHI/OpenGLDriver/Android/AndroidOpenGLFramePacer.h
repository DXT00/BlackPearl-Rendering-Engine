//
// Created by DXT00 on 2025/4/15.
//

#pragma once
#include "BlackPearl/Core/PlatformFramePacer.h"

namespace BlackPearl{
    class AndroidOpenGLFramePacer : public IAndroidFramePacer
    {
    public:
        bool SwapBuffers(bool bLockToVsync) override;
        bool SwapBuffersPacer(bool bLockToVsync) ;

        virtual void Init() override;
        virtual bool SupportsFramePace(int32_t QueryFramePace) override;

    private:

        // legacy framepacer state
        int32_t DesiredSyncIntervalRelativeTo60Hz = -1;
        int32_t DesiredSyncIntervalRelativeToDevice = -1;
        int32_t DriverSyncIntervalRelativeToDevice = -1;
        float DriverRefreshRate = 60.0f;
        int64_t DriverRefreshNanos = 16666666;
        double LastTimeEmulatedSync = -1.0;
        uint32_t SwapBufferFailureCount = 0;
    };

}



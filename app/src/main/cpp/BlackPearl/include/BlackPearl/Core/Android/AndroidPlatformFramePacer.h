//
// Created by DXT00 on 2025/4/15.
//
#pragma once
#include "stdint.h"
namespace BlackPearl{


struct IAndroidFramePacer
{
    virtual void Init() = 0;
    virtual ~IAndroidFramePacer() {}
    virtual bool SwapBuffers(bool bLockToVsync) { return true; }
    virtual bool SupportsFramePace(int32_t QueryFramePace) = 0;
};

struct FAndroidPlatformRHIFramePacer
{
    // FGenericPlatformRHIFramePacer interface
    static bool IsEnabled();
    static void Destroy();
    static int32_t GetFramePace();
    static int32_t SetFramePace(int32_t FramePace);
    static bool SupportsFramePace(int32_t QueryFramePace)
    { return (FramePacer) ? FramePacer->SupportsFramePace(QueryFramePace) : false; }

    // FAndroidPlatformRHIFramePacer interface
    static void Init(IAndroidFramePacer* InFramePacer);
    static int32_t GetLegacySyncInterval();
    static void SwapBuffers(bool bLockToVsync) { if (FramePacer) { FramePacer->SwapBuffers(bLockToVsync); } }


private:
    /** The actual GL or Vulkan frame pacer */
    static IAndroidFramePacer* FramePacer;
    friend struct FAndroidOpenGLFramePacer;
 //   friend struct FAndroidVulkanFramePacer;

    /* Actual current frame pace */
    static int32_t InternalFramePace;
};

typedef FAndroidPlatformRHIFramePacer FPlatformRHIFramePacer;

}
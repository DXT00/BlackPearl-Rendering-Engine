//
// Created by DXT00 on 2025/4/15.
//

#include "Core/Android/AndroidPlatformFramePacer.h"
#include "Core/Android/AndroidPlatformMisc.h"
#include "BlackPearl/RHI/RHICommandList.h"
namespace BlackPearl
{
    
    

IAndroidFramePacer* FAndroidPlatformRHIFramePacer::FramePacer = nullptr;
int32_t FAndroidPlatformRHIFramePacer::InternalFramePace = 0;




void FAndroidPlatformRHIFramePacer::Init(IAndroidFramePacer* InFramePacer)
{
    std::vector<int32_t> RefreshRates = FAndroidMisc::GetSupportedNativeDisplayRefreshRates();
    std::string RefreshRatesString;
    for (int32_t Rate : RefreshRates)
    {
        RefreshRatesString += std::to_string(Rate);
    }
    GE_CORE_INFO("Device supports the following refresh rates {}" , RefreshRatesString);
    FramePacer = InFramePacer;
    FramePacer->Init();
}

bool FAndroidPlatformRHIFramePacer::IsEnabled()
{
    return FramePacer != nullptr;
}

int32_t FAndroidPlatformRHIFramePacer::GetFramePace()
{
    if (InternalFramePace == 0)
    {
        // Internal frame pacing FPS is not set, fall back to generic framepacer based on RHI.SyncInterval
        int32_t SyncIntervalFramePace = Configuration::SyncInterval;
        InternalFramePace = SyncIntervalFramePace;
    }

    return InternalFramePace;
}

int32_t FAndroidPlatformRHIFramePacer::SetFramePace(int32_t InFramePace)
{
    int32_t NewFramePace = SupportsFramePace(InFramePace) ? InFramePace : 0;

    // Call generic framepacer to update rhi.SyncInterval where possible
   // FGenericPlatformRHIFramePacer::SetFramePaceToSyncInterval(InFramePace);
    Configuration::SyncInterval = InFramePace;
    /* Update cvar if necessary */
    if (InternalFramePace != NewFramePace)
    {
        InternalFramePace = NewFramePace;
    }

    return NewFramePace;
}

int32_t FAndroidPlatformRHIFramePacer::GetLegacySyncInterval()
{
    return RHIGetSyncInterval();
}

void FAndroidPlatformRHIFramePacer::Destroy()
{
    if (FramePacer != nullptr)
    {
        delete FramePacer;
        FramePacer = nullptr;
    }
}
}
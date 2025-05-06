//
// Created by DXT00 on 2025/4/15.
//
#include "pch.h"

#include "Core/Generic/GenericPlatformFramePacer.h"
#include "BlackPearl/Config.h"
#include "Core/PlatformMisc.h"
#include "BlackPearl/Math/basics.h"
namespace BlackPearl{

    int32_t GenericPlatformRHIFramePacer::GetFramePaceFromSyncInterval()
    {
        int32_t SyncInterval = Configuration::SyncInterval;


        if (SyncInterval <= 0)
            return 0;

        return FPlatformMisc::GetMaxRefreshRate() / math::clamp(SyncInterval, 1, FPlatformMisc::GetMaxSyncInterval());
    }

    bool GenericPlatformRHIFramePacer::SupportsFramePace(int32_t QueryFramePace)
    {
        if (QueryFramePace < 0)
            return false;

        if (QueryFramePace == 0)
            return true; // No Vsync

        if (FPlatformMisc::GetMaxRefreshRate() % QueryFramePace != 0)
            return false; // Must be a multiple

        int32_t TargetSyncInterval = FPlatformMisc::GetMaxRefreshRate() / QueryFramePace;
        return TargetSyncInterval <= FPlatformMisc::GetMaxSyncInterval();
    }

    int32_t GenericPlatformRHIFramePacer::SetFramePaceToSyncInterval(int32_t InFramePace)
    {
       // static IConsoleVariable* SyncIntervalCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("rhi.SyncInterval"));
        if (SupportsFramePace(InFramePace))
        {
            int32_t NewSyncInterval = InFramePace > 0
                                    ? math::clamp(FPlatformMisc::GetMaxRefreshRate() / InFramePace, (int32_t)1, FPlatformMisc::GetMaxSyncInterval())
                                    : 0;

           // SyncIntervalCVar->Set(NewSyncInterval, ECVF_SetByCode);
           Configuration::SyncInterval = NewSyncInterval;
        }

        return GetFramePace();
    }

}
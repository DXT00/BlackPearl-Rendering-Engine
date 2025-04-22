//
// Created by DXT00 on 2025/4/15.
//

#include "Android/AndroidOpenGLFramePacer.h"
#include "OpenGLDrvPrivate.h"
#include "BlackPearl/Core/PlatformProc.h"
#include "BlackPearl/Core/PlatformTime.h"
namespace BlackPearl
{

    extern float AndroidThunkCpp_GetMetaDataFloat(const std::string& Key);
        bool AndroidOpenGLFramePacer::SwapBuffers(bool bLockToVsync) {

            EGLDisplay eglDisplay = AndroidEGL::GetInstance()->GetDisplay();
            EGLSurface eglSurface = AndroidEGL::GetInstance()->GetSurface();
            int32_t SyncInterval = FAndroidPlatformRHIFramePacer::GetLegacySyncInterval();
            eglSwapInterval(eglDisplay, SyncInterval);
            eglSwapBuffers(eglDisplay, eglSurface);
        }

    static bool GGetTimeStampsSucceededThisFrame = true;
    static uint32_t GGetTimeStampsRetryCount = 0;

    bool ShouldUseGPUFencesToLimitLatency()
    {
//        if (CanUseGetFrameTimestampsForThisFrame())
//        {
//            return true; // this method requires a GPU fence to give steady results
//        }
//        return FAndroidPlatformRHIFramePacer::CVarDisableOpenGLGPUSync.GetValueOnAnyThread() == 0; // otherwise just based on the FAndroidPlatformRHIFramePacer::CVar; thought to be bad to use GPU fences on PowerVR
//
        return false;
    }

    bool AndroidOpenGLFramePacer::SwapBuffersPacer(bool bLockToVsync) {


//        VERIFY_EGL_SCOPE();
//
//        EGLDisplay eglDisplay = AndroidEGL::GetInstance()->GetDisplay();
//        EGLSurface eglSurface = AndroidEGL::GetInstance()->GetSurface();
//        int32_t SyncInterval = FAndroidPlatformRHIFramePacer::GetLegacySyncInterval();
//
//        bool bPrintMethod = false;
//
//        {
//            if (DesiredSyncIntervalRelativeTo60Hz != SyncInterval)
//            {
//                GGetTimeStampsRetryCount = 0;
//
//                bPrintMethod = true;
//                DesiredSyncIntervalRelativeTo60Hz = SyncInterval;
//                DriverRefreshRate = 60.0f;
//                DriverRefreshNanos = 16666666;
//
//
//                EGLnsecsANDROID EGL_COMPOSITE_DEADLINE_ANDROID_Value = -1;
//                EGLnsecsANDROID EGL_COMPOSITE_INTERVAL_ANDROID_Value = -1;
//                EGLnsecsANDROID EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID_Value = -1;
//
//                if (eglGetCompositorTimingANDROID_p)
//                {
//                    {
//                        EGLint Item = EGL_COMPOSITE_DEADLINE_ANDROID;
//                        if (!eglGetCompositorTimingANDROID_p(eglDisplay, eglSurface, 1, &Item, &EGL_COMPOSITE_DEADLINE_ANDROID_Value))
//                        {
//                            EGL_COMPOSITE_DEADLINE_ANDROID_Value = -1;
//                        }
//                    }
//                    {
//                        EGLint Item = EGL_COMPOSITE_INTERVAL_ANDROID;
//                        if (!eglGetCompositorTimingANDROID_p(eglDisplay, eglSurface, 1, &Item, &EGL_COMPOSITE_INTERVAL_ANDROID_Value))
//                        {
//                            EGL_COMPOSITE_INTERVAL_ANDROID_Value = -1;
//                        }
//                    }
//                    {
//                        EGLint Item = EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID;
//                        if (!eglGetCompositorTimingANDROID_p(eglDisplay, eglSurface, 1, &Item, &EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID_Value))
//                        {
//                            EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID_Value = -1;
//                        }
//                    }
////                    UE_LOG(LogRHI, Log, ("AndroidEGL:SwapBuffers eglGetCompositorTimingANDROID EGL_COMPOSITE_DEADLINE_ANDROID=%lld, EGL_COMPOSITE_INTERVAL_ANDROID=%lld, EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID=%lld"),
////                           EGL_COMPOSITE_DEADLINE_ANDROID_Value,
////                           EGL_COMPOSITE_INTERVAL_ANDROID_Value,
////                           EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID_Value
//  //                  );
//                }
//
//                float RefreshRate = 60;//AndroidThunkCpp_GetMetaDataFloat(("unreal.display.getRefreshRate"));
//
//                GE_CORE_INFO("JNI Display getRefreshRate={:f}",
//                       RefreshRate
//                );
//
//                if (EGL_COMPOSITE_INTERVAL_ANDROID_Value >= 4000000 && EGL_COMPOSITE_INTERVAL_ANDROID_Value <= 41666666)
//                {
//                    DriverRefreshRate = float(1000000000.0 / double(EGL_COMPOSITE_INTERVAL_ANDROID_Value));
//                    DriverRefreshNanos = EGL_COMPOSITE_INTERVAL_ANDROID_Value;
//                }
//                else if (RefreshRate >= 24.0f && RefreshRate <= 250.0f)
//                {
//                    DriverRefreshRate = RefreshRate;
//                    DriverRefreshNanos = int64_t(0.5 + 1000000000.0 / double(RefreshRate));
//                }
//
////                UE_LOG(LogRHI, Log, ("Final display timing metrics: DriverRefreshRate=%7.4f  DriverRefreshNanos=%lld"),
////                       DriverRefreshRate,
////                       DriverRefreshNanos
//                );
//
//                // make sure requested interval is in supported range
//                EGLint MinSwapInterval, MaxSwapInterval;
//                AndroidEGL::GetInstance()->GetSwapIntervalRange(MinSwapInterval, MaxSwapInterval);
//
//                int64_t SyncIntervalNanos = (30 + 1000000000l * int64_t(SyncInterval)) / 60;
//
//                int32_t UnderDriverInterval = int32_t(SyncIntervalNanos / DriverRefreshNanos);
//                int32_t OverDriverInterval = UnderDriverInterval + 1;
//
//                int64_t UnderNanos = int64_t(UnderDriverInterval) * DriverRefreshNanos;
//                int64_t OverNanos = int64_t(OverDriverInterval) * DriverRefreshNanos;
//
//                DesiredSyncIntervalRelativeToDevice = (math::abs(SyncIntervalNanos - UnderNanos) < math::abs(SyncIntervalNanos - OverNanos)) ?
//                                                      UnderDriverInterval : OverDriverInterval;
//
//                int32_t DesiredDriverSyncInterval = math::clamp<int32_t>(DesiredSyncIntervalRelativeToDevice, MinSwapInterval, MaxSwapInterval);
//
//                //UE_LOG(LogRHI, Log, ("AndroidEGL:SwapBuffers Min=%d, Max=%d, Request=%d, ClosestDriver=%d, SetDriver=%d"), MinSwapInterval, MaxSwapInterval, DesiredSyncIntervalRelativeTo60Hz, DesiredSyncIntervalRelativeToDevice, DesiredDriverSyncInterval);
//
//                if (DesiredDriverSyncInterval != DriverSyncIntervalRelativeToDevice)
//                {
//                    DriverSyncIntervalRelativeToDevice = DesiredDriverSyncInterval;
//                   // UE_LOG(LogRHI, Log, ("Called eglSwapInterval %d"), DesiredDriverSyncInterval);
//                    eglSwapInterval(eglDisplay, DriverSyncIntervalRelativeToDevice);
//                }
//            }
//
//            if (DesiredSyncIntervalRelativeToDevice > DriverSyncIntervalRelativeToDevice)
//            {
//                {
//                  //  UE_CLOG(bPrintMethod, LogRHI, Display, ("Using niave method for frame pacing (possible with timestamps method)"));
//                    if (LastTimeEmulatedSync > 0.0)
//                    {
//                       // QUICK_SCOPE_CYCLE_COUNTER(STAT_StallForEmulatedSyncInterval);
//                        float MinTimeBetweenFrames = (float(DesiredSyncIntervalRelativeToDevice) / DriverRefreshRate);
//
//                        for (;;)
//                        {
//                            float ThisTime = FPlatformTime::Seconds() - LastTimeEmulatedSync;
//                            // sleep only when there is substantial time left to a next sync interval
//                            // for a small duration rely on eglSwapBuffers
//                            if (ThisTime > 0.001f && ThisTime < MinTimeBetweenFrames)
//                            {
//                                // do not sleep for too long, poll occlussion queries from time to time as RT might be waiting for them
//                                float SleepDuration = math::min(MinTimeBetweenFrames - ThisTime, 0.003f);
//                                FPlatformProc::Sleep(SleepDuration);
//                            }
//                            else
//                            {
//                                break;
//                            }
//
//                           // GetDynamicRHI<FOpenGLDynamicRHI>()->RHIPollOcclusionQueries();
//                        }
//                    }
//                }
//            }
//            if (CanUseGetFrameTimestamps())
//            {
//                UE_CLOG(bPrintMethod, LogRHI, Display, ("Using eglGetFrameTimestampsANDROID method for frame pacing"));
//
//                //static bool bPrintOnce = true;
//                if (FrameIDs[(int32_t(NextFrameIDSlot) - 1) % NUM_FRAMES_TO_MONITOR])
//                    // not supported   && eglGetFrameTimestampsSupportedANDROID_p && eglGetFrameTimestampsSupportedANDROID_p(eglDisplay, eglSurface, EGL_FIRST_COMPOSITION_START_TIME_ANDROID))
//                {
//                    //UE_CLOG(bPrintOnce, LogRHI, Log, ("eglGetFrameTimestampsSupportedANDROID retured true for EGL_FIRST_COMPOSITION_START_TIME_ANDROID"));
//                    EGLint TimestampList = EGL_FIRST_COMPOSITION_START_TIME_ANDROID;
//                    //EGLint TimestampList = EGL_COMPOSITION_LATCH_TIME_ANDROID;
//                    //EGLint TimestampList = EGL_LAST_COMPOSITION_START_TIME_ANDROID;
//                    //EGLint TimestampList = EGL_DISPLAY_PRESENT_TIME_ANDROID;
//                    EGLnsecsANDROID Result = 0;
//                    int32_t DeltaFrameIndex = 1;
//                    for (int32_t Index = int32_t(NextFrameIDSlot) - 1; Index >= int32_t(NextFrameIDSlot) - NUM_FRAMES_TO_MONITOR && Index >= 0; Index--)
//                    {
//                        Result = 0;
//                        if (FrameIDs[Index % NUM_FRAMES_TO_MONITOR])
//                        {
//                            eglGetFrameTimestampsANDROID_p(eglDisplay, eglSurface, FrameIDs[Index % NUM_FRAMES_TO_MONITOR], 1, &TimestampList, &Result);
//                        }
//                        if (Result > 0)
//                        {
//                            break;
//                        }
//                        DeltaFrameIndex++;
//                    }
//
//                    GGetTimeStampsSucceededThisFrame = Result > 0;
//                    if (GGetTimeStampsSucceededThisFrame)
//                    {
//                        EGLnsecsANDROID FudgeFactor = 0; //  8333 * 1000;
//                        EGLnsecsANDROID DeltaNanos = EGLnsecsANDROID(DesiredSyncIntervalRelativeToDevice) * EGLnsecsANDROID(DeltaFrameIndex) * DriverRefreshNanos;
//                        EGLnsecsANDROID PresentationTime = Result + DeltaNanos + FudgeFactor;
//                        eglPresentationTimeANDROID_p(eglDisplay, eglSurface, PresentationTime);
//                        GGetTimeStampsRetryCount = 0;
//                    }
//                    else
//                    {
//                        GGetTimeStampsRetryCount++;
//                        if (GGetTimeStampsRetryCount == FAndroidPlatformRHIFramePacer::CVarTimeStampErrorRetryCount.GetValueOnAnyThread())
//                        {
//                            UE_LOG(LogRHI, Log, ("eglGetFrameTimestampsANDROID_p failed for %d consecutive frames, reverting to naive frame pacer."), GGetTimeStampsRetryCount);
//                        }
//                    }
//                }
//                else
//                {
//                    //UE_CLOG(bPrintOnce, LogRHI, Log, ("eglGetFrameTimestampsSupportedANDROID doesn't exist or retured false for EGL_FIRST_COMPOSITION_START_TIME_ANDROID, discarding eglGetNextFrameIdANDROID_p and eglGetFrameTimestampsANDROID_p"));
//                }
//                //bPrintOnce = false;
//            }
//
//            LastTimeEmulatedSync = FPlatformTime::Seconds();
//
//            {
//                //QUICK_SCOPE_CYCLE_COUNTER(STAT_eglSwapBuffers);
//
//                FrameIDs[(NextFrameIDSlot) % NUM_FRAMES_TO_MONITOR] = 0;
//                if (eglGetNextFrameIdANDROID_p && (CanUseGetFrameTimestamps() || FAndroidPlatformRHIFramePacer::CVarSpewGetFrameTimestamps.GetValueOnAnyThread()))
//                {
//                    eglGetNextFrameIdANDROID_p(eglDisplay, eglSurface, &FrameIDs[(NextFrameIDSlot) % NUM_FRAMES_TO_MONITOR]);
//                }
//                NextFrameIDSlot++;
//
//                if (eglSurface == NULL || !eglSwapBuffers(eglDisplay, eglSurface))
//                {
//                    // shutdown if swapbuffering goes down
//                    if (SwapBufferFailureCount > 10)
//                    {
//                        //Process.killProcess(Process.myPid());		//@todo android
//                    }
//                    SwapBufferFailureCount++;
//
//                    // basic reporting
//                    if (eglSurface == NULL)
//                    {
//                        return false;
//                    }
//                    else
//                    {
//                        if (eglGetError() == EGL_CONTEXT_LOST)
//                        {
//                            //Logger.LogOut("swapBuffers: EGL11.EGL_CONTEXT_LOST err: " + eglGetError());
//                            //Process.killProcess(Process.myPid());		//@todo android
//                        }
//                    }
//
//                    return false;
//                }
//            }
//
//            if (DesiredSyncIntervalRelativeToDevice > 0 && eglGetFrameTimestampsANDROID_p && FAndroidPlatformRHIFramePacer::CVarSpewGetFrameTimestamps.GetValueOnAnyThread())
//            {
//                static EGLint TimestampList[9] =
//                        {
//                                EGL_REQUESTED_PRESENT_TIME_ANDROID,
//                                EGL_RENDERING_COMPLETE_TIME_ANDROID,
//                                EGL_COMPOSITION_LATCH_TIME_ANDROID,
//                                EGL_FIRST_COMPOSITION_START_TIME_ANDROID,
//                                EGL_LAST_COMPOSITION_START_TIME_ANDROID,
//                                EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID,
//                                EGL_DISPLAY_PRESENT_TIME_ANDROID,
//                                EGL_DEQUEUE_READY_TIME_ANDROID,
//                                EGL_READS_DONE_TIME_ANDROID
//                        };
//
//                static const TCHAR* TimestampStrings[9] =
//                        {
//                                ("EGL_REQUESTED_PRESENT_TIME_ANDROID"),
//                                ("EGL_RENDERING_COMPLETE_TIME_ANDROID"),
//                                ("EGL_COMPOSITION_LATCH_TIME_ANDROID"),
//                                ("EGL_FIRST_COMPOSITION_START_TIME_ANDROID"),
//                                ("EGL_LAST_COMPOSITION_START_TIME_ANDROID"),
//                                ("EGL_FIRST_COMPOSITION_GPU_FINISHED_TIME_ANDROID"),
//                                ("EGL_DISPLAY_PRESENT_TIME_ANDROID"),
//                                ("EGL_DEQUEUE_READY_TIME_ANDROID"),
//                                ("EGL_READS_DONE_TIME_ANDROID")
//                        };
//
//
//                EGLnsecsANDROID Results[NUM_FRAMES_TO_MONITOR][9] = { {0} };
//                EGLnsecsANDROID FirstRealValue = 0;
//                for (int32_t Index = int32_t(NextFrameIDSlot) - NUM_FRAMES_TO_MONITOR; Index < int32_t(NextFrameIDSlot); Index++)
//                {
//                    eglGetFrameTimestampsANDROID_p(eglDisplay, eglSurface, FrameIDs[Index % NUM_FRAMES_TO_MONITOR], 9, TimestampList, Results[Index % NUM_FRAMES_TO_MONITOR]);
//                    for (int32_t IndexInner = 0; IndexInner < 9; IndexInner++)
//                    {
//                        if (!FirstRealValue || (Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner] > 1 && Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner] < FirstRealValue))
//                        {
//                            FirstRealValue = Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner];
//                        }
//                    }
//                }
//                UE_CLOG(FAndroidPlatformRHIFramePacer::CVarSpewGetFrameTimestamps.GetValueOnAnyThread() > 1, LogRHI, Log, ("************************************  frame %d   base time is %lld"), NextFrameIDSlot - 1, FirstRealValue);
//
//                for (int32_t Index = int32_t(NextFrameIDSlot) - NUM_FRAMES_TO_MONITOR; Index < int32_t(NextFrameIDSlot); Index++)
//                {
//
//                    UE_CLOG(FAndroidPlatformRHIFramePacer::CVarSpewGetFrameTimestamps.GetValueOnAnyThread() > 1, LogRHI, Log, ("eglGetFrameTimestampsANDROID_p  frame %d"), Index);
//                    for (int32_t IndexInner = 0; IndexInner < 9; IndexInner++)
//                    {
//                        int32_t MsVal = (Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner] > 1) ? int32_t((Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner] - FirstRealValue) / 1000000) : int32_t(Results[Index % NUM_FRAMES_TO_MONITOR][IndexInner]);
//
//                        UE_CLOG(FAndroidPlatformRHIFramePacer::CVarSpewGetFrameTimestamps.GetValueOnAnyThread() > 1, LogRHI, Log, ("     %8d    %s"), MsVal, TimestampStrings[IndexInner]);
//                    }
//                }
//
//                int32_t IndexLast = int32_t(NextFrameIDSlot) - NUM_FRAMES_TO_MONITOR;
//                int32_t IndexLastNext = IndexLast + 1;
//
//                if (Results[IndexLast % NUM_FRAMES_TO_MONITOR][3] > 1 && Results[IndexLastNext % NUM_FRAMES_TO_MONITOR][3] > 1)
//                {
//                    int32_t MsVal = int32_t((Results[IndexLastNext % NUM_FRAMES_TO_MONITOR][3] - Results[IndexLast % NUM_FRAMES_TO_MONITOR][3]) / 1000000);
//
//                    RecordedFrameInterval[NumRecordedFrameInterval++] = MsVal;
//                    if (NumRecordedFrameInterval == 100)
//                    {
//                        FString All;
//                        int32_t NumOnTarget = 0;
//                        int32_t NumBelowTarget = 0;
//                        int32_t NumAboveTarget = 0;
//                        for (int32_t Index = 0; Index < 100; Index++)
//                        {
//                            if (Index)
//                            {
//                                All += TCHAR(' ');
//                            }
//                            All += FString::Printf(("%d"), RecordedFrameInterval[Index]);
//
//                            if (RecordedFrameInterval[Index] > DesiredSyncIntervalRelativeTo60Hz * 16 - 8 && RecordedFrameInterval[Index] < DesiredSyncIntervalRelativeTo60Hz * 16 + 8)
//                            {
//                                NumOnTarget++;
//                            }
//                            else if (RecordedFrameInterval[Index] < DesiredSyncIntervalRelativeTo60Hz * 16)
//                            {
//                                NumBelowTarget++;
//                            }
//                            else
//                            {
//                                NumAboveTarget++;
//                            }
//                        }
//                        UE_LOG(LogRHI, Log, ("%3d fast  %3d ok  %3d slow   %s"), NumBelowTarget, NumOnTarget, NumAboveTarget, *All);
//                        NumRecordedFrameInterval = 0;
//                    }
//                }
//            }
//        }

        return true;
    }

         void AndroidOpenGLFramePacer::Init() {

         }
         bool AndroidOpenGLFramePacer::SupportsFramePace(int32_t QueryFramePace) {
            return true;

         }

}

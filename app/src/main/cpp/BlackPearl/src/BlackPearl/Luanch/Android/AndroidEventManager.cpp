//
// Created by DXT00 on 2025/4/13.
//
#include "pch.h"

#include "Luanch/Android/AndroidEventManager.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Core/Window.h"
#include <queue>
#if USE_ANDROID_EVENTS

#include "BlackPearl/ApplicationCore/Android/AndroidApplication.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/Core/PlatformTime.h"
#include "BlackPearl/Core/PlatformProc.h"

//#include "Android/AndroidApplication.h"
//#include "AudioDevice.h"
//#include "Misc/CallbackDevice.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
//#include "IHeadMountedDisplay.h"
//#include "IXRTrackingSystem.h"
//#include "RenderingThread.h"
//#include "UnrealEngine.h"

namespace BlackPearl
{

FAppEventManager* FAppEventManager::sInstance = NULL;


FAppEventData::FAppEventData(ANativeWindow *WindowIn) {
    GE_ASSERT(WindowIn);
    WindowWidth = ANativeWindow_getWidth(WindowIn);
    WindowHeight = ANativeWindow_getHeight(WindowIn);
    GE_ASSERT(WindowWidth >= 0 && WindowHeight >= 0);
}



FAppEventManager* FAppEventManager::GetInstance()
{
    if(!sInstance)
    {
        sInstance = new FAppEventManager();
    }

    return sInstance;
}

static const char* GetAppEventName(EAppEventState State)
{
    const char* Names[] = {
            ("APP_EVENT_STATE_WINDOW_CREATED"),
            ("APP_EVENT_STATE_WINDOW_RESIZED"),
            ("APP_EVENT_STATE_WINDOW_CHANGED"),
            ("APP_EVENT_STATE_WINDOW_DESTROYED"),
            ("APP_EVENT_STATE_WINDOW_REDRAW_NEEDED"),
            ("APP_EVENT_STATE_ON_DESTROY"),
            ("APP_EVENT_STATE_ON_PAUSE"),
            ("APP_EVENT_STATE_ON_RESUME"),
            ("APP_EVENT_STATE_ON_STOP"),
            ("APP_EVENT_STATE_ON_START"),
            ("APP_EVENT_STATE_WINDOW_LOST_FOCUS"),
            ("APP_EVENT_STATE_WINDOW_GAINED_FOCUS"),
            ("APP_EVENT_STATE_SAVE_STATE"),
            ("APP_EVENT_STATE_APP_SUSPENDED"),
            ("APP_EVENT_STATE_APP_ACTIVATED"),
            ("APP_EVENT_RUN_CALLBACK"),
    };


    if (State == APP_EVENT_STATE_INVALID)
    {
        return ("APP_EVENT_STATE_INVALID");
    }
    else if (State > APP_EVENT_RUN_CALLBACK || State < 0)
    {
        return ("UnknownEAppEventStateValue");
    }
    else
    {
        return Names[State];
    }
}

//app->Run()里Tick()
void FAppEventManager::Tick()
{
   // GE_ASSERT(IsInGameThread());
    while (!Queue.empty())
    {
        FAppEventPacket Event = DequeueAppEvent();
       // FPlatformMisc::LowLevelOutputDebugStringf(("FAppEventManager::Tick processing, %d"), int(Event.State));

        switch (Event.State)
        {
            case APP_EVENT_STATE_WINDOW_CREATED:
                AndroidWindow::EventManagerUpdateWindowDimensions(Event.Data.WindowWidth, Event.Data.WindowHeight);
                bCreateWindow = true;
                break;
            case APP_EVENT_STATE_WINDOW_RESIZED:
                // Cache the new window's dimensions for the game thread.
                AndroidWindow::EventManagerUpdateWindowDimensions(Event.Data.WindowWidth, Event.Data.WindowHeight);
                ExecWindowResized();
                break;
            case APP_EVENT_STATE_WINDOW_CHANGED:
                // React on device orientation/windowSize changes only when application has window
                // In case window was created this tick it should already has correct size
                // see 'Java_com_epicgames_unreal_GameActivity_nativeOnConfigurationChanged' for event thread/game thread mismatches.
                ExecWindowResized();
                break;
            case APP_EVENT_STATE_SAVE_STATE:
                bSaveState = true; //todo android: handle save state.
                break;
            case APP_EVENT_STATE_WINDOW_DESTROYED:
                bHaveWindow = false;
              //  FPlatformMisc::LowLevelOutputDebugStringf(("APP_EVENT_STATE_WINDOW_DESTROYED, %d, %d, %d"), int(bRunning), int(bHaveWindow), int(bHaveGame));
                break;
            case APP_EVENT_STATE_ON_START:
                //doing nothing here
                break;
            case APP_EVENT_STATE_ON_DESTROY:
                GE_ASSERT(bHaveWindow == false);
                GE_ASSERT(IsEngineExitRequested()); //destroy immediately. Game will shutdown.
                //FPlatformMisc::LowLevelOutputDebugStringf(("APP_EVENT_STATE_ON_DESTROY"));
                break;
            case APP_EVENT_STATE_ON_STOP:
                bHaveGame = false;
                ReleaseMicrophone(true);
                break;
            case APP_EVENT_STATE_ON_PAUSE:
                FAndroidAppEntry::OnPauseEvent();
                bHaveGame = false;
                break;
            case APP_EVENT_STATE_ON_RESUME:
                bHaveGame = true;
                break;

                // window focus events that follow their own hierarchy, and might or might not respect App main events hierarchy
            case APP_EVENT_STATE_WINDOW_GAINED_FOCUS:
                bWindowInFocus = true;
                break;
            case APP_EVENT_STATE_WINDOW_LOST_FOCUS:
                bWindowInFocus = false;
                break;
            case APP_EVENT_RUN_CALLBACK:
            {
               // UE_LOG(LogAndroidEvents, Display, ("Event thread callback running."));
                Event.Data.CallbackFunc();
                break;
            }
            case APP_EVENT_STATE_APP_ACTIVATED:
                bRunning = true;
              //  FPlatformMisc::LowLevelOutputDebugStringf(("Execution will be resumed!"));
                break;
            case APP_EVENT_STATE_APP_SUSPENDED:
                bRunning = false;
              //  FPlatformMisc::LowLevelOutputDebugStringf(("Execution will be paused..."));
                break;
            default:
                break;
              //  UE_LOG(LogAndroidEvents, Display, ("Application Event : %u  not handled. "), Event.State);
        }

        if (bCreateWindow)
        {
            // wait until activity is in focus.
            if (bWindowInFocus)
            {
                ExecWindowCreated();
                bCreateWindow = false;
                bHaveWindow = true;
                //FPlatformMisc::LowLevelOutputDebugStringf(("ExecWindowCreated, %d, %d, %d"), int(bRunning), int(bHaveWindow), int(bHaveGame));
            }
        }
    }

//    if (EmptyQueueHandlerEvent)
//    {
//        EmptyQueueHandlerEvent->Trigger();
//    }

    if (!bRunning)
    {
        //FPlatformMisc::LowLevelOutputDebugStringf(("FAppEventManager::Tick EventHandlerEvent Wait "));
        //EventHandlerEvent->Wait();
       // FPlatformMisc::LowLevelOutputDebugStringf(("FAppEventManager::Tick EventHandlerEvent DONE Wait "));
    }
}

void FAppEventManager::ReleaseMicrophone(bool shuttingDown)
{
//    if (FModuleManager::Get().IsModuleLoaded("Voice"))
//    {
//       // UE_LOG(LogTemp, Log, ("Android release microphone"));
//        FModuleManager::Get().UnloadModule("Voice", shuttingDown);
//    }
}

void FAppEventManager::TriggerEmptyQueue()
{
//    if (EmptyQueueHandlerEvent)
//    {
//        EmptyQueueHandlerEvent->Trigger();
//    }
}

FAppEventManager::FAppEventManager():
       // EventHandlerEvent(nullptr)
       // ,EmptyQueueHandlerEvent(nullptr)
       FirstInitialized(false)
        ,bCreateWindow(false)
        ,bWindowInFocus(true)
        ,bSaveState(false)
        ,bAudioPaused(false)
        ,bHaveWindow(false)
        ,bHaveGame(false)
        ,bRunning(false)
{
    pthread_mutex_init(&QueueMutex, NULL);

//    IConsoleVariable* CVarScale = IConsoleManager::Get().FindConsoleVariable(("r.MobileContentScaleFactor"));
//    GE_ASSERT(CVarScale);
//    CVarScale->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&FAppEventManager::OnScaleFactorChanged));
//
//    IConsoleVariable* CVarResX = IConsoleManager::Get().FindConsoleVariable(("r.Mobile.DesiredResX"));
//    GE_ASSERT(CVarResX);
//    CVarResX->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&FAppEventManager::OnScaleFactorChanged));
//
//    IConsoleVariable* CVarResY = IConsoleManager::Get().FindConsoleVariable(("r.Mobile.DesiredResY"));
//    GE_ASSERT(CVarResY);
//    CVarResY->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&FAppEventManager::OnScaleFactorChanged));
}

//void FAppEventManager::OnScaleFactorChanged(IConsoleVariable* CVar)
//{
//    if ((CVar->GetFlags() & ECVF_SetByMask) == ECVF_SetByConsole)
//    {
//        FAppEventManager::GetInstance()->ExecWindowResized();
//    }
//}

void FAppEventManager::HandleWindowCreated_EventThread(void* InWindow)
{
    bool AlreadyInited = FirstInitialized;

    // Make sure window will not be deleted until event is processed
    // Window could be deleted by OS while event queue stuck at game start-up phase
    AndroidWindow::AcquireWindowRef((ANativeWindow*)InWindow);

    GE_ASSERT(AndroidWindow::GetHardwareWindow_EventThread() == NULL);
    AndroidWindow::SetHardwareWindow_EventThread(InWindow);

    if (!AlreadyInited)
    {
        //This cannot wait until first tick. 
        FirstInitialized = true;
    }
    EnqueueAppEvent(APP_EVENT_STATE_WINDOW_CREATED, FAppEventData((ANativeWindow*)InWindow));
}

void FAppEventManager::HandleWindowClosed_EventThread()
{
    GE_ASSERT(AndroidWindow::GetHardwareWindow_EventThread());

    AndroidWindow::ReleaseWindowRef((ANativeWindow*)AndroidWindow::GetHardwareWindow_EventThread());
    AndroidWindow::SetHardwareWindow_EventThread(nullptr);

    EnqueueAppEvent(APP_EVENT_STATE_WINDOW_DESTROYED);
}


//void FAppEventManager::SetEventHandlerEvent(FEvent* InEventHandlerEvent)
//{
//    EventHandlerEvent = InEventHandlerEvent;
//}
//
//void FAppEventManager::SetEmptyQueueHandlerEvent(FEvent* InEventHandlerEvent)
//{
//    EmptyQueueHandlerEvent = InEventHandlerEvent;
//}

void FAppEventManager::PauseRendering()
{
//    if(GUseThreadedRendering )
//    {
//        if (GIsThreadedRendering)
//        {
//            StopRenderingThread();
//        }
//    }
//    else
    {
        RHIReleaseThreadOwnership();
    }
}


void FAppEventManager::ResumeRendering()
{
//    if( GUseThreadedRendering )
//    {
//        if (!GIsThreadedRendering)
//        {
//            StartRenderingThread();
//        }
//    }
//    else
    {
        RHIAcquireThreadOwnership();
    }
}


void FAppEventManager::ExecWindowCreated()
{
    //UE_LOG(LogAndroidEvents, Display, ("ExecWindowCreated"));
    // When application launched while device is in sleep mode SystemResolution could be set to opposite orientation values
    // Force to update SystemResolution to current values whenever we create a new window
    RHIRect ScreenRect = AndroidWindow::GetScreenRect();
   // FSystemResolution::RequestResolutionChange(ScreenRect.Right, ScreenRect.Bottom, EWindowMode::Fullscreen);

    // ReInit with the new window handle
    FAndroidAppEntry::ReInitWindow();
    AndroidApplication::OnWindowSizeChanged();
}

void FAppEventManager::ExecWindowResized()
{
    if (bRunning)
    {
       // FlushRenderingCommands();
    }
    AndroidWindow::InvalidateCachedScreenRect();
    FAndroidAppEntry::ReInitWindow();
   // AndroidApplication::OnWindowSizeChanged();
}

void FAppEventManager::PauseAudio()
{
//    if (!GEngine || !GEngine->IsInitialized())
//    {
//        UE_LOG(LogTemp, Log, ("Engine not initialized, not pausing Android audio"));
//        return;
//    }
//
//    bAudioPaused = true;
//    UE_LOG(LogTemp, Log, ("Android pause audio"));
//
//    FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice();
//    if (AudioDevice)
//    {
//        FAudioCommandFence Fence;
//        Fence.BeginFence();
//        Fence.Wait();
//
//        AudioDevice->SuspendContext();
//    }
}


void FAppEventManager::ResumeAudio()
{
//    if (!GEngine || !GEngine->IsInitialized())
//    {
//        UE_LOG(LogTemp, Log, ("Engine not initialized, not resuming Android audio"));
//        return;
//    }
//
//    bAudioPaused = false;
//    UE_LOG(LogTemp, Log, ("Android resume audio"));
//
//    FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice();
//    if (AudioDevice)
//    {
//        AudioDevice->ResumeContext();
//    }
}


void FAppEventManager::EnqueueAppEvent(EAppEventState InState, FAppEventData&& InData)
{
    FAppEventPacket Event;
    Event.State = InState;
    Event.Data = InData;

    int rc = pthread_mutex_lock(&QueueMutex);
    GE_ASSERT(rc == 0);
    Queue.push(Event);

//    if (EmptyQueueHandlerEvent)
//    {
//        EmptyQueueHandlerEvent->Reset();
//    }

    rc = pthread_mutex_unlock(&QueueMutex);
    GE_ASSERT(rc == 0);

   // FPlatformMisc::LowLevelOutputDebugStringf(("LogAndroidEvents::EnqueueAppEvent : %u, [width=%d, height=%d], tid = %d, %s"), InState, InData.WindowWidth, InData.WindowHeight, gettid(), GetAppEventName(InState));
}

FAppEventPacket FAppEventManager::DequeueAppEvent()
{
    int rc = pthread_mutex_lock(&QueueMutex);
    GE_ASSERT(rc == 0);

    FAppEventPacket OutData;
    OutData = Queue.front();
    Queue.pop();

    rc = pthread_mutex_unlock(&QueueMutex);
    GE_ASSERT(rc == 0);

    //UE_LOG(LogAndroidEvents, Display, ("LogAndroidEvents::DequeueAppEvent : %u, [width=%d, height=%d], %s"), OutData.State, OutData.Data.WindowWidth, OutData.Data.WindowHeight, GetAppEventName(OutData.State))

    return OutData;
}


bool FAppEventManager::IsGamePaused()
{
    return !bRunning;
}


bool FAppEventManager::IsGameInFocus()
{
    return (bWindowInFocus && bHaveWindow);
}


bool FAppEventManager::WaitForEventInQueue(EAppEventState InState, double TimeoutSeconds)
{
    bool FoundEvent = false;
    double StopTime = FPlatformTime::Seconds() + TimeoutSeconds;

    std::queue<FAppEventPacket> HoldingQueue;
    while (!FoundEvent)
    {
        int rc = pthread_mutex_lock(&QueueMutex);
        GE_ASSERT(rc == 0);

        // Copy the existing queue (and GE_ASSERT for our event)
        while (!Queue.empty())
        {
            FAppEventPacket OutData;
            OutData = Queue.front();
            Queue.pop();

            if (OutData.State == InState)
                FoundEvent = true;

            HoldingQueue.push(OutData);
        }

        if (FoundEvent)
            break;

        // Time expired?
        if (FPlatformTime::Seconds() > StopTime)
            break;

        // Unlock for new events and wait a bit before trying again
        rc = pthread_mutex_unlock(&QueueMutex);
        GE_ASSERT(rc == 0);
        FPlatformProc::Sleep(0.01f);
    }

    // Add events back to queue from holding
    while (!HoldingQueue.empty())
    {
        FAppEventPacket OutData;
        OutData = HoldingQueue.front();
        HoldingQueue.pop();
        Queue.push(OutData);
    }

    int rc = pthread_mutex_unlock(&QueueMutex);
    GE_ASSERT(rc == 0);

    return FoundEvent;
}

extern volatile bool GEventHandlerInitialized;


}
#endif
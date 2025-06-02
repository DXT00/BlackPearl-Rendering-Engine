//
// Created by DXT00 on 2025/4/13.
//
#include "pch.h"
#include "Luanch/Luanch.h"
#include "Luanch/Android/LuanchAndroid.h"
#include "BlackPearl/Core/PlatformProc.h"
#include "BlackPearl/Core/PlatformMisc.h"
#include "BlackPearl/Config.h"
#include "Core/CriticalSection.h"

#ifdef GE_PLATFORM_ANDROID

#include "Luanch/Android/AndroidEventManager.h"
#include "BlackPearl/Core/Android/AndroidWindow.h"
#include "Core/Android/AndroidJNI.h"
#include "Luanch/Android/AndroidInputManager.h"

#ifdef USE_ANDROID_JNI

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#endif

namespace BlackPearl {
    FCriticalSection GAndroidWindowLock;
    volatile bool GWindowInit = false;

    pthread_t G_AndroidEventThread;

    static uint32_t EventThreadID = 0;
    static bool bAppIsActive_EventThread = false;

    extern JavaVM *GJavaVM;
    extern ANativeActivity *GNativeActivity;

    extern void BlockRendering();

    // called whenever the app loses loses window or pause.
    static void SuspendApp_EventThread() {
//        if (!bAppIsActive_EventThread)
//        {
//            return;
//        }
//        bAppIsActive_EventThread = false;
//        // Lock the window, this prevents event thread from removing the window whilst the RHI initializes.
//
//        //UE_LOG(LogAndroid, Log, TEXT("event thread, suspending app, acquiring HW window lock."));
//        GAndroidWindowLock.Lock();
//
//        if (bReadyToProcessEvents == false)
//        {
//            // App has stopped before we can process events.
//            // AndroidLaunch will lock GAndroidWindowLock, and set bReadyToProcessEvents when we are able to block the RHI and queue up other events.
//            // we ignore events until this point as acquiring GAndroidWindowLock means requires the window to be properly initialized.
//           // UE_LOG(LogAndroid, Log, TEXT("event thread, app not yet ready."));
//            return;
//        };
//
//        TSharedPtr<FEvent, ESPMode::ThreadSafe> EMDoneTrigger = MakeShareable(FPlatformProcess::GetSynchEventFromPool(), [](FEvent* EventToDelete)
//        {
//            FPlatformProcess::ReturnSynchEventToPool(EventToDelete);
//        });
//
////        // perform the delegates before the window handle is cleared.
////        // This ensures any tasks that require a window handle will have it before we block the RT on the invalid window.
////        FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_RUN_CALLBACK, FAppEventData([EMDoneTrigger]()
////                                                                                               {
////                                                                                                   UE_LOG(LogAndroid, Log, TEXT("performing app backgrounding callback. %p"), EMDoneTrigger.Get());
////
////                                                                                                   FCoreDelegates::ApplicationWillDeactivateDelegate.Broadcast();
////                                                                                                   FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Broadcast();
////                                                                                                   FAppEventManager::GetInstance()->PauseAudio();
////                                                                                                   FAppEventManager::ReleaseMicrophone(false);
////                                                                                                   EMDoneTrigger->Trigger();
////                                                                                               }));
//
//        FEmbeddedCommunication::WakeGameThread();
//
//        FPreLoadScreenManager::EnableRendering(false);
//
//        FThreadHeartBeat::Get().SuspendHeartBeat(true);

        // wait for a period of time before blocking rendering
        //UE_LOG(LogAndroid, Log, TEXT("AndroidEGL::  SuspendApp_EventThread, waiting for event manager to process. tid: %d"), FPlatformTLS::GetCurrentThreadId());
        //  bool bSuccess = EMDoneTrigger->Wait(4000);
        //UE_CLOG(!bSuccess, LogAndroid, Log, TEXT("backgrounding callback, not responded in timely manner."));

        // BlockRendering();

        // Suspend the GT.
        FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_APP_SUSPENDED);
    }

// called when the app has window + resume.
    static void ActivateApp_EventThread() {
        if (bAppIsActive_EventThread) {
            // Seems this can occur.
            return;
        }

        // Unlock window when we're ready.
        //UE_LOG(LogAndroid, Log, TEXT("event thread, activate app, unlocking HW window"));
        GAndroidWindowLock.Unlock();
//        // wake the GT up.
        FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_APP_ACTIVATED);
//
        bAppIsActive_EventThread = true;
//        FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_RUN_CALLBACK, FAppEventData([]()
//                                                                                               {
//                                                                                                   UE_LOG(LogAndroid, Log, TEXT("performing app foregrounding callback."));
//                                                                                                   FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Broadcast();
//                                                                                                   FCoreDelegates::ApplicationHasReactivatedDelegate.Broadcast();
//                                                                                                   FAppEventManager::GetInstance()->ResumeAudio();
//                                                                                               }));
//
//        if (EventHandlerEvent)
//        {
//            // Must flush the queue before enabling rendering.
//            EventHandlerEvent->Trigger();
//        }
//
//        FThreadHeartBeat::Get().ResumeHeartBeat(true);
//
//        FPreLoadScreenManager::EnableRendering(true);
//
//        extern void AndroidThunkCpp_ShowHiddenAlertDialog();
//        AndroidThunkCpp_ShowHiddenAlertDialog();
    }

//Called from the event process thread
    static void OnAppCommandCB(struct android_app *app, int32_t cmd) {
        // GE_ASSERT(IsInAndroidEventThread());
        static bool bDidGainFocus = false;
        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("OnAppCommandCB cmd: %u, tid = %d"), cmd, gettid());

        static bool bHasFocus = false;
        static bool bHasWindow = false;
        static bool bIsResumed = false;

        // Set event thread's view of the window dimensions:
        {
            ANativeWindow *DimensionWindow = app->pendingWindow ? app->pendingWindow : app->window;
            if (DimensionWindow) {
                BlackPearl::AndroidWindow::SetWindowDimensions_EventThread(DimensionWindow);
            }
        }

        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                /**
                * Command from main thread: the app should generate a new saved state
                * for itself, to restore from later if needed.  If you have saved state,
                * allocate it with malloc and place it in android_app.savedState with
                * the size in android_app.savedStateSize.  The will be freed for you
                * later.
                */
                // the OS asked us to save the state of the app
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_SAVE_STATE"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_SAVE_STATE);
                break;
            case APP_CMD_INIT_WINDOW:
                /**
                 * Command from main thread: a new ANativeWindow is ready for use.  Upon
                 * receiving this command, android_app->window will contain the new window
                 * surface.
                 */
                // get the window ready for showing
                // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Case APP_CMD_INIT_WINDOW"));
                //UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_INIT_WINDOW"));
                FAppEventManager::GetInstance()->HandleWindowCreated_EventThread(
                        app->pendingWindow);
                bHasWindow = true;
                GWindowInit = true;
                if (bHasWindow && bHasFocus && bIsResumed) {
                    ActivateApp_EventThread();
                }
                break;
            case APP_CMD_TERM_WINDOW:
                /**
                 * Command from main thread: the existing ANativeWindow needs to be
                 * terminated.  Upon receiving this command, android_app->window still
                 * contains the existing window; after calling android_app_exec_cmd
                 * it will be set to NULL.
                 */
                //  FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Case APP_CMD_TERM_WINDOW, tid = %d"), gettid());
                // clean up the window because it is being hidden/closed
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_TERM_WINDOW"));

                SuspendApp_EventThread();

                FAppEventManager::GetInstance()->HandleWindowClosed_EventThread();
                bHasWindow = false;
                break;
            case APP_CMD_LOST_FOCUS:
                /**
                 * Command from main thread: the app's activity window has lost
                 * input focus.
                 */
                // if the app lost focus, avoid unnecessary processing (like monitoring the accelerometer)
                // log it, but the actual event will be simulated later in APP_CMD_PAUSE
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_LOST_FOCUS"));

                break;
            case APP_CMD_GAINED_FOCUS:
                /**
                 * Command from main thread: the app's activity window has gained
                 * input focus.
                 */

                // bring back a certain functionality, like monitoring the accelerometer
                // log it, but the actual event will be simulated later in APP_CMD_RESUME
                GE_CORE_INFO("Case APP_CMD_GAINED_FOCUS");

                // still check for a rare case needing activation
                if (bHasWindow && bHasFocus && bIsResumed) {
                    ActivateApp_EventThread();
                }
                break;
            case APP_CMD_INPUT_CHANGED:
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_INPUT_CHANGED"));
                break;
            case APP_CMD_WINDOW_RESIZED:
                /**
                 * Command from main thread: the current ANativeWindow has been resized.
                 * Please redraw with its new size.
                 */
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_WINDOW_RESIZED"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_WINDOW_RESIZED,
                                                                 FAppEventData(app->window));
                break;
            case APP_CMD_WINDOW_REDRAW_NEEDED:
                /**
                 * Command from main thread: the system needs that the current ANativeWindow
                 * be redrawn.  You should redraw the window before handing this to
                 * android_app_exec_cmd() in order to avoid transient drawing glitches.
                 */
                //UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_WINDOW_REDRAW_NEEDED"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(
                        APP_EVENT_STATE_WINDOW_REDRAW_NEEDED);
                break;
            case APP_CMD_CONTENT_RECT_CHANGED:
                /**
                 * Command from main thread: the content area of the window has changed,
                 * such as from the soft input window being shown or hidden.  You can
                 * find the new content rect in android_app::contentRect.
                 */
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_CONTENT_RECT_CHANGED"));
                break;
                /* receive this event from Java instead to work around NDK bug with AConfiguration_getOrientation in Oreo
                case APP_CMD_CONFIG_CHANGED:
                    {
                        // Command from main thread: the current device configuration has changed.
                        UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_CONFIG_CHANGED"));

                        bool bPortrait = (AConfiguration_getOrientation(app->config) == ACONFIGURATION_ORIENTATION_PORT);
                        if (FAndroidWindow::OnWindowOrientationChanged(bPortrait))
                        {
                            FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_WINDOW_CHANGED);
                        }
                    }
                    break;
                */
            case APP_CMD_LOW_MEMORY:
                /**
                 * Command from main thread: the system is running low on memory.
                 * Try to reduce your memory use.
                 */
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_LOW_MEMORY"));
                break;
            case APP_CMD_START:
                /**
                 * Command from main thread: the app's activity has been started.
                 */
                //  UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_START"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_ON_START);

                break;
            case APP_CMD_RESUME:
                /**
                 * Command from main thread: the app's activity has been resumed.
                 */
                bIsResumed = true;

                // assume focus on resume
                bDidGainFocus = true;
                bHasFocus = true;

                if (bHasWindow && bHasFocus && bIsResumed) {
                    ActivateApp_EventThread();
                }
                // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Case APP_CMD_RESUME"));
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_RESUME"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_ON_RESUME);

                // trigger focus
                FAppEventManager::GetInstance()->EnqueueAppEvent(
                        APP_EVENT_STATE_WINDOW_GAINED_FOCUS);

                /*
                * On the initial loading the restart method must be called immediately
                * in order to restart the app if the startup movie was playing
                */
//            if (bShouldRestartFromInterrupt)
//            {
//                AndroidThunkCpp_RestartApplication("");
//            }
                break;
            case APP_CMD_PAUSE: {
                /**
                 * Command from main thread: the app's activity has been paused.
                 */
                //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Case APP_CMD_PAUSE"));
                //UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_PAUSE"));

                // simulate lost focus
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_WINDOW_LOST_FOCUS);
                bHasFocus = false;

                // Ignore pause command for Oculus if the window hasn't been initialized to prevent halting initial load
                // if the headset is not active
                if (!bHasWindow && FAndroidMisc::GetDeviceMake() == std::string("Oculus")) {
                    //    FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Oculus: Ignoring APP_CMD_PAUSE command before APP_CMD_INIT_WINDOW"));
                    //    UE_LOG(LogAndroid, Log, TEXT("Oculus: Ignoring APP_CMD_PAUSE command before APP_CMD_INIT_WINDOW"));
                    break;
                }

                bIsResumed = false;
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_ON_PAUSE);

                bool bAllowReboot = true;
#if FAST_BOOT_HACKS
                if (FEmbeddedDelegates::GetNamedObject(TEXT("LoggedInObject")) == nullptr)
            {
                bAllowReboot = false;
            }
#endif

                // Restart on resuming if did not complete engine initialization
//            if (!bDidCompleteEngineInit && bDidGainFocus  && !bIgnorePauseOnDownloaderStart && bAllowReboot)
//            {
//// 			// only do this if early startup enabled
//// 			std::string *EarlyRestart = FAndroidMisc::GetConfigRulesVariable(TEXT("earlyrestart"));
//// 			if (EarlyRestart != NULL && EarlyRestart->Equals("true", ESearchCase::IgnoreCase))
//// 			{
//// 				bShouldRestartFromInterrupt = true;
//// 			}
//            }
//            bIgnorePauseOnDownloaderStart = false;
//
//            /*
//             * On the initial loading the pause method must be called immediately
//             * in order to stop the startup movie's sound
//            */
//            if (IsPreLoadScreenPlaying() && bAllowReboot)
//            {
//                UE_LOG(LogAndroid, Log, TEXT("MoviePlayer force completion"));
//                GetMoviePlayer()->ForceCompletion();
//            }

                SuspendApp_EventThread();

                break;
            }
            case APP_CMD_STOP:
                /**
                 * Command from main thread: the app's activity has been stopped.
                 */
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_STOP"));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_ON_STOP);
                break;
            case APP_CMD_DESTROY:
                /**
                * Command from main thread: the app's activity is being destroyed,
                * and waiting for the app thread to clean up and exit before proceeding.
                */
                // UE_LOG(LogAndroid, Log, TEXT("Case APP_CMD_DESTROY"));

//            FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_RUN_CALLBACK, FAppEventData([]()
//            {
//                FGraphEventRef WillTerminateTask = FFunctionGraphTask::CreateAndDispatchWhenReady([]()
//                        {
//                    PRAGMA_DISABLE_DEPRECATION_WARNINGS
//                    FCoreDelegates::ApplicationWillTerminateDelegate.Broadcast();
//                    PRAGMA_ENABLE_DEPRECATION_WARNINGS
//
//                                                                                                                                                                                             FCoreDelegates::GetApplicationWillTerminateDelegate().Broadcast();
//                                                                                                                                                                                         }, TStatId(), NULL, ENamedThreads::GameThread);
//                                                                                                       FTaskGraphInterface::Get().WaitUntilTaskCompletes(WillTerminateTask);
//                                                                                                       FAndroidMisc::NonReentrantRequestExit();
//                                                                                                   }));
                FAppEventManager::GetInstance()->EnqueueAppEvent(APP_EVENT_STATE_ON_DESTROY);

                // Exit here, avoids having to unlock the window and letting the RHI's deal with invalid window.
                extern void AndroidThunkCpp_ForceQuit();
                AndroidThunkCpp_ForceQuit();

                break;
        }

//    if (EventHandlerEvent)
//    {
//        EventHandlerEvent->Trigger();
//    }

        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#### END OF OnAppCommandCB cmd: %u, tid = %d"), cmd, gettid());
    }

    static void *AndroidEventThreadWorker(void *param) {
        pthread_setname_np(pthread_self(), "BP_EventWorker");
        EventThreadID = FPlatformTLS::GetCurrentThreadId();
        FAndroidMisc::RegisterThreadName("BP_EventWorker", EventThreadID);
//
        struct android_app *state = (struct android_app *) param;
//
//    FPlatformProcess::SetThreadAffinityMask(FPlatformAffinity::GetMainGameMask());
//
//    FPlatformMisc::LowLevelOutputDebugString(TEXT("Entering event processing thread engine entry point"));
//
        ALooper *looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
        ALooper_addFd(looper, state->msgread, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL,
                      &state->cmdPollSource);
        state->looper = looper;

        state->onAppCmd = OnAppCommandCB;
        state->onInputEvent = LuanchAndroid::HandleInput;


//    // window is initially invalid/locked.
        GE_CORE_INFO("event thread, Initial HW window lock.");
        GAndroidWindowLock.Lock();

        while (!ShouldEngineExit()) {
            int ident, events;
            struct android_poll_source *source;

            // 处理事件
            while ((ident = ALooper_pollAll(0, nullptr, &events, (void **) &source)) >= 0) {
                if (source != nullptr) source->process(state, source);
                if (state->destroyRequested != 0) return NULL;
            }


        }
        GAndroidWindowLock.Unlock();
        return NULL;
    }

    struct android_app *GNativeAndroidApp = NULL;

    namespace LuanchAndroid {

        static float lastX = 0, lastY = 0;
        static bool isDragging = false;

        int32_t HandleInput(struct android_app* app, AInputEvent* event) {
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
                float x = AMotionEvent_getX(event, 0);
                float y = AMotionEvent_getY(event, 0);

                switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) {
                    case AMOTION_EVENT_ACTION_DOWN:
                        lastX = x;
                        lastY = y;
                        isDragging = true;
                        break;

                    case AMOTION_EVENT_ACTION_MOVE:
                        if (isDragging) {
                            // 计算触摸位移差
                            float dx = x - lastX;
                            float dy = lastY - y; // Y轴取反（Android坐标原点在左上）
                            GE_CORE_INFO("[dxt00] get rot dx_yaw:%f, dy_pitch%f", dx, dy);
                            AndroidInputManager::GetInstance()->EnqueueInputRotationEvent(dx,dy);
//                            // 更新相机角度
//                            camera.yaw += dx * camera.sensitivity;
//                            camera.pitch += dy * camera.sensitivity;
//
//                            // 限制俯仰角（避免翻转）
//                            if (camera.pitch > 89.0f) camera.pitch = 89.0f;
//                            if (camera.pitch < -89.0f) camera.pitch = -89.0f;
//
//                            // 更新相机朝向
//                            glm::vec3 front;
//                            front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
//                            front.y = sin(glm::radians(camera.pitch));
//                            front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
//                            camera.front = glm::normalize(front);

                            lastX = x;
                            lastY = y;
                        }

                        if (AMotionEvent_getPointerCount(event) >= 2) {
                            // 双指滑动控制移动
                            float dx = AMotionEvent_getX(event, 1) - AMotionEvent_getX(event, 0);
                            float dy = AMotionEvent_getY(event, 1) - AMotionEvent_getY(event, 0);
                            AndroidInputManager::GetInstance()->EnqueueInputPositionEvent(dx,dy);
                            GE_CORE_INFO("[dxt00] get pos dx:%f, dy%f", dx, dy);
                            // 计算移动方向（右向量和前向量的叉积得到右方向）
                          //  glm::vec3 right = glm::normalize(glm::cross(camera.front, camera.up));

//                            // 双指水平滑动：左右移动
//                            if (fabs(dx) > 10) {
//                                camera.position += right * (dx > 0 ? camera.speed : -camera.speed);
//                            }
//
//                            // 双指垂直滑动：前后移动
//                            if (fabs(dy) > 10) {
//                                camera.position += camera.front * (dy > 0 ? -camera.speed : camera.speed);
//                            }
                        }
                        break;
                        break;

                    case AMOTION_EVENT_ACTION_UP:
                        isDragging = false;
                        break;



                }
            }
            return 0;
        }

        void InitAndriodThread(struct android_app *state) {
            GNativeAndroidApp = state;
            //  FTaskTagScope Scope(ETaskTag::EGameThread);

            GGameThreadId = FPlatformTLS::GetCurrentThreadId();
            //BootTimingPoint("android_main");
            //FPlatformMisc::LowLevelOutputDebugString(TEXT("Entering native app glue main function"));

            //GNativeAndroidApp = state;
            // GE_ASSERT(GNativeAndroidApp);

            pthread_attr_t otherAttr;
            pthread_attr_init(&otherAttr);
            pthread_attr_setdetachstate(&otherAttr, PTHREAD_CREATE_DETACHED);
            pthread_create(&G_AndroidEventThread, &otherAttr, AndroidEventThreadWorker, state);

            // FPlatformMisc::LowLevelOutputDebugString(TEXT("Created event thread"));
        }
#define JNI_CURRENT_VERSION JNI_VERSION_1_6
        // 对应 AndroidJNI.cpp 中的 JNI_OnLoad
        void InitJavaEnv(struct android_app *state) {

            //FPlatformMisc::LowLevelOutputDebugString(TEXT("In the JNI_OnLoad function"));
            //JNI_OnUnload(state->activity->vm,nullptr);
            JNIEnv* Env = NULL;

            // if you have problems with stuff being missing especially in distribution builds then it could be because proguard is stripping things from java
            // check proguard-project.txt and see if your stuff is included in the exceptions
            GJavaVM = state->activity->vm;
            JNIEnv* env = state->activity->env;
            state->activity->vm->GetEnv((void **)&Env, JNI_CURRENT_VERSION);
            GNativeActivity = state->activity;

          //  AndroidApplication::InitializeJavaEnv(BlackPearl::GJavaVM, JNI_CURRENT_VERSION, FJavaWrapper::GameActivityThis);
          //  FJavaWrapper::FindClassesAndMethods(Env);

        }
    }
#undef JNI_CURRENT_VERSION

}
#endif
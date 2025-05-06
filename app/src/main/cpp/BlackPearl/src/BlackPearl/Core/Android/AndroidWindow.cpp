//
// Created by DXT00 on 2025/4/13.
//
// Copyright Epic Games, Inc. All Rights Reserved.
#include "pch.h"

#include "Core/Android/AndroidWindow.h"
#include "Core/Android/AndroidPlatformMisc.h"
#include "BlackPearl/Luanch/Android/AndroidEventManager.h"
#include "BlackPearl/Core/Platform.h"
#include "Core/PlatformProc.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Luanch/Luanch.h"
#include "BlackPearl/Common/CommonFunc.h"
//#include "Android/AndroidWindowUtils.h"
//#include "Android/AndroidEventManager.h"


//#include "HAL/OutputDevices.h"
//#include "HAL/IConsoleManager.h"
//#include "Misc/CommandLine.h"
//#include "HAL/PlatformStackWalk.h"

//
//int32_t GAndroidWindowDPI = 0;
//static FAutoConsoleVariableRef CVarAndroidWindowDPI(
//        TEXT("Android.WindowDPI"),
//        GAndroidWindowDPI,
//        TEXT("Values > 0 will set the system window resolution (i.e. swap buffer) to achieve the DPI requested.\n")
//TEXT("default: 0"),
//ECVF_ReadOnly);
//
//int32_t GAndroidWindowDPIQueryMethod = 0;
//static FAutoConsoleVariableRef CVarAndroidWindowDPIQueryMethod(
//        TEXT("Android.DPIQueryMethod"),
//        GAndroidWindowDPIQueryMethod,
//        TEXT("The method used to determine the native screen DPI when calculating the scale factor required to achieve the requested Android.WindowDPI.\n")
//TEXT("0: Use displaymetrics xdpi/ydpi (default)\n")
//TEXT("1: Use displaymetrics densityDpi"),
//ECVF_ReadOnly);
//
//int32_t GAndroid3DSceneMaxDesiredPixelCount = 0;
//static FAutoConsoleVariableRef CVarAndroid3DSceneMaxDesiredPixelCount(
//        TEXT("Android.3DSceneMaxDesiredPixelCount"),
//        GAndroid3DSceneMaxDesiredPixelCount,
//        TEXT("Works in conjunction with Android.WindowDPI, this specifies a maximum pixel count for the 3D scene.\n")
//TEXT("Values >0 will be used to scale down the 3D scene (equivalent to setting r.screenpercentage)\n")
//TEXT("such that the 3d scene will be no more than 3DSceneMaxDesiredPixelCount pixels.\n")
//TEXT("This is only applied if the 3d scene pixel at the 'Android.WindowDPI' DPI goes above the specified value.\n")
//TEXT("default: 0"),
//ECVF_ReadOnly);
//
//int32_t GAndroid3DSceneMinDPI = 0;
//static FAutoConsoleVariableRef CVarAndroid3DSceneMinDPI(
//        TEXT("Android.3DSceneMinDPI"),
//        GAndroid3DSceneMinDPI,
//        TEXT("Works in conjunction with Android.3DSceneMaxDesiredPixelCount, and specifies a minimum DPI level for the 3D scene.\n")
//TEXT("Values >0 specify an absolute minimum 3D scene DPI, if Android.3DSceneMaxDesiredPixelCount causes the 3d resolution to be below\n")
//TEXT("Android.3DSceneMinDPI then the scale factor will be set to achieve the minimum dpi.\n")
//TEXT("Useful to achieve a minimum quality level on low DPI devices at the expense of GPU performance.\n")
//TEXT("default: 0"),
//ECVF_ReadOnly);

namespace BlackPearl {


    int GAndroidPropagateAlpha = 0;

    struct FAndroidCachedWindowRectParams {
        int32_t WindowWidth = -1;
        int32_t WindowHeight = -1;

        float ContentScaleFactor = -1.0f;
        int32_t MobileResX = -1;
        int32_t MobileResY = -1;

        int32_t WindowDPI = -1;
        int32_t SceneMinDPI = -1;
        int32_t SceneMaxDesiredPixelCount = -1;

        ANativeWindow *Window_EventThread = nullptr;

        bool operator==(const FAndroidCachedWindowRectParams &Rhs) const {
            return FMemory::Memcmp(this, &Rhs, sizeof(Rhs)) == 0;
        }

        bool operator!=(const FAndroidCachedWindowRectParams &Rhs) const {
            return FMemory::Memcmp(this, &Rhs, sizeof(Rhs)) != 0;
        }
    };


// Cached calculated screen resolution
    static FAndroidCachedWindowRectParams CachedWindowRect;
    static FAndroidCachedWindowRectParams CachedWindowRect_EventThread;

    const FAndroidCachedWindowRectParams &GetCachedRect(bool bUseEventThreadWindow) {
        return bUseEventThreadWindow ? CachedWindowRect_EventThread : CachedWindowRect;
    }

    static void ClearCachedWindowRects() {
        CachedWindowRect = FAndroidCachedWindowRectParams();
        CachedWindowRect_EventThread = FAndroidCachedWindowRectParams();
    }

    static int32_t GSurfaceViewWidth = -1;
    static int32_t GSurfaceViewHeight = -1;

    void *AndroidWindow::NativeWindow = NULL;

    AndroidWindow::~AndroidWindow() {
        //       Use NativeWindow_Destroy() instead.
    }
    AndroidWindow::AndroidWindow() {
    }

    void AndroidWindow::Init()
    {

    }
    void AndroidWindow::OnUpdate()
    {
        /*glfwSwapBuffers(m_Window);
        glfwPollEvents();*/




    }
    void AndroidWindow::SetCursorCallBack()
    {

    }
    bool AndroidWindow::ShouldClose()
    {

        return false;
    }
    bool AndroidWindow::IsKeyPressed(int keycode)
    {

    }
    bool AndroidWindow::IsMouseButtonPressed(int button)
    {
    }
    std::pair<float, float> AndroidWindow::GetMousePosition()
    {

    }

    math::vector<int, 2> AndroidWindow::GetCurWindowSize()
    {
        RHIRect ScreenRect = GetScreenRect();
        return {ScreenRect.width(), ScreenRect.height()};
    }


    std::shared_ptr<AndroidWindow> AndroidWindow::Make() {
        //return std::shared_ptr<AndroidWindow>(new AndroidWindow());
        GE_ASSERT(0);
        //TODO::
        return nullptr;
    }


    void AndroidWindow::Initialize(class AndroidApplication *const Application,
                                    //const std::shared_ptr<FGenericWindowDefinition> &InDefinition,
                                    const std::shared_ptr <AndroidWindow> &InParent, const bool bShowImmediately) {
        OwningApplication = Application;
       // Definition = InDefinition;
    }

    bool AndroidWindow::GetFullScreenInfo(int32_t &X, int32_t &Y, int32_t &Width, int32_t &Height) const {
        RHIRect ScreenRect = GetScreenRect();

//        X = ScreenRect.Left;
//        Y = ScreenRect.Top;
//        Width = ScreenRect.Right - ScreenRect.Left;
//        Height = ScreenRect.Bottom - ScreenRect.Top;

        return true;
    }


    void AndroidWindow::SetOSWindowHandle(void *InWindow) {
        // not expecting a window to be supplied on android.
        GE_ASSERT(InWindow == nullptr);
    }


//This function is declared in the Java-defined class, GameActivity.java: "public native void nativeSetObbInfo(String PackageName, int Version, int PatchVersion);"
    static bool GAndroidIsPortrait = false;
    static int GWindowOrientation = -1;
    static std::atomic<bool> GAndroidSafezoneRequiresUpdate = false;
    static int GAndroidDepthBufferPreference = 0;
    static math::float4 GAndroidPortraitSafezone = math::float4(-1.0f, -1.0f, -1.0f, -1.0f);
    static math::float4 GAndroidLandscapeSafezone = math::float4(-1.0f, -1.0f, -1.0f, -1.0f);
#if USE_ANDROID_JNI
    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetWindowInfo(JNIEnv* jenv, jobject thiz, jint orientation, jint DepthBufferPreference, jint PropagateAlpha)
    {
        ClearCachedWindowRects();
        GWindowOrientation = orientation;
        bool bIsPortrait = GWindowOrientation == EAndroidConfigurationOrientation::ORIENTATION_PORTRAIT;
        GAndroidIsPortrait = bIsPortrait == JNI_TRUE;
        GAndroidDepthBufferPreference = DepthBufferPreference;
        GAndroidPropagateAlpha = PropagateAlpha;
        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("App is running in %s\n"), GAndroidIsPortrait ? TEXT("Portrait") : TEXT("Landscape"));
        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("AndroidPropagateAlpha =  %d\n"), GAndroidPropagateAlpha);
    }

    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetSurfaceViewInfo(JNIEnv* jenv, jobject thiz, jint width, jint height)
    {
        GSurfaceViewWidth = width;
        GSurfaceViewHeight = height;
      //  UE_LOG(LogAndroid, Log, TEXT("nativeSetSurfaceViewInfo width=%d and height=%d"), GSurfaceViewWidth, GSurfaceViewHeight);
    }

    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetSafezoneInfo(JNIEnv* jenv, jobject thiz, jboolean bIsPortrait, jfloat left, jfloat top, jfloat right, jfloat bottom)
    {
        if (bIsPortrait)
        {
//            GAndroidPortraitSafezone.X = left;
//            GAndroidPortraitSafezone.Y = top;
//            GAndroidPortraitSafezone.Z = right;
//            GAndroidPortraitSafezone.W = bottom;
        }
        else
        {
//            GAndroidLandscapeSafezone.X = left;
//            GAndroidLandscapeSafezone.Y = top;
//            GAndroidLandscapeSafezone.Z = right;
//            GAndroidLandscapeSafezone.W = bottom;
        }

        GAndroidSafezoneRequiresUpdate = true;
        //UE_LOG(LogAndroid, Log, TEXT("nativeSetSafezoneInfo bIsPortrait=%d, left=%f, top=%f, right=%f, bottom=%f"), bIsPortrait ? 1 : 0, left, top, right, bottom);
    }
#endif

    bool AndroidWindow::bAreCachedNativeDimensionsValid = false;
    int32_t AndroidWindow::CachedNativeWindowWidth = -1;
    int32_t AndroidWindow::CachedNativeWindowHeight = -1;

    bool AndroidWindow::IsPortraitOrientation() {
        return GAndroidIsPortrait;
    }

    math::float4 AndroidWindow::GetSafezone(bool bPortrait) {
        return bPortrait ? GAndroidPortraitSafezone : GAndroidLandscapeSafezone;
    }

    bool AndroidWindow::SafezoneUpdated() {
        bool bRequiresUpdate = true;
        return GAndroidSafezoneRequiresUpdate.compare_exchange_weak(bRequiresUpdate, false);
    }

    int32_t AndroidWindow::GetDepthBufferPreference() {
        return GAndroidDepthBufferPreference;
    }

    void AndroidWindow::InvalidateCachedScreenRect() {
        ClearCachedWindowRects();
    }

    void AndroidWindow::AcquireWindowRef(ANativeWindow *InWindow) {
#if USE_ANDROID_JNI
        ANativeWindow_acquire(InWindow);
#endif
    }

    void AndroidWindow::ReleaseWindowRef(ANativeWindow *InWindow) {
#if USE_ANDROID_JNI
        ANativeWindow_release(InWindow);
#endif
    }

    void AndroidWindow::SetHardwareWindow_EventThread(void *InWindow) {
#if USE_ANDROID_EVENTS
        GE_ASSERT(IsInAndroidEventThread());
#endif
        NativeWindow = InWindow; //using raw native window handle for now. Could be changed to use AndroidWindow later if needed
    }

    void *AndroidWindow::GetHardwareWindow_EventThread() {
        return NativeWindow;
    }

    bool AndroidWindow::WaitForWindowDimensions() {
        while (!bAreCachedNativeDimensionsValid) {
            if (ShouldEngineExit()
#if USE_ANDROID_EVENTS
                || FAppEventManager::GetInstance()->WaitForEventInQueue(EAppEventState::APP_EVENT_STATE_ON_DESTROY, 0.0f)
#endif
                    ) {
                // Application is shutting down
                return false;
            }
            FPlatformProc::Sleep(0.001f);
        }
        return true;
    }

// To be called during initialization from the event thread.
// once set the dimensions are 'valid' and further changes are updated via FAppEventManager::Tick
    void AndroidWindow::SetWindowDimensions_EventThread(ANativeWindow *DimensionWindow) {
        if (bAreCachedNativeDimensionsValid == false) {
#if USE_ANDROID_JNI
            CachedNativeWindowWidth = ANativeWindow_getWidth(DimensionWindow);
            CachedNativeWindowHeight = ANativeWindow_getHeight(DimensionWindow);
#else // Lumin case.
            int32_t ResWidth, ResHeight;
            if (FPlatformMisc::GetOverrideResolution(ResWidth, ResHeight)) {
                CachedNativeWindowWidth = ResWidth;
                CachedNativeWindowHeight = ResHeight;
            }
#endif
            FPlatformMisc::MemoryBarrier();
            bAreCachedNativeDimensionsValid = true;
        }
    }

// Update the dimensions from FAppEventManager::Tick based on messages posted from the event thread.
    void AndroidWindow::EventManagerUpdateWindowDimensions(int32_t Width, int32_t Height) {
        GE_ASSERT(bAreCachedNativeDimensionsValid);
        GE_ASSERT(Width >= 0 && Height >= 0);

        bool bChanged = CachedNativeWindowWidth != Width || CachedNativeWindowHeight != Height;

        CachedNativeWindowWidth = Width;
        CachedNativeWindowHeight = Height;

        if (bChanged) {
            InvalidateCachedScreenRect();
        }
    }

    void *AndroidWindow::WaitForHardwareWindow() {
        // Sleep if the hardware window isn't currently available.
        // The Window may not exist if the activity is pausing/resuming, in which case we make this thread wait
        // This case will come up frequently as a result of the DON flow in Gvr.
        // Until the app is fully resumed. It would be nicer if this code respected the lifecycle events
        // of an android app instead, but all of those events are handled on a separate thread and it would require
        // significant re-architecturing to do.

        // Before sleeping, we peek into the event manager queue to see if it contains an ON_DESTROY event,
        // in which case, we exit the loop to allow the application to exit before a window has been created.
        // It is not sufficient to GE_ASSERT the IsEngineExitRequested() global function, as the handler reacting to the APP_EVENT_STATE_ON_DESTROY
        // may be running in the same thread as this method and therefore lead to a deadlock.

        void *WindowEventThread = GetHardwareWindow_EventThread();
        while (WindowEventThread == nullptr) {
#if USE_ANDROID_EVENTS
            if (ShouldEngineExit() || FAppEventManager::GetInstance()->WaitForEventInQueue(EAppEventState::APP_EVENT_STATE_ON_DESTROY, 0.0f))
            {
                // Application is shutting down soon, abort the wait and return nullptr
                return nullptr;
            }
#endif
            FPlatformProc::Sleep(0.001f);
            WindowEventThread = GetHardwareWindow_EventThread();
        }
        return WindowEventThread;
    }

#if USE_ANDROID_JNI
    extern bool AndroidThunkCpp_IsOculusMobileApplication();
#endif

    static bool IsCachedRectValid(bool bUseEventThreadWindow, const FAndroidCachedWindowRectParams &TestRect) {
        // window must be valid when bUseEventThreadWindow and null when !bUseEventThreadWindow.
        GE_ASSERT((TestRect.Window_EventThread != nullptr) == bUseEventThreadWindow);

        const FAndroidCachedWindowRectParams &CachedRect = GetCachedRect(bUseEventThreadWindow);

        bool bValidCache = true;

        if (CachedRect.ContentScaleFactor != TestRect.ContentScaleFactor) {
//            FPlatformMisc::LowLevelOutputDebugStringf(
//                    TEXT("***** RequestedContentScaleFactor different %f != %f, not using res cache (%d)"),
//                    TestRect.ContentScaleFactor, CachedWindowRect.ContentScaleFactor, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.MobileResX != TestRect.MobileResX) {
//            FPlatformMisc::LowLevelOutputDebugStringf(
//                    TEXT("***** RequestedMobileResX different %d != %d, not using res cache (%d)"), TestRect.MobileResX,
//                    CachedWindowRect.MobileResX, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.MobileResY != TestRect.MobileResY) {
//            FPlatformMisc::LowLevelOutputDebugStringf(
//                    TEXT("***** RequestedMobileResY different %d != %d, not using res cache (%d)"), TestRect.MobileResY,
//                    CachedWindowRect.MobileResY, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.Window_EventThread != TestRect.Window_EventThread) {
//            FPlatformMisc::LowLevelOutputDebugStringf(TEXT("***** Window different, not using res cache (%d)"),
//                                                      (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.WindowDPI != TestRect.WindowDPI) {
           // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("***** WindowDPI is %d, not using res cache (%d)"),
           //                                           TestRect.WindowDPI, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.SceneMaxDesiredPixelCount != TestRect.SceneMaxDesiredPixelCount) {
           // FPlatformMisc::LowLevelOutputDebugStringf(
           //         TEXT("***** SceneMaxDesiredPixelCount is %d, not using res cache (%d)"),
           //         TestRect.SceneMaxDesiredPixelCount, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.SceneMinDPI != TestRect.SceneMinDPI) {
           // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("***** SceneMinDPI is %d, not using res cache (%d)"),
           //                                           TestRect.SceneMinDPI, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        if (CachedRect.WindowWidth <= 8) {
           // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("***** WindowWidth is %d, not using res cache (%d)"),
           //                                           CachedRect.WindowWidth, (int32_t) bUseEventThreadWindow);
            bValidCache = false;
        }

        return bValidCache;
    }

    void CacheRect(bool bUseEventThreadWindow, const FAndroidCachedWindowRectParams &NewValues) {
        GE_ASSERT(NewValues.Window_EventThread != nullptr || !bUseEventThreadWindow);

        (bUseEventThreadWindow ? CachedWindowRect_EventThread : CachedWindowRect) = NewValues;

        //UE_LOG(LogAndroid, Log, TEXT("***** Cached WindowRect %d, %d (%d)"), NewValues.WindowWidth,
        //       NewValues.WindowHeight, (int32_t) bUseEventThreadWindow);
    }

    struct FAndroidDisplayInfo {
        math::int2 WindowDims;
        float SceneScaleFactor;
    };

    static FAndroidDisplayInfo
    GetAndroidDisplayInfoFromDPITargets(int32_t TargetDPI, int32_t SceneMaxDesiredPixelCount, int32_t LowerLimit3DDPI);

    RHIRect AndroidWindow::GetScreenRect(bool bUseEventThreadWindow) {
        int32_t OverrideResX, OverrideResY;
        // allow a subplatform to dictate resolution - we can't easily subclass AndroidWindow the way its used
//        if (FPlatformMisc::GetOverrideResolution(OverrideResX, OverrideResY)) {
//            RHIRect Rect;
//            Rect.Left = Rect.Top = 0;
//            Rect.Right = OverrideResX;
//            Rect.Bottom = OverrideResY;
//
//            return Rect;
//        }

        // too much of the following code needs JNI things, just assume override
#if !USE_ANDROID_JNI

        UE_LOG(LogAndroid, Fatal,
               TEXT("AndroidWindow::CalculateSurfaceSize currently expedcts non-JNI platforms to override resolution"));
        return RHIRect();
#else

        static const bool bIsOculusMobileApp = AndroidThunkCpp_IsOculusMobileApplication();

        // CSF is a multiplier to 1280x720
        //static IConsoleVariable* CVarScale = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MobileContentScaleFactor"));
        float MobileContentScaleFactor = Configuration::WindowWidth/Configuration::WindowHeight;
        // If the app is for Oculus Mobile then always use 0 as ScaleFactor (to match window size).
        float RequestedContentScaleFactor = bIsOculusMobileApp ? 0.0f :MobileContentScaleFactor;// CVarScale->GetFloat();

        std::string CmdLineCSF;
//        if (FParse::Value(FCommandLine::Get(), TEXT("mcsf="), CmdLineCSF, false))
//        {
//            RequestedContentScaleFactor = FCString::Atof(*CmdLineCSF);
//        }

    //    static IConsoleVariable* CVarResX = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Mobile.DesiredResX"));
     //   static IConsoleVariable* CVarResY = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Mobile.DesiredResY"));
        int32_t RequestedResX = bIsOculusMobileApp ? 0 : Configuration::WindowWidth;//CVarResX->GetInt();
        int32_t RequestedResY = bIsOculusMobileApp ? 0 : Configuration::WindowHeight;//CVarResY->GetInt();

//        std::string CmdLineMDRes;
//        if (FParse::Value(FCommandLine::Get(), TEXT("mobileresx="), CmdLineMDRes, false))
//        {
//            RequestedResX = FCString::Atoi(*CmdLineMDRes);
//        }
//        if (FParse::Value(FCommandLine::Get(), TEXT("mobileresy="), CmdLineMDRes, false))
//        {
//            RequestedResY = FCString::Atoi(*CmdLineMDRes);
//        }

        // since orientation won't change on Android, use cached results if still valid. Different cache is maintained for event_thread flavor.
        ANativeWindow* Window = bUseEventThreadWindow ? (ANativeWindow*)AndroidWindow::GetHardwareWindow_EventThread() : nullptr;
        FAndroidCachedWindowRectParams CurrentParams;
        CurrentParams.ContentScaleFactor = RequestedContentScaleFactor;
        CurrentParams.MobileResX = RequestedResX;
        CurrentParams.MobileResY = RequestedResX;
        CurrentParams.Window_EventThread = Window;
        CurrentParams.WindowDPI = GAndroidWindowDPI;
        CurrentParams.SceneMinDPI = math::min(GAndroid3DSceneMinDPI, GAndroidWindowDPI);
        CurrentParams.SceneMaxDesiredPixelCount = GAndroid3DSceneMaxDesiredPixelCount;

        bool bComputeRect = !IsCachedRectValid(bUseEventThreadWindow, CurrentParams);
        if (bComputeRect)
        {
            // currently hardcoding resolution

            // get the aspect ratio of the physical screen
            int32_t ScreenWidth, ScreenHeight;
            CalculateSurfaceSize(ScreenWidth, ScreenHeight, bUseEventThreadWindow);

           // static auto* MobileHDRCvar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.MobileHDR"));
            const bool bMobileHDR = (MobileHDRCvar== 1);
            //UE_LOG(LogAndroid, Log, TEXT("Mobile HDR: %s"), bMobileHDR ? TEXT("YES") : TEXT("no"));

            if (!bIsOculusMobileApp)
            {
                if (CurrentParams.WindowDPI && RequestedResX == 0 && RequestedResY == 0)
                {
                    FAndroidDisplayInfo Info = GetAndroidDisplayInfoFromDPITargets(CurrentParams.WindowDPI, CurrentParams.SceneMaxDesiredPixelCount, CurrentParams.SceneMinDPI);
                    ScreenWidth = Info.WindowDims.x;
                    ScreenHeight = Info.WindowDims.y;
//                    if(IsInGameThread())
//                    {
//                        static IConsoleVariable* CVarSSP = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SecondaryScreenPercentage.GameViewport"));
//                        CVarSSP->Set((float)Info.SceneScaleFactor * 100.0f);
//                    }
                }
                else
                {
                   // AndroidWindowUtils::ApplyContentScaleFactor(ScreenWidth, ScreenHeight);
                }
            }

            // save for future calls
            CurrentParams.WindowWidth = ScreenWidth;
            CurrentParams.WindowHeight = ScreenHeight;
            CacheRect(bUseEventThreadWindow, CurrentParams);
        }

        const FAndroidCachedWindowRectParams& CachedRect = GetCachedRect(bUseEventThreadWindow);

        // create rect and return
        RHIRect ScreenRect;
//        ScreenRect.Left = 0;
//        ScreenRect.Top = 0;
//        ScreenRect.Right = CachedRect.WindowWidth;
//        ScreenRect.Bottom = CachedRect.WindowHeight;
        ScreenRect.minX = 0;
        ScreenRect.minY = 0;
        ScreenRect.maxX = CachedRect.WindowWidth;
        ScreenRect.maxY = CachedRect.WindowHeight;
        return ScreenRect;
#endif
    }

    void
    AndroidWindow::CalculateSurfaceSize(int32_t &SurfaceWidth, int32_t &SurfaceHeight, bool bUseEventThreadWindow) {
        // allow a subplatform to dictate resolution - we can't easily subclass AndroidWindow the way its used
        if (FPlatformMisc::GetOverrideResolution(SurfaceWidth, SurfaceHeight)) {
            return;
        }

        // too much of the following code needs JNI things, just assume override
#if !USE_ANDROID_JNI

        UE_LOG(LogAndroid, Fatal,
               TEXT("AndroidWindow::CalculateSurfaceSize currently expects non-JNI platforms to override resolution"));

#else

        if (bUseEventThreadWindow)
        {
            GE_ASSERT(IsInAndroidEventThread());
            ANativeWindow* WindowEventThread = (ANativeWindow*)GetHardwareWindow_EventThread();
            GE_ASSERT(WindowEventThread);

            SurfaceWidth = (GSurfaceViewWidth > 0) ? GSurfaceViewWidth : ANativeWindow_getWidth(WindowEventThread);
            SurfaceHeight = (GSurfaceViewHeight > 0) ? GSurfaceViewHeight : ANativeWindow_getHeight(WindowEventThread);
        }
        else
        {
            AndroidWindow::WaitForWindowDimensions();

            SurfaceWidth = (GSurfaceViewWidth > 0) ? GSurfaceViewWidth : CachedNativeWindowWidth;
            SurfaceHeight = (GSurfaceViewHeight > 0) ? GSurfaceViewHeight : CachedNativeWindowHeight;
        }

        // some phones gave it the other way (so, if swap if the app is landscape, but width < height)
        if ((GAndroidIsPortrait && SurfaceWidth > SurfaceHeight) ||
            (!GAndroidIsPortrait && SurfaceWidth < SurfaceHeight))
        {
            std::swap(SurfaceWidth, SurfaceHeight);
        }

        // ensure the size is divisible by a specified amount
        // do not convert to a surface size that is larger than native resolution
        // Mobile VR doesn't need buffer quantization as Unreal never renders directly to the buffer in VR mode.
        static const bool bIsMobileVRApp = AndroidThunkCpp_IsOculusMobileApplication();
        const int DividableBy = bIsMobileVRApp ? 1 : 8;
        SurfaceWidth = (SurfaceWidth / DividableBy) * DividableBy;
        SurfaceHeight = (SurfaceHeight / DividableBy) * DividableBy;
#endif
    }

    bool AndroidWindow::OnWindowOrientationChanged(int Orientation) {
        if (GWindowOrientation != Orientation) {
            GWindowOrientation = Orientation;
            bool bIsPortrait = GWindowOrientation == EAndroidConfigurationOrientation::ORIENTATION_PORTRAIT;
//            UE_LOG(LogAndroid, Log, TEXT("Window orientation changed: %s"),
//                   bIsPortrait ? TEXT("Portrait") : TEXT("Landscape"));
            GAndroidIsPortrait = bIsPortrait;
            return true;
        }
        return false;
    }

    extern std::string AndroidThunkCpp_GetMetaDataString(const std::string &Key);


    math::int2 SanitizeAndroidScreenSize(const math::int2&& MaxScreenDims, const math::int2&& RequestedScreenDims)
    {
        math::int2 SanitizedDims;
        // ensure Width and Height is multiple of 8
        SanitizedDims.x = (RequestedScreenDims.x / 8) * 8;
        SanitizedDims.y = (RequestedScreenDims.y / 8) * 8;

        // clamp to native resolution
        SanitizedDims.x = math::min(SanitizedDims.x, MaxScreenDims.x);
        SanitizedDims.y = math::min(SanitizedDims.y, MaxScreenDims.y);
        return SanitizedDims;
    }

// Sets the Android screen size to
    static FAndroidDisplayInfo
    GetAndroidDisplayInfoFromDPITargets(int32_t TargetDPI, int32_t SceneMaxDesiredPixelCount, int32_t LowerLimit3DDPI) {
        auto StringToMap = [](const std::string &str) {
            std::map <std::string, std::string> mapret;
            std::vector <std::string> OutArray;
            OutArray = CommonFunc::SplitString(str, ';');
            if (OutArray.size() % 2 == 0) {
                for (int i = 0; i < OutArray.size(); i += 2) {
                    mapret.insert({OutArray[i], OutArray[i + 1]});
                }
            }
            return mapret;
        };

        std::string JNIMetrics = AndroidThunkCpp_GetMetaDataString(std::string(("unreal.displaymetrics.metrics")));
        std::string JNIDisplay = AndroidThunkCpp_GetMetaDataString(std::string(("unreal.display")));

        std::map <std::string, std::string> metricsParams = StringToMap(JNIMetrics);
        std::map <std::string, std::string> displayParams = StringToMap(JNIDisplay);
        static const std::string DensityDpiKey(("densityDpi"));
        static const std::string xDpiKey(("xdpi"));
        static const std::string yDpiKey(("ydpi"));
        static const std::string WidthPixelsKey(("realWidth"));
        static const std::string HeightPixelsKey(("realHeight"));

        int32_t DensityDPI = metricsParams.find(("densityDpi"))!= metricsParams.end() ? atoi(
                metricsParams[DensityDpiKey].c_str()) : 0;
        int32_t xdpi = metricsParams.find(("xdpi"))!= metricsParams.end()  ? atoi(metricsParams[xDpiKey].c_str()) : 0;
        int32_t ydpi = metricsParams.find(("ydpi"))!= metricsParams.end() ? atoi(metricsParams[yDpiKey].c_str()) : 0;
        int32_t avgdpi = (xdpi + ydpi) / 2;

//        UE_LOG(LogAndroid, Display,
//               ("AndroidDisplayInfoFromDPITargets : DPI info: DensityDPI %d, dpi %d, xdpi %d, ydpi %d"), DensityDPI,
//               avgdpi, xdpi, ydpi);

        int32_t NativeScreenDensityDPI;
        switch (GAndroidWindowDPIQueryMethod) {
            case 1: {
                NativeScreenDensityDPI = DensityDPI;
                break;
            }
            case 0:
            default: {
                NativeScreenDensityDPI = avgdpi;
                break;
            }
        }

        math::int2 NativeScreenPixelDims(
                displayParams.find(WidthPixelsKey)!= displayParams.end() ?  atoi(displayParams[WidthPixelsKey].c_str()) : 0,
                displayParams.find(HeightPixelsKey) != displayParams.end() ? atoi(displayParams[HeightPixelsKey].c_str())
                                                        : 0
        );

        FAndroidDisplayInfo Info;
        Info.SceneScaleFactor = 1.0f;

        // NativeScreenDensityDPI is approx.
        const float ApproxSystemToDesiredDPIScale = math::min((float) TargetDPI / (float) NativeScreenDensityDPI,
                                                               1.0f);
        Info.WindowDims = math::int2 (
                (int32_t) ((float) NativeScreenPixelDims.x * ApproxSystemToDesiredDPIScale),
                (int32_t) ((float) NativeScreenPixelDims.y * ApproxSystemToDesiredDPIScale));
        math::int2 Sanitized = SanitizeAndroidScreenSize(std::move(NativeScreenPixelDims),
                                                                              math::int2(Info.WindowDims));

        // ignore minor differences from sanitization.
        //float ScaleFromSanitizing = FMath::Sqrt((float)(Sanitized.X * Sanitized.Y) / (float)(Info.WindowDims.X * Info.WindowDims.Y));
        Info.WindowDims = Sanitized;
        //TargetDPI = ScaleFromSanitizing;

//        UE_LOG(LogAndroid, Display, ("AndroidDisplayInfoFromDPITargets : Native screen dpi %d, res %d x %d"),
//               NativeScreenDensityDPI, NativeScreenPixelDims.X, NativeScreenPixelDims.Y);
//        UE_CLOG(TargetDPI <= NativeScreenDensityDPI, LogAndroid, Display,
//                ("AndroidDisplayInfoFromDPITargets : New DPI target %d, window dims %d, %d"), TargetDPI,
//                Info.WindowDims.X, Info.WindowDims.Y);
//        UE_CLOG(TargetDPI > NativeScreenDensityDPI, LogAndroid, Display,
//                ("AndroidDisplayInfoFromDPITargets : TargetDPI too high, using native screen DPI %d, window dims %d, %d"),
//                NativeScreenDensityDPI, Info.WindowDims.X, Info.WindowDims.Y);
        TargetDPI = math::min(TargetDPI, NativeScreenDensityDPI);

        int DesiredPixelCount = Info.WindowDims.x * Info.WindowDims.y;
        if (SceneMaxDesiredPixelCount && DesiredPixelCount > SceneMaxDesiredPixelCount) {
            // if we're going to be pushing too many pixels, scale back 3d scene size to get us to SceneMaxDesiredPixelCount
            Info.SceneScaleFactor = std::sqrt((float) SceneMaxDesiredPixelCount / (float) DesiredPixelCount);
//            UE_LOG(LogAndroid, Warning,
//                   ("AndroidDisplayInfoFromDPITargets : DPI %d has a %d pixels, this exceeds the pixels limit of %d by %d%%. 3d scene target is reduced to %d x %d"),
//                   TargetDPI,
//                   DesiredPixelCount,
//                   SceneMaxDesiredPixelCount,
//                   (uint32)(((float) DesiredPixelCount / (float) SceneMaxDesiredPixelCount) * 100),
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.X * Info.SceneScaleFactor),
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.Y * Info.SceneScaleFactor)
           // );
        }

        // if a min dpi was specified then clamp to that and accept a perf hit for res quality.
        if (LowerLimit3DDPI && (int32_t) ((float) TargetDPI * Info.SceneScaleFactor) < LowerLimit3DDPI) {
            float DPILimitScale = (float) LowerLimit3DDPI / (float) TargetDPI;

//            UE_LOG(LogAndroid, Warning,
//                   ("AndroidDisplayInfoFromDPITargets : 3d scene target of %d DPI is lower than specified limit of %d DPI, increasing scene target dims %dx%d -> %dx%d"),
//                   (uint32) FMath::RoundFromZero((float) TargetDPI * Info.SceneScaleFactor),
//                   LowerLimit3DDPI,
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.X * Info.SceneScaleFactor),
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.Y * Info.SceneScaleFactor),
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.X * DPILimitScale),
//                   (uint32) FMath::RoundFromZero((float) Info.WindowDims.Y * DPILimitScale)
 //           );
            Info.SceneScaleFactor = (float) LowerLimit3DDPI / (float) TargetDPI;
        }

        if (SceneMaxDesiredPixelCount) {
            int FinalSceneTargetPixelCount = (int) ((float) DesiredPixelCount * Info.SceneScaleFactor *
                                                    Info.SceneScaleFactor);
//            UE_LOG(LogAndroid, Display,
//                   ("AndroidDisplayInfoFromDPITargets : SceneTarget Pixel count %d%% of limit."),
//                   (uint32)(((float) FinalSceneTargetPixelCount / (float) SceneMaxDesiredPixelCount) * 100));
        }

        return Info;
    }

}
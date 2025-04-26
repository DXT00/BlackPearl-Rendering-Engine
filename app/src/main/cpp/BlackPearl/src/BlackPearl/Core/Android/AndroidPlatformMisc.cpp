//
// Created by DXT00 on 2025/4/11.
//
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Android/AndroidPlatformMisc.h"
#include "Core/CriticalSection.h"
#include "Core/ScopeLock.h"
#include "Core/Android/AndroidJavaEnv.h"
//#include "Android/AndroidJavaEnv.h"
//#include "HAL/PlatformStackWalk.h"
//#include "Misc/FileHelper.h"
//#include "Misc/App.h"
//#include "Misc/ConfigCacheIni.h"
//#include "Misc/CoreDelegates.h"
//#include "Misc/FeedbackContext.h"
//#include "Misc/OutputDeviceRedirector.h"
//#include "HAL/IConsoleManager.h"
//#include "Modules/ModuleManager.h"
#include <string.h>
#include <dlfcn.h>
#include <sys/statfs.h>
#include <sys/syscall.h>
#include <sched.h>
#include <unistd.h>
#include <jni.h>

//#include "Android/AndroidPlatformCrashContext.h"
//#include "HAL/PlatformMallocCrash.h"
//#include "Android/AndroidJavaMessageBox.h"
//#include "GenericPlatform/GenericPlatformChunkInstall.h"
//
//#include "Misc/Parse.h"
//#include "Internationalization/Regex.h"

#include <android/log.h>

#if USE_ANDROID_INPUT
#include <android/keycodes.h>
#endif
#if USE_ANDROID_JNI
#include <android/asset_manager.h>
//#include <cpu-features.h>
#include <android_native_app_glue.h>

#endif
#include "BlackPearl/Luanch/Luanch.h"

namespace BlackPearl {
    extern int GAndroidPropagateAlpha;

    extern struct android_app* GNativeAndroidApp;
    extern std::string AndroidThunkCpp_GetAndroidId();
    extern std::string AndroidThunkCpp_GetAdvertisingId();
    extern void AndroidThunkCpp_RegisterForRemoteNotifications();
    extern void AndroidThunkJava_AddNetworkListener();
   // extern void AndroidSetupDefaultThreadAffinity();

    extern void AndroidThunkCpp_UnregisterForRemoteNotifications();

    extern bool AndroidThunkCpp_IsAllowedRemoteNotifications();
#if USE_ANDROID_JNI
    extern AAssetManager * AndroidThunkCpp_GetAssetManager();
    extern int32_t GAndroidPackageVersion;
    extern std::vector<int32_t> AndroidThunkCpp_GetSupportedNativeDisplayRefreshRates();
    extern bool AndroidThunkCpp_SetNativeDisplayRefreshRate(int32_t RefreshRate);
    extern int32_t AndroidThunkCpp_GetNativeDisplayRefreshRate();
    extern void AndroidThunkCpp_ShowConsoleWindow();
    extern int32_t AndroidThunkCpp_GetMetaDataInt(const std::string& Key);
    extern void AndroidThunkCpp_Vibrate(int32_t Intensity, int32_t Duration);

#else
#define GAndroidPackageVersion    1
#endif

// run time compatibility information
    std::string FAndroidMisc::AndroidVersion; // version of android we are running eg "4.0.4"
    int32_t FAndroidMisc::AndroidMajorVersion = 0; // integer major version of Android we are running, eg 10
    int32_t FAndroidMisc::TargetSDKVersion = 0; // Target SDK version, eg 29.
    std::string FAndroidMisc::DeviceMake; // make of the device we are running on eg. "samsung"
    std::string FAndroidMisc::DeviceModel; // model of the device we are running on eg "SAMSUNG-SGH-I437"
    std::string FAndroidMisc::DeviceBuildNumber; // platform image build number of device "R16NW.G960NKSU1ARD6"
    std::string FAndroidMisc::OSLanguage; // language code the device is set to eg "deu"

// Build/API level we are running.
    int32_t FAndroidMisc::AndroidBuildVersion = 0;

// Whether or not the system handles the volume buttons (event will still be generated either way)
    bool FAndroidMisc::VolumeButtonsHandledBySystem = true;

// Whether an app restart is needed to free driver allocated memory after precompiling PSOs
    bool FAndroidMisc::bNeedsRestartAfterPSOPrecompile = false;

// Key/Value pair variables from the optional configuration.txt
    std::map <std::string, std::string> FAndroidMisc::ConfigRulesVariables;

    static FCriticalSection AndroidThreadNamesLock;
    static std::map<uint32_t, const char *>AndroidThreadNames;

    //EDeviceScreenOrientation FAndroidMisc::DeviceOrientation = EDeviceScreenOrientation::Unknown;

    extern void AndroidThunkCpp_ForceQuit();

    extern void AndroidThunkCpp_SetOrientation(int32_t Value);

    extern void AndroidThunkCpp_SetCellularPreference(int32_t Value);

    extern int32_t AndroidThunkCpp_GetCellularPreference();

// From AndroidFile.cpp
    extern std::string GFontPathBase;

    static char AndroidCpuThermalSensorFileBuf[256] = "";

//    static void OverrideCpuThermalSensorFileFromCVar(IConsoleVariable *Var) {
//        std::string Override = CVarAndroidCPUThermalSensorFilePath.GetValueOnAnyThread();
//        const int32_t Len = Override.Len();
//        if (Len == 0) {
//            return;
//        }
//
//        if (Len < UE_ARRAY_COUNT(AndroidCpuThermalSensorFileBuf)) {
//            FCStringAnsi::Strcpy(AndroidCpuThermalSensorFileBuf, TCHAR_TO_ANSI(*Override));
//            //UE_LOG(LogAndroid, Display, TEXT("Thermal sensor's filepath was set to `%s`"), *Override);
//            return;
//        }
//
//        //UE_LOG(LogAndroid, Display, TEXT("Thermal sensor's filepath is too long, max path is `%u`"),
//               UE_ARRAY_COUNT(AndroidCpuThermalSensorFileBuf));
//    }

    static void InitCpuThermalSensor() {
//        OverrideCpuThermalSensorFileFromCVar(nullptr);
//        CVarAndroidCPUThermalSensorFilePath->SetOnChangedCallback(
//                FConsoleVariableDelegate::CreateStatic(&OverrideCpuThermalSensorFileFromCVar));
//
//        uint32_t Counter = 0;
//        const uint32_t INVALID_INDEX = -1;
//        uint32_t CPUSensorIndex = INVALID_INDEX;
//        while (true) {
//            char Buf[256] = "";
//            sprintf(Buf, "/sys/devices/virtual/thermal/thermal_zone%u/type", Counter);
//            if (FILE * File = fopen(Buf, "r")) {
//                fgets(Buf, UE_ARRAY_COUNT(Buf), File);
//                fclose(File);
//                char *Ptr = Buf;
//                while (!iscntrl(*Ptr))        // it appears that zone type string ends up with \n symbol
//                {
//                    ++Ptr;
//                }
//                *Ptr = 0;
//
//                if (strstr(Buf, "cpu-") && CPUSensorIndex == INVALID_INDEX) {
//                    CPUSensorIndex = Counter;
//                    FCStringAnsi::Sprintf(AndroidCpuThermalSensorFileBuf,
//                                          "/sys/devices/virtual/thermal/thermal_zone%u/temp", Counter);
//                }
//
//                //UE_LOG(LogAndroid, Display,
//                       TEXT("Detected thermal sensor `%s` at /sys/devices/virtual/thermal/thermal_zone%u/temp"),
//                       ANSI_TO_TCHAR(Buf), Counter);
//                ++Counter;
//            } else {
//                break;
//            }
//        }
//
//        std::vector <std::string> SensorLocations;
//        GConfig->GetArray(TEXT("ThermalSensors"), TEXT("SensorLocations"), SensorLocations, GEngineIni);
//
//        for (uint32_t i = 0; i < SensorLocations.Num(); ++i) {
//            auto ConvertedStr = StringCast<ANSICHAR>(*SensorLocations[i]);
//            const char *SensorFilePath = ConvertedStr.Get();
//            if (FILE * File = fopen(SensorFilePath, "r")) {
//                FCStringAnsi::Strcpy(AndroidCpuThermalSensorFileBuf, SensorFilePath);
//                //UE_LOG(LogAndroid, Display, TEXT("Selecting thermal sensor located at `%s`"), *SensorLocations[i]);
//                fclose(File);
//                return;
//            }
//        }
//
//        if (CPUSensorIndex != INVALID_INDEX) {
//            //UE_LOG(LogAndroid, Display, TEXT("Selecting thermal sensor located at `%s`"),
//                   ANSI_TO_TCHAR(AndroidCpuThermalSensorFileBuf));
//        } else {
//            //UE_LOG(LogAndroid, Display,
//                   TEXT("No CPU thermal sensor was detected. To manually override the sensor path set android.CPUThermalSensorFilePath CVar."));
//        }
    }

    void FAndroidMisc::RequestExit(bool Force, const char *CallSite) {

#if PLATFORM_COMPILER_OPTIMIZATION_PG_PROFILING
        // Write the PGO profiling file on a clean shutdown.
        extern void PGO_WriteFile();
        if (!GIsCriticalError)
        {
            PGO_WriteFile();
            // exit now to avoid a possible second PGO write when AndroidMain exits.
            Force = true;
        }
#endif

        //UE_LOG(LogAndroid, Log, TEXT("FAndroidMisc::RequestExit(%i, %s)"), Force,
        //      CallSite ? CallSite : TEXT("<NoCallSiteInfo>"));
//        if (GLog) {
//            GLog->Flush();
//        }

        if (Force) {
#if USE_ANDROID_JNI
            AndroidThunkCpp_ForceQuit();
#else
            exit(1);
#endif
        } else {
            RequestEngineExit();//("Android RequestExit")
        }
    }

    extern void AndroidThunkCpp_RestartApplication(const std::string &IntentString);

    bool FAndroidMisc::RestartApplication() {
#if USE_ANDROID_JNI
        AndroidThunkCpp_RestartApplication("");
        return true;
#else
        return FGenericPlatformMisc::RestartApplication();
#endif
    }

    void FAndroidMisc::LocalPrint(const char *Message) {
        // Builds for distribution should not have logging in them:
        // http://developer.android.com/tools/publishing/preparing.html#publishing-configure
#if !UE_BUILD_SHIPPING || ENABLE_PGO_PROFILE
        const int MAX_LOG_LENGTH = 4096;
        // not static since may be called by different threads
        wchar_t MessageBuffer[MAX_LOG_LENGTH];

        const char *SourcePtr = Message;
        while (*SourcePtr) {
            wchar_t *WritePtr = MessageBuffer;
            int32_t RemainingSpace = MAX_LOG_LENGTH;
            while (*SourcePtr && --RemainingSpace > 0) {
                if (*SourcePtr == ('\r')) {
                    // If next character is newline, skip it
                    if (*(++SourcePtr) == ('\n'))
                        ++SourcePtr;
                    break;
                } else if (*SourcePtr == ('\n')) {
                    ++SourcePtr;
                    break;
                } else {
                    *WritePtr++ = static_cast<wchar_t>(*SourcePtr++);
                }
            }
            *WritePtr = '\0';
            __android_log_print(ANDROID_LOG_DEBUG, "UE", "%ls", MessageBuffer);
        }
#endif
    }

// Test for device vulkan support.
    static void EstablishVulkanDeviceSupport();

    namespace FAndroidAppEntry {
        extern void PlatformInit();
    }

    void FAndroidMisc::PlatformPreInit() {
        //FAndroidCrashContext::Initialize();
        //FGenericPlatformMisc::PlatformPreInit();
        EstablishVulkanDeviceSupport();
        FAndroidAppEntry::PlatformInit();
    }

    static volatile bool HeadPhonesArePluggedIn = false;

    static FAndroidMisc::FBatteryState CurrentBatteryState;

    static FCriticalSection ReceiversLock;
    static struct {
        int Volume;
        double TimeOfChange;
    } CurrentVolume;

#if USE_ANDROID_JNI
    extern "C"
    {
    
        JNIEXPORT void Java_com_epicgames_unreal_HeadsetReceiver_stateChanged(JNIEnv * jni, jclass clazz, jint state)
        {
           // FPlatformMisc::LowLevelOutputDebugStringf(std::vector("nativeHeadsetEvent(%i)"), state);
            HeadPhonesArePluggedIn = (state == 1);
        }
    
        JNIEXPORT void Java_com_epicgames_unreal_VolumeReceiver_volumeChanged(JNIEnv * jni, jclass clazz, jint volume)
        {
            //FPlatformMisc::LowLevelOutputDebugStringf(std::vector("nativeVolumeEvent(%i)"), volume);
            ReceiversLock.Lock();
            CurrentVolume.Volume = volume;
           // CurrentVolume.TimeOfChange = FApp::GetCurrentTime();
            ReceiversLock.Unlock();
        }
    
        JNIEXPORT void Java_com_epicgames_unreal_BatteryReceiver_dispatchEvent(JNIEnv * jni, jclass clazz, jint status, jint level, jint temperature)
        {
//           // FPlatformMisc::LowLevelOutputDebugStringf(std::vector("nativeBatteryEvent(stat = %i, lvl = %i %, temp = %3.2f \u00B0C)"), status, level, float(temperature)/10.f);
//
//            ReceiversLock.Lock();
//            const bool bWasInLowPowerMode = CurrentBatteryState.Level <= GAndroidLowPowerBatteryThreshold;
//
//            FAndroidMisc::FBatteryState state;
//            state.State = (FAndroidMisc::EBatteryState)status;
//            state.Level = level;
//            state.Temperature = float(temperature)/10.f;
//            CurrentBatteryState = state;
//
//            const bool bIsInLowPowerMode = CurrentBatteryState.Level <= GAndroidLowPowerBatteryThreshold;
//            ReceiversLock.Unlock();
//
//            // When we cross the low power battery level threshold, inform the active application
//            if (bIsInLowPowerMode != bWasInLowPowerMode)
//            {
//                FGraphEventRef LowPowerTask = FFunctionGraphTask::CreateAndDispatchWhenReady([=]()
//                {
//                    //UE_LOG(LogAndroid, Display, std::vector("Low Power Mode Changed: %d"), bIsInLowPowerMode);
//                    FCoreDelegates::OnLowPowerMode.Broadcast(bIsInLowPowerMode);
//                }, TStatId(), NULL, ENamedThreads::GameThread);
//            }
//        }
    }
#endif

#if USE_ANDROID_JNI

    // Manage Java side OS event receivers.
    static struct
    {
        const char*		ClazzName;
        JNINativeMethod	Jnim;
        jclass			Clazz;
        jmethodID		StartReceiver;
        jmethodID		StopReceiver;
    } JavaEventReceivers[] =
    {
        { "com/example/blackpearl/VolumeReceiver",{ "volumeChanged", "(I)V",  (void *)Java_com_epicgames_unreal_VolumeReceiver_volumeChanged } },
        { "com/example/blackpearl/BatteryReceiver",{ "dispatchEvent", "(III)V",(void *)Java_com_epicgames_unreal_BatteryReceiver_dispatchEvent } },
        { "com/example/blackpearl/HeadsetReceiver",{ "stateChanged",  "(I)V",  (void *)Java_com_epicgames_unreal_HeadsetReceiver_stateChanged } },
    };
    
    void InitializeJavaEventReceivers()
    {
        // Register natives to receive Volume, Battery, Headphones events
        JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
        if (nullptr != JEnv)
        {
            auto CheckJNIExceptions = [&JEnv]()
            {
                if (JEnv->ExceptionCheck())
                {
                    JEnv->ExceptionDescribe();
                    JEnv->ExceptionClear();
                }
            };
            auto GetStaticMethod = [&JEnv, &CheckJNIExceptions](const char* MethodName, jclass Clazz, const char* ClazzName)
            {
                jmethodID Method = JEnv->GetStaticMethodID(Clazz, MethodName, "(Landroid/app/Activity;)V");
                if (Method == 0)
                {
                    //UE_LOG(LogAndroid, Error, std::vector("Can't find method %s of class %s"), ANSI_TO_TCHAR(MethodName), ANSI_TO_TCHAR(ClazzName));
                }
                CheckJNIExceptions();
                return Method;
            };
    
            for (auto& JavaEventReceiver : JavaEventReceivers)
            {
                JavaEventReceiver.Clazz = AndroidJavaEnv::FindJavaClassGlobalRef(JavaEventReceiver.ClazzName);
                if (JavaEventReceiver.Clazz == nullptr)
                {
                    //UE_LOG(LogAndroid, Error, std::vector("Can't find class for %s"), ANSI_TO_TCHAR(JavaEventReceiver.ClazzName));
                    continue;
                }
                if (JNI_OK != JEnv->RegisterNatives(JavaEventReceiver.Clazz, &JavaEventReceiver.Jnim, 1))
                {
                    //UE_LOG(LogAndroid, Error, std::vector("RegisterNatives failed for %s on %s"), ANSI_TO_TCHAR(JavaEventReceiver.ClazzName), ANSI_TO_TCHAR(JavaEventReceiver.Jnim.name));
                    CheckJNIExceptions();
                }
                JavaEventReceiver.StartReceiver = GetStaticMethod("startReceiver", JavaEventReceiver.Clazz, JavaEventReceiver.ClazzName);
                JavaEventReceiver.StopReceiver = GetStaticMethod("stopReceiver", JavaEventReceiver.Clazz, JavaEventReceiver.ClazzName);
            }
        }
        else
        {
            //UE_LOG(LogAndroid, Warning, std::vector("Failed to initialize java event receivers. JNIEnv is not valid."));
        }
    }
    
    void EnableJavaEventReceivers(bool bEnableReceivers)
    {
        JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
        if (nullptr != JEnv)
        {
            for (auto& JavaEventReceiver : JavaEventReceivers)
            {
                jmethodID methodId = bEnableReceivers ? JavaEventReceiver.StartReceiver : JavaEventReceiver.StopReceiver;
                if (methodId != 0)
                {
                    JEnv->CallStaticVoidMethod(JavaEventReceiver.Clazz, methodId, GNativeAndroidApp->activity->clazz);
                }
            }
        }
    }

#endif    //USE_ANDROID_JNI


   //static FDelegateHandle AndroidOnBackgroundBinding;
  //  static FDelegateHandle AndroidOnForegroundBinding;

#if (STATS || ENABLE_STATNAMEDEVENTS)

    static void StartTraceMarkers()
    {
        if (FAndroidMisc::TraceMarkerFileDescriptor != -1)
        {
            //UE_LOG(LogAndroid, Warning, std::vector("Systrace event logging already open."));
            return;
        }
    
        // Setup trace file descriptor
        FAndroidMisc::TraceMarkerFileDescriptor = open("/sys/kernel/debug/tracing/trace_marker", O_WRONLY);
        if (FAndroidMisc::TraceMarkerFileDescriptor == -1)
        {
            //UE_LOG(LogAndroid, Warning, std::vector("Trace Marker failed to open; systrace support disabled"));
        }
        else
        {
            //UE_LOG(LogAndroid, Display, std::vector("Started systrace events logging."));
        }
    }
    
    static void StopTraceMarkers()
    {
        // Tear down trace file descriptor
        if (FAndroidMisc::TraceMarkerFileDescriptor != -1)
        {
            close(FAndroidMisc::TraceMarkerFileDescriptor);
            FAndroidMisc::TraceMarkerFileDescriptor = -1;
            //UE_LOG(LogAndroid, Display, std::vector("Stopped systrace events logging."));
        }
    }
    
    static void UpdateTraceMarkersEnable(IConsoleVariable* Var)
    {
        if (!GAndroidTraceMarkersEnabled)
        {
            StopTraceMarkers();
        }
        else
        {
            StartTraceMarkers();
        }
    }
#endif

    void FAndroidMisc::PlatformInit() {
        // Increase the maximum number of simultaneously open files
        // Display Timer resolution.
        // Get swap file info
        // Display memory info
        // Setup user specified thread affinity if any
     //   AndroidSetupDefaultThreadAffinity();

#if (STATS || ENABLE_STATNAMEDEVENTS)
        //Loading NDK libandroid.so atrace functions, available in the android libraries way before NDK headers.
        void* const LibAndroid = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
        if (LibAndroid != nullptr)
        {
            // Retrieve function pointers from shared object.
            ATrace_beginSection = reinterpret_cast<ATrace_beginSection_Type>(dlsym(LibAndroid, "ATrace_beginSection"));
            ATrace_endSection = reinterpret_cast<ATrace_endSection_Type>(dlsym(LibAndroid, "ATrace_endSection"));
            ATrace_isEnabled = 	reinterpret_cast<ATrace_isEnabled_Type>(dlsym(LibAndroid, "ATrace_isEnabled"));
        }
    
        if (!ATrace_beginSection || !ATrace_endSection || !ATrace_isEnabled)
        {
            //UE_LOG(LogAndroid, Warning, std::vector("Failed to use native systrace functionality."));
            ATrace_beginSection = nullptr;
            ATrace_endSection = nullptr;
            ATrace_isEnabled = nullptr;
    
            if (FParse::Param(FCommandLine::Get(), std::vector("enablesystrace")))
            {
                GAndroidTraceMarkersEnabled = 1;
            }
    
            if (GAndroidTraceMarkersEnabled)
            {
                StartTraceMarkers();
            }
    
            // Watch for CVar update
            CAndroidTraceMarkersEnabled->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UpdateTraceMarkersEnable));
        }
        else
        {
            bUseNativeSystrace = true;
        }
#endif

#if USE_ANDROID_JNI
        InitializeJavaEventReceivers();
       // AndroidOnBackgroundBinding = FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddStatic(EnableJavaEventReceivers, false);
      //  AndroidOnForegroundBinding = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddStatic(EnableJavaEventReceivers, true);
    
        AndroidThunkJava_AddNetworkListener();
#endif


        InitCpuThermalSensor();
    }

    extern void AndroidThunkCpp_DismissSplashScreen();

    void FAndroidMisc::PlatformTearDown() {
#if (STATS || ENABLE_STATNAMEDEVENTS)
        StopTraceMarkers();
#endif

//        auto RemoveBinding = [](TMulticastDelegate<void()> &ApplicationLifetimeDelegate,
//                                FDelegateHandle &DelegateBinding) {
//            if (DelegateBinding.IsValid()) {
//                ApplicationLifetimeDelegate.Remove(DelegateBinding);
//                DelegateBinding.Reset();
//            }
//        };

      //  RemoveBinding(FCoreDelegates::ApplicationWillEnterBackgroundDelegate, AndroidOnBackgroundBinding);
      //  RemoveBinding(FCoreDelegates::ApplicationHasEnteredForegroundDelegate, AndroidOnForegroundBinding);
    }

    void FAndroidMisc::UpdateDeviceOrientation() {
//        //QUICK_SCOPE_CYCLE_COUNTER(STAT_FAndroidMisc_UpdateDeviceOrientation);
//#if USE_ANDROID_JNI
//        JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
//        if (JEnv)
//        {
//            static jmethodID getRotationMethod = 0;
//            static jmethodID getOrientationMethod = 0;
//
//            if (getRotationMethod == 0 || getOrientationMethod == 0)
//            {
//                jclass MainClass = AndroidJavaEnv::FindJavaClassGlobalRef("com/example/blackpearl/MainNativeActivity");
//                if (MainClass != nullptr)
//                {
//                    getRotationMethod = JEnv->GetMethodID(MainClass, "AndroidThunkJava_GetDeviceRotation", "()I");
//                    getOrientationMethod = JEnv->GetMethodID(MainClass, "AndroidThunkJava_GetConfigurationOrientation", "()I");
//                    JEnv->DeleteGlobalRef(MainClass);
//                }
//            }
//
//            if (getRotationMethod != 0 && getOrientationMethod != 0)
//            {
//                const int Rotation = JEnv->CallIntMethod(AndroidJavaEnv::GetGameActivityThis(), getRotationMethod);
//                const int Orientation = JEnv->CallIntMethod(AndroidJavaEnv::GetGameActivityThis(), getOrientationMethod);
//                if (Orientation == EAndroidConfigurationOrientation::ORIENTATION_PORTRAIT)
//                {
//                    switch (Rotation)
//                    {
//                    case EAndroidSurfaceRotation::ROTATION_0:	DeviceOrientation = EDeviceScreenOrientation::Portrait;             break;
//                    case EAndroidSurfaceRotation::ROTATION_90:	DeviceOrientation = EDeviceScreenOrientation::LandscapeLeft;        break;
//                    case EAndroidSurfaceRotation::ROTATION_180:	DeviceOrientation = EDeviceScreenOrientation::PortraitUpsideDown;   break;
//                    case EAndroidSurfaceRotation::ROTATION_270:	DeviceOrientation = EDeviceScreenOrientation::LandscapeRight;       break;
//                    }
//                }
//                else if (Orientation == EAndroidConfigurationOrientation::ORIENTATION_LANDSCAPE)
//                {
//                    switch (Rotation)
//                    {
//                    case EAndroidSurfaceRotation::ROTATION_0:	DeviceOrientation = EDeviceScreenOrientation::LandscapeLeft;        break;
//                    case EAndroidSurfaceRotation::ROTATION_90:	DeviceOrientation = EDeviceScreenOrientation::PortraitUpsideDown;				break;
//                    case EAndroidSurfaceRotation::ROTATION_180:	DeviceOrientation = EDeviceScreenOrientation::LandscapeRight;		break;
//                    case EAndroidSurfaceRotation::ROTATION_270:	DeviceOrientation = EDeviceScreenOrientation::Portrait;   break;
//                    }
//                }
//            }
//        }
//#endif
    }

//    void FAndroidMisc::PlatformHandleSplashScreen(bool ShowSplashScreen) {
//#if USE_ANDROID_JNI
//        if (!ShowSplashScreen)
//        {
//            AndroidThunkCpp_DismissSplashScreen();
//        }
//        // Update the device orientation in case the game thread is blocked
//        FAndroidMisc::UpdateDeviceOrientation();
//#endif
//    }

    void FAndroidMisc::GetEnvironmentVariable(const char *VariableName, char *Result, int32_t ResultLength) {
        *Result = 0;
        // @todo Android : get environment variable.
    }

    std::string FAndroidMisc::GetEnvironmentVariable(const char *VariableName) {
        // @todo Android : get environment variable.
        return std::string();
    }

    const char *FAndroidMisc::GetSystemErrorMessage(char *OutBuffer, int32_t BufferCount, int32_t Error) {
        GE_ASSERT(OutBuffer && BufferCount);
        *OutBuffer = ('\0');
        if (Error == 0) {
            Error = errno;
        }
        char ErrorBuffer[1024];
//        if (strerror_r(Error, ErrorBuffer, 1024) == 0) {
//            FCString::Strcpy(OutBuffer, BufferCount, UTF8_TO_TCHAR((const ANSICHAR *) ErrorBuffer));
//        } else {
//            *OutBuffer =('\0');
//        }
        return OutBuffer;
    }
//
//    EAppReturnType::Type
//    FAndroidMisc::MessageBoxExt(EAppMsgType::Type MsgType, const char *Text, const char *Caption) {
//#if USE_ANDROID_JNI
//        FJavaAndroidMessageBox MessageBox;
//        MessageBox.SetText(Text);
//        MessageBox.SetCaption(Caption);
//        EAppReturnType::Type * ResultValues = nullptr;
//        static EAppReturnType::Type ResultsOk[] = {
//            EAppReturnType::Ok };
//        static EAppReturnType::Type ResultsYesNo[] = {
//            EAppReturnType::Yes, EAppReturnType::No };
//        static EAppReturnType::Type ResultsOkCancel[] = {
//            EAppReturnType::Ok, EAppReturnType::Cancel };
//        static EAppReturnType::Type ResultsYesNoCancel[] = {
//            EAppReturnType::Yes, EAppReturnType::No, EAppReturnType::Cancel };
//        static EAppReturnType::Type ResultsCancelRetryContinue[] = {
//            EAppReturnType::Cancel, EAppReturnType::Retry, EAppReturnType::Continue };
//        static EAppReturnType::Type ResultsYesNoYesAllNoAll[] = {
//            EAppReturnType::Yes, EAppReturnType::No, EAppReturnType::YesAll,
//            EAppReturnType::NoAll };
//        static EAppReturnType::Type ResultsYesNoYesAllNoAllCancel[] = {
//            EAppReturnType::Yes, EAppReturnType::No, EAppReturnType::YesAll,
//            EAppReturnType::NoAll, EAppReturnType::Cancel };
//        static EAppReturnType::Type ResultsYesNoYesAll[] = {
//            EAppReturnType::Yes, EAppReturnType::No, EAppReturnType::YesAll };
//
//        // TODO: Should we localize button text?
//
//        switch (MsgType)
//        {
//        case EAppMsgType::Ok:
//            MessageBox.AddButton(std::vector("Ok"));
//            ResultValues = ResultsOk;
//            break;
//        case EAppMsgType::YesNo:
//            MessageBox.AddButton(std::vector("Yes"));
//            MessageBox.AddButton(std::vector("No"));
//            ResultValues = ResultsYesNo;
//            break;
//        case EAppMsgType::OkCancel:
//            MessageBox.AddButton(std::vector("Ok"));
//            MessageBox.AddButton(std::vector("Cancel"));
//            ResultValues = ResultsOkCancel;
//            break;
//        case EAppMsgType::YesNoCancel:
//            MessageBox.AddButton(std::vector("Yes"));
//            MessageBox.AddButton(std::vector("No"));
//            MessageBox.AddButton(std::vector("Cancel"));
//            ResultValues = ResultsYesNoCancel;
//            break;
//        case EAppMsgType::CancelRetryContinue:
//            MessageBox.AddButton(std::vector("Cancel"));
//            MessageBox.AddButton(std::vector("Retry"));
//            MessageBox.AddButton(std::vector("Continue"));
//            ResultValues = ResultsCancelRetryContinue;
//            break;
//        case EAppMsgType::YesNoYesAllNoAll:
//            MessageBox.AddButton(std::vector("Yes"));
//            MessageBox.AddButton(std::vector("No"));
//            MessageBox.AddButton(std::vector("Yes To All"));
//            MessageBox.AddButton(std::vector("No To All"));
//            ResultValues = ResultsYesNoYesAllNoAll;
//            break;
//        case EAppMsgType::YesNoYesAllNoAllCancel:
//            MessageBox.AddButton(std::vector("Yes"));
//            MessageBox.AddButton(std::vector("No"));
//            MessageBox.AddButton(std::vector("Yes To All"));
//            MessageBox.AddButton(std::vector("No To All"));
//            MessageBox.AddButton(std::vector("Cancel"));
//            ResultValues = ResultsYesNoYesAllNoAllCancel;
//            break;
//        case EAppMsgType::YesNoYesAll:
//            MessageBox.AddButton(std::vector("Yes"));
//            MessageBox.AddButton(std::vector("No"));
//            MessageBox.AddButton(std::vector("Yes To All"));
//            ResultValues = ResultsYesNoYesAll;
//            break;
//        default:
//            GE_ASSERT(0);
//        }
//        int32_t Choice = MessageBox.Show();
//        if (Choice >= 0 && nullptr != ResultValues)
//        {
//            return ResultValues[Choice];
//        }
//#endif
//
//        // Failed to show dialog, or failed to get a response,
//        // return default cancel response instead.
//        return FGenericPlatformMisc::MessageBoxExt(MsgType, Text, Caption);
//    }
//
//    bool FAndroidMisc::HasPlatformFeature(const char *FeatureName) {
//        if (FCString::Stricmp(FeatureName, std::vector("Vulkan")) == 0) {
//            return FAndroidMisc::ShouldUseVulkan();
//        }
//
//        return FGenericPlatformMisc::HasPlatformFeature(FeatureName);
//    }
//
//    bool FAndroidMisc::UseRenderThread() {
//        // if we in general don't want to use the render thread due to commandline, etc, then don't
//        if (!FGenericPlatformMisc::UseRenderThread()) {
//            return false;
//        }
//
//        // Check for DisableThreadedRendering CVar from DeviceProfiles config
//        // Any devices in the future that need to disable threaded rendering should be given a device profile and use this CVar
//        const IConsoleVariable *const CVar = IConsoleManager::Get().FindConsoleVariable(
//                std::vector("r.AndroidDisableThreadedRendering"));
//        if (CVar && CVar->GetInt() != 0) {
//            return false;
//        }
//
//        // there is a crash with the nvidia tegra dual core processors namely the optimus 2x and xoom
//        // when running multithreaded it can't handle multiple threads using opengl (bug)
//        // tested with lg optimus 2x and motorola xoom
//        // come back and revisit this later
//        // https://code.google.com/p/android/issues/detail?id=32636
//        if (FAndroidMisc::GetGPUFamily() == std::string(std::vector("NVIDIA Tegra")) && FPlatformMisc::NumberOfCores() <= 2 &&
//            FAndroidMisc::GetGLVersion().StartsWith(std::vector("OpenGL ES 2."))) {
//            return false;
//        }
//
//        // Vivante GC1000 with 2.x driver has issues with render thread
//        if (FAndroidMisc::GetGPUFamily().StartsWith(std::vector("Vivante GC1000")) &&
//            FAndroidMisc::GetGLVersion().StartsWith(std::vector("OpenGL ES 2."))) {
//            return false;
//        }
//
//        // there is an issue with presenting the buffer on kindle fire (1st gen) with multiple threads using opengl
//        if (FAndroidMisc::GetDeviceModel() == std::string(std::vector("Kindle Fire"))) {
//            return false;
//        }
//
//        // there is an issue with swapbuffer ordering on startup on samsung s3 mini with multiple threads using opengl
//        if (FAndroidMisc::GetDeviceModel() == std::string(std::vector("GT-I8190L"))) {
//            return false;
//        }
//
//        return true;
//    }
//
//    int32_t FAndroidMisc::NumberOfCores() {
//#if USE_ANDROID_JNI
//        int32_t NumberOfCores = android_getCpuCount();
//#else
//        int32_t NumberOfCores = 0;
//#endif
//
//        static int CalculatedNumberOfCores = 0;
//        if (CalculatedNumberOfCores == 0) {
//            pid_t ThreadId = gettid();
//            cpu_set_t cpuset;
//            CPU_ZERO(&cpuset);
//            if (sched_getaffinity(ThreadId, sizeof(cpuset), &cpuset) != -1) {
//                CalculatedNumberOfCores = CPU_COUNT(&cpuset);
//            }
//
//            ////UE_LOG(LogTemp, Log, std::vector("%d cores and %d assignable cores"), NumberOfCores, CalculatedNumberOfCores);
//        }
//
//        return !CalculatedNumberOfCores ? NumberOfCores : CalculatedNumberOfCores;
//    }

//    int32_t FAndroidMisc::NumberOfCoresIncludingHyperthreads() {
//        return NumberOfCores();
//    }


    static FAndroidMisc::FCPUState CurrentCPUState;

    FAndroidMisc::FCPUState &FAndroidMisc::GetCPUState() {
        uint64_t UserTime, NiceTime, SystemTime, SoftIRQTime, IRQTime, IdleTime, IOWaitTime;
        int32_t Index = 0;
        char Buffer[500];

//        CurrentCPUState.CoreCount = math::min(FAndroidMisc::NumberOfCores(),
//                                               FAndroidMisc::FCPUState::MaxSupportedCores);
//        FILE *FileHandle = fopen("/proc/stat", "r");
//        if (FileHandle) {
//            CurrentCPUState.ActivatedCoreCount = 0;
//            for (size_t n = 0; n < CurrentCPUState.CoreCount; n++) {
//                CurrentCPUState.Status[n] = 0;
//                CurrentCPUState.PreviousUsage[n] = CurrentCPUState.CurrentUsage[n];
//            }
//
//            while (fgets(Buffer, 100, FileHandle)) {
//#if PLATFORM_64BITS
//                sscanf(Buffer, "%5s %8lu %8lu %8lu %8lu %8lu %8lu %8lu", CurrentCPUState.Name,
//                    &UserTime, &NiceTime, &SystemTime, &IdleTime, &IOWaitTime, &IRQTime,
//                    &SoftIRQTime);
//#else
//                sscanf(Buffer, "%5s %8llu %8llu %8llu %8llu %8llu %8llu %8llu", CurrentCPUState.Name,
//                       &UserTime, &NiceTime, &SystemTime, &IdleTime, &IOWaitTime, &IRQTime,
//                       &SoftIRQTime);
//#endif
//
//                if (0 == strncmp(CurrentCPUState.Name, "cpu", 3)) {
//                    Index = CurrentCPUState.Name[3] - '0';
//                    if (Index >= 0 && Index < CurrentCPUState.CoreCount) {
//                        if (CurrentCPUState.Name[5] != '\0') {
//                            Index = atol(&CurrentCPUState.Name[3]);
//                        }
//                        CurrentCPUState.CurrentUsage[Index].IdleTime = IdleTime;
//                        CurrentCPUState.CurrentUsage[Index].NiceTime = NiceTime;
//                        CurrentCPUState.CurrentUsage[Index].SystemTime = SystemTime;
//                        CurrentCPUState.CurrentUsage[Index].SoftIRQTime = SoftIRQTime;
//                        CurrentCPUState.CurrentUsage[Index].IRQTime = IRQTime;
//                        CurrentCPUState.CurrentUsage[Index].IOWaitTime = IOWaitTime;
//                        CurrentCPUState.CurrentUsage[Index].UserTime = UserTime;
//                        CurrentCPUState.CurrentUsage[Index].TotalTime =
//                                UserTime + NiceTime + SystemTime + SoftIRQTime + IRQTime + IdleTime + IOWaitTime;
//                        CurrentCPUState.Status[Index] = 1;
//                        CurrentCPUState.ActivatedCoreCount++;
//                    }
//                    if (Index == CurrentCPUState.CoreCount - 1)
//                        break;
//                }
//            }
//            fclose(FileHandle);
//
//            uint64_t WallTime;
//            double CPULoad[CurrentCPUState.CoreCount];
//            CurrentCPUState.AverageUtilization = 0.0;
//            for (size_t n = 0; n < CurrentCPUState.CoreCount; n++) {
//                if (CurrentCPUState.CurrentUsage[n].TotalTime <= CurrentCPUState.PreviousUsage[n].TotalTime) {
//                    CPULoad[n] = 0;
//                    continue;
//                }
//
//                WallTime = CurrentCPUState.CurrentUsage[n].TotalTime - CurrentCPUState.PreviousUsage[n].TotalTime;
//                IdleTime = CurrentCPUState.CurrentUsage[n].IdleTime - CurrentCPUState.PreviousUsage[n].IdleTime;
//
//                if (!WallTime || WallTime <= IdleTime) {
//                    CPULoad[n] = 0;
//                    continue;
//                }
//                CPULoad[n] = ((double) WallTime - (double) IdleTime) * 100.0 / (double) WallTime;
//                CurrentCPUState.Utilization[n] = CPULoad[n];
//                CurrentCPUState.AverageUtilization += CPULoad[n];
//            }
//            CurrentCPUState.AverageUtilization /= (double) CurrentCPUState.CoreCount;
//        } else {
//            FMemory::Memzero(CurrentCPUState);
//        }
        return CurrentCPUState;
    }


    bool FAndroidMisc::SupportsLocalCaching() {
        return true;

        /*if ( SupportsUTime() )
        {
            return true;
        }*/


    }

    static int SysGetRandomSupported = -1;

// http://man7.org/linux/man-pages/man2/getrandom.2.html
// getrandom() was introduced in version 3.17 of the Linux kernel
//   and glibc version 2.25.

// Check known platforms if SYS_getrandom isn't defined
#if !defined(SYS_getrandom)
#if PLATFORM_CPU_X86_FAMILY && PLATFORM_64BITS
#define SYS_getrandom 318
#elif PLATFORM_CPU_X86_FAMILY && !PLATFORM_64BITS
#define SYS_getrandom 355
#elif PLATFORM_CPU_ARM_FAMILY && PLATFORM_64BITS
#define SYS_getrandom 278
#elif PLATFORM_CPU_ARM_FAMILY && !PLATFORM_64BITS
#define SYS_getrandom 384
#endif
#endif // !defined(SYS_getrandom)

    namespace {
#if defined(SYS_getrandom)

#if !defined(GRND_NONBLOCK)
#define GRND_NONBLOCK 0x0001
#endif

        int SysGetRandom(void *buf, size_t buflen) {
            if (SysGetRandomSupported < 0) {
                int Ret = syscall(SYS_getrandom, buf, buflen, GRND_NONBLOCK);

                // If -1 is returned with ENOSYS, kernel doesn't support getrandom
                SysGetRandomSupported = ((Ret == -1) && (errno == ENOSYS)) ? 0 : 1;
            }

            return SysGetRandomSupported ?
                   syscall(SYS_getrandom, buf, buflen, GRND_NONBLOCK) : -1;
        }

#else

        int SysGetRandom(void *buf, size_t buflen)
        {
            return -1;
        }

#endif // !SYS_getrandom
    }

/**
 * Try to use SYS_getrandom which would be the fastest, otherwise fall back to 
 * use /proc/sys/kernel/random/uuid to get GUID; do NOT use JNI since this may be called too early
 */
//    void FAndroidMisc::CreateGuid(struct FGuid &Result) {
//        QUICK_SCOPE_CYCLE_COUNTER(STAT_FGenericPlatformMisc_CreateGuid);
//
//        static bool bGetRandomFailed = false;
//        static bool bProcUUIDFailed = false;
//
//        if (!bGetRandomFailed) {
//            int BytesRead = SysGetRandom(&Result, sizeof(Result));
//
//            if (BytesRead == sizeof(Result)) {
//                // https://tools.ietf.org/html/rfc4122#section-4.4
//                // https://en.wikipedia.org/wiki/Universally_unique_identifier
//                //
//                // The 4 bits of digit M indicate the UUID version, and the 1â€“3
//                //   most significant bits of digit N indicate the UUID variant.
//                // xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
//                Result[1] = (Result[1] & 0xffff0fff) | 0x00004000; // version 4
//                Result[2] = (Result[2] & 0x3fffffff) | 0x80000000; // variant 1
//                return;
//            }
//            bGetRandomFailed = true;
//        }
//
//#define FROM_HEX(_a) ( (_a) <= '9' ? (_a) - '0' : (_a) <= 'F' ?  (_a) - 'A' + 10 : (_a) - 'a' + 10 )
//
//        if (!bProcUUIDFailed) {
//            int32_t Handle = open("/proc/sys/kernel/random/uuid", O_RDONLY);
//            if (Handle != -1) {
//                char LineBuffer[36];
//                int ReadBytes = read(Handle, LineBuffer, 36);
//                close(Handle);
//                if (ReadBytes == 36) {
//                    Result.A = FROM_HEX(LineBuffer[0]) << 28 | FROM_HEX(LineBuffer[1]) << 24 |
//                               FROM_HEX(LineBuffer[2]) << 20 | FROM_HEX(LineBuffer[3]) << 16 |
//                               FROM_HEX(LineBuffer[4]) << 12 | FROM_HEX(LineBuffer[5]) << 8 |
//                               FROM_HEX(LineBuffer[6]) << 4 | FROM_HEX(LineBuffer[7]);
//                    Result.B = FROM_HEX(LineBuffer[9]) << 28 | FROM_HEX(LineBuffer[10]) << 24 |
//                               FROM_HEX(LineBuffer[11]) << 20 | FROM_HEX(LineBuffer[12]) << 16 |
//                               FROM_HEX(LineBuffer[14]) << 12 | FROM_HEX(LineBuffer[15]) << 8 |
//                               FROM_HEX(LineBuffer[16]) << 4 | FROM_HEX(LineBuffer[17]);
//                    Result.C = FROM_HEX(LineBuffer[19]) << 28 | FROM_HEX(LineBuffer[20]) << 24 |
//                               FROM_HEX(LineBuffer[21]) << 20 | FROM_HEX(LineBuffer[22]) << 16 |
//                               FROM_HEX(LineBuffer[24]) << 12 | FROM_HEX(LineBuffer[25]) << 8 |
//                               FROM_HEX(LineBuffer[26]) << 4 | FROM_HEX(LineBuffer[27]);
//                    Result.D = FROM_HEX(LineBuffer[28]) << 28 | FROM_HEX(LineBuffer[29]) << 24 |
//                               FROM_HEX(LineBuffer[30]) << 20 | FROM_HEX(LineBuffer[31]) << 16 |
//                               FROM_HEX(LineBuffer[32]) << 12 | FROM_HEX(LineBuffer[33]) << 8 |
//                               FROM_HEX(LineBuffer[34]) << 4 | FROM_HEX(LineBuffer[35]);
//                    return;
//                }
//            }
//            bProcUUIDFailed = true;
//        }
//
//#undef FROM_HEX
//
//        // fall back to generic CreateGuid
//        FGenericPlatformMisc::CreateGuid(Result);
//    }
//
///**
// * Good enough default crash reporter.
// */
//    void DefaultCrashHandler(const FAndroidCrashContext &Context) {
//        static int32_t bHasEntered = 0;
//        if (FPlatformAtomics::InterlockedCompareExchange(&bHasEntered, 1, 0) == 0) {
//            const SIZE_T StackTraceSize = 65535;
//            ANSICHAR StackTrace[StackTraceSize];
//            StackTrace[0] = 0;
//
//            FPlatformMisc::LowLevelOutputDebugStringf(std::vector("Starting StackWalk..."));
//
//            // Walk the stack and dump it to the allocated memory.
//            FPlatformStackWalk::StackWalkAndDump(StackTrace, StackTraceSize, 0, Context.Context);
//            ////UE_LOG(LogAndroid, Error, std::vector("\n%s\n"), ANSI_TO_TCHAR(StackTrace));
//
//            if (GLog) {
//                GLog->Panic();
//            }
//
//            if (GWarn) {
//                GWarn->Flush();
//            }
//        }
//    }
//
///** Global pointer to crash handler */
//    void (*GCrashHandlerPointer)(const FGenericCrashContext &Context) = NULL;
//
//    static constexpr int32_t TargetSignals[] =
//            {
//                    SIGQUIT, // SIGQUIT is a user-initiated "crash".
//                    SIGILL,
//                    SIGFPE,
//                    SIGBUS,
//                    SIGSEGV,
//                    SIGSYS,
//                    SIGABRT
//            };
//    static constexpr int32_t NumTargetSignals = UE_ARRAY_COUNT(TargetSignals);
//
//    static const char *SignalToString(int32_t Signal) {
//        switch (Signal) {
//            case SIGQUIT:
//                return "SIGQUIT";
//            case SIGILL:
//                return "SIGILL";
//            case SIGFPE:
//                return "SIGFPE";
//            case SIGBUS:
//                return "SIGBUS";
//            case SIGSEGV:
//                return "SIGSEGV";
//            case SIGSYS:
//                return "SIGSYS";
//            case SIGABRT:
//                return "SIGABRT";
//            default:
//                return FAndroidCrashContext::ItoANSI(Signal, 16, 16);
//        }
//    }

#if ANDROID_HAS_RTSIGNALS

    float GAndroidSignalTimeOut = 20.0f;
    static FAutoConsoleVariableRef CAndroidSignalTimeout(
        std::vector("android.SignalTimeout"),
        GAndroidSignalTimeOut,
        std::vector("Time in seconds to wait for the signal handler to complete before timing out and terminating the process."),
        ECVF_Default
    );
    
    template<typename Derived >
    typename FSignalHandler< Derived >::FSignalParams FSignalHandler< Derived >::SignalParams;
    template<typename Derived >
    int32_t FSignalHandler< Derived >::SignalThreadStatus = (int32_t)FSignalHandler< Derived >::ESignalThreadStatus::NotInitialized;
    template<typename Derived >
    uint32_t FSignalHandler< Derived >::ForwardingThreadID = 0xffffffff;
    template<typename Derived >
    int32_t FSignalHandler< Derived >::ForwardingSignalType = -1;
    template<typename Derived >
    struct sigaction FSignalHandler< Derived >::PreviousActionForForwardSignal;
    
    
    class FThreadCallstackSignalHandler : FSignalHandler<FThreadCallstackSignalHandler>
    {
        friend class FSignalHandler<FThreadCallstackSignalHandler>;
    public:
        static void Init()
        {
            FSignalHandler<FThreadCallstackSignalHandler>::Init(THREADBACKTRACE_SIGNAL_FWD);
            HookTargetSignal();
        }
    
        static void Release()
        {
            RestorePreviousTargetSignalHandler();
            FSignalHandler<FThreadCallstackSignalHandler>::Release();
        }
    
    private:
        static void OnTargetSignal(int Signal, siginfo* Info, void* Context)
        {
            while (FPlatformAtomics::InterlockedCompareExchange(&handling_signal, 1, 0) != 0)
            {
                FPlatformProcess::SleepNoStats(0.0f);
            }
            FSignalHandler<FThreadCallstackSignalHandler>::ForwardSignal(Signal, Info, Context);
            FPlatformAtomics::AtomicStore(&handling_signal, 0);
        }
    
        static void HandleTargetSignal(int Signal, siginfo* Info, void* Context, uint32_t CrashingThreadId)
        {
            FPlatformStackWalk::HandleBackTraceSignal(Info, Context);
        }
    
        static void HookTargetSignal()
        {
            GE_ASSERT(bSignalHooked == false);
            struct sigaction ActionForThread;
            FMemory::Memzero(ActionForThread);
            sigfillset(&ActionForThread.sa_mask);
            ActionForThread.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
            ActionForThread.sa_sigaction = &OnTargetSignal;
            sigaction(THREAD_CALLSTACK_GENERATOR, &ActionForThread, &PreviousActionForThreadGenerator);
            bSignalHooked = true;
        }
    
        static void RestorePreviousTargetSignalHandler()
        {
            if (bSignalHooked)
            {
                bSignalHooked = false;
                sigaction(THREAD_CALLSTACK_GENERATOR, &PreviousActionForThreadGenerator, nullptr);
            }
        }
    
        static bool bSignalHooked;
        static volatile sig_atomic_t handling_signal;
        static struct sigaction PreviousActionForThreadGenerator;
    };
    
    const ANSICHAR* FAndroidMisc::CodeToString(int Signal, int si_code)
    {
        switch (Signal)
        {
            case SIGILL:
            {
                switch (si_code)
                {
                    // SIGILL
                    case ILL_ILLOPC: return "ILL_ILLOPC";
                    case ILL_ILLOPN: return "ILL_ILLOPN";
                    case ILL_ILLADR: return "ILL_ILLADR";
                    case ILL_ILLTRP: return "ILL_ILLTRP";
                    case ILL_PRVOPC: return "ILL_PRVOPC";
                    case ILL_PRVREG: return "ILL_PRVREG";
                    case ILL_COPROC: return "ILL_COPROC";
                    case ILL_BADSTK: return "ILL_BADSTK";
                }
            }
            break;
            case SIGFPE:
            {
                switch (si_code)
                {
                    // SIGFPE
                    case FPE_INTDIV: return "FPE_INTDIV";
                    case FPE_INTOVF: return "FPE_INTOVF";
                    case FPE_FLTDIV: return "FPE_FLTDIV";
                    case FPE_FLTOVF: return "FPE_FLTOVF";
                    case FPE_FLTUND: return "FPE_FLTUND";
                    case FPE_FLTRES: return "FPE_FLTRES";
                    case FPE_FLTINV: return "FPE_FLTINV";
                    case FPE_FLTSUB: return "FPE_FLTSUB";
                }
            }
            break;
            case SIGBUS:
            {
                switch (si_code)
                {
                    // SIGBUS
                    case BUS_ADRALN: return "BUS_ADRALN";
                    case BUS_ADRERR: return "BUS_ADRERR";
                    case BUS_OBJERR: return "BUS_OBJERR";
                }
            }
            break;
            case SIGSEGV:
            {
                switch (si_code)
                {
                    // SIGSEGV
                    case SEGV_MAPERR: return "SEGV_MAPERR";
                    case SEGV_ACCERR: return "SEGV_ACCERR";
                }
            }
            break;
        }
        return FAndroidCrashContext::ItoANSI(si_code, 10, 0);
    }
    
    std::string FAndroidMisc::GetFatalSignalMessage(int Signal, siginfo* Info)
    {
        const int MessageSize = 255;
        char AnsiMessage[MessageSize];
        FCStringAnsi::Strncpy(AnsiMessage, "Caught signal : ", MessageSize);
        FCStringAnsi::Strcat(AnsiMessage, SignalToString(Signal));
        FCStringAnsi::Strcat(AnsiMessage, " (");
        FCStringAnsi::Strcat(AnsiMessage, CodeToString(Signal, Info->si_code));
        FCStringAnsi::Strcat(AnsiMessage, ")");
        switch (Signal)
        {
            case SIGILL:
            case SIGFPE:
            case SIGSEGV:
            case SIGBUS:
            case SIGTRAP:
            {
                FCStringAnsi::Strcat(AnsiMessage, " fault address 0x");
                FCStringAnsi::Strcat(AnsiMessage, FAndroidCrashContext::ItoANSI((uintptr_t)Info->si_addr, 16, 16));
                break;
            }
        }
    
        return ANSI_TO_TCHAR(AnsiMessage);
    }
    
    // Making the signal handler available to track down issues with failing crash handler.
    static void (*GFatalSignalHandlerOverrideFunc)(int Signal, struct siginfo* Info, void* Context, uint32_t CrashingThreadId) = nullptr;
    void FAndroidMisc::OverrideFatalSignalHandler(void (*FatalSignalHandlerOverrideFunc)(int Signal, struct siginfo* Info, void* Context, uint32_t CrashingThreadId))
    {
        GFatalSignalHandlerOverrideFunc = FatalSignalHandlerOverrideFunc;
    }
    
    volatile sig_atomic_t FThreadCallstackSignalHandler::handling_signal = 0;
    bool FThreadCallstackSignalHandler::bSignalHooked = false;
    struct sigaction FThreadCallstackSignalHandler::PreviousActionForThreadGenerator;
    
    class FFatalSignalHandler : public FSignalHandler<FFatalSignalHandler>
    {
        friend class FSignalHandler<FFatalSignalHandler>;
    public:
        static void Init()
        {
            FSignalHandler<FFatalSignalHandler>::Init(FATAL_SIGNAL_FWD);
            HookTargetSignals();
        }
    
        static void Release()
        {
            RestorePreviousTargetSignalHandlers();
            FSignalHandler<FFatalSignalHandler>::Release();
        }
    
        static bool IsInFatalSignalHandler()
        {
            return FPlatformAtomics::AtomicRead(&handling_fatal_signal) > 0;
        }
    
    protected:
    
        static void EnterFatalCrash()
        {
            // we are a fatal signal, we can only handle one at a time. So avoid allow multiple fatal signals going through
            if (FPlatformAtomics::InterlockedIncrement(&handling_fatal_signal) != 1)
            {
                FPlatformProcess::SleepNoStats(60.0f);
                // exit immediately, crash malloc can cause deadlocks when attempting to clean up static objects via exit().
                _exit(1);
            }
        }
    
        static void OnTargetSignal(int Signal, siginfo* Info, void* Context)
        {
            EnterFatalCrash();
            FSignalHandler<FFatalSignalHandler>::ForwardSignal(Signal, Info, Context);
            RestorePreviousTargetSignalHandlers();
    
            // re-raise the signal for the benefit of the previous handler.
            raise(Signal);
        }
    
        static void HandleTargetSignal(int Signal, siginfo* Info, void* Context, uint32_t CrashingThreadId)
        {
            if (GFatalSignalHandlerOverrideFunc)
            {
                GFatalSignalHandlerOverrideFunc(Signal, Info, Context, CrashingThreadId);
            }
            else
            {
                // Switch to malloc crash.
                FPlatformMallocCrash::Get().SetAsGMalloc();
    
                std::string Message = FAndroidMisc::GetFatalSignalMessage(Signal, Info);
                FAndroidCrashContext CrashContext(ECrashContextType::Crash, *Message);
    
                CrashContext.InitFromSignal(Signal, Info, Context, CrashingThreadId);
                CrashContext.CaptureCrashInfo();
                if (GCrashHandlerPointer)
                {
                    GCrashHandlerPointer(CrashContext);
                }
                else
                {
                    // call default one
                    DefaultCrashHandler(CrashContext);
                }
            }
        }
    
        static void HookTargetSignals()
        {
            GE_ASSERT(PreviousSignalHandlersValid == false);
            // hook our signals and record current set.
            struct sigaction Action;
            FMemory::Memzero(&Action, sizeof(struct sigaction));
            Action.sa_sigaction = &OnTargetSignal;
            // sigfillset will block all other signals whilst the signal handler is processing.
            sigfillset(&Action.sa_mask);
            Action.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
    
            for (int32_t i = 0; i < NumTargetSignals; ++i)
            {
                int result = sigaction(TargetSignals[i], &Action, &PrevActions[i]);
                UE_CLOG(result != 0, LogAndroid, Error, std::vector("sigaction(%d) failed to set: %d, errno = %x "), i, result, errno);
            }
            PreviousSignalHandlersValid = true;
        }
    
        static void RestorePreviousTargetSignalHandlers()
        {
            if (PreviousSignalHandlersValid)
            {
                for (int32_t i = 0; i < NumTargetSignals; ++i)
                {
                    int result = sigaction(TargetSignals[i], &PrevActions[i], NULL);
                    UE_CLOG(result != 0, LogAndroid, Error, std::vector("sigaction(%d) failed to set prev action: %d, errno = %x "), i, result, errno);
                }
                PreviousSignalHandlersValid = false;
            }
        }
    
        static volatile sig_atomic_t handling_fatal_signal;
        static struct sigaction PrevActions[NumTargetSignals];
        static bool PreviousSignalHandlersValid;
    };
    
    volatile sig_atomic_t FFatalSignalHandler::handling_fatal_signal = 0;
    struct sigaction FFatalSignalHandler::PrevActions[NumTargetSignals];
    bool FFatalSignalHandler::PreviousSignalHandlersValid = false;
#endif// ANDROID_HAS_RTSIGNALS

//    static void SetDefaultSignalHandlers() {
//        struct sigaction Action;
//        FMemory::Memzero(&Action, sizeof(struct sigaction));
//        Action.sa_handler = SIG_DFL;
//        sigemptyset(&Action.sa_mask);
//
//        for (int32_t i = 0; i < NumTargetSignals; ++i) {
//            sigaction(TargetSignals[i], &Action, NULL);
//        }
//    }

    bool FAndroidMisc::IsInSignalHandler() {
#if ANDROID_HAS_RTSIGNALS
        return FFatalSignalHandler::IsInFatalSignalHandler();
#else
        return false;
#endif
    }

//    void FAndroidMisc::TriggerCrashHandler(ECrashContextType InType, const char *InErrorMessage,
//                                           const char *OverrideCallstack) {
//        if (InType != ECrashContextType::Crash) {
//            // we dont flush logs during a fatal signal, malloccrash can cause us to deadlock.
//            if (GLog) {
//                GLog->Panic();
//            }
//            if (GWarn) {
//                GWarn->Flush();
//            }
//            if (GError) {
//                GError->Flush();
//            }
//        }
//
//        FAndroidCrashContext CrashContext(InType, InErrorMessage);
//
//        if (OverrideCallstack) {
//            CrashContext.SetOverrideCallstack(OverrideCallstack);
//        } else {
//            CrashContext.CaptureCrashInfo();
//        }
//
//        if (GCrashHandlerPointer) {
//            GCrashHandlerPointer(CrashContext);
//        } else {
//            // call default one
//            DefaultCrashHandler(CrashContext);
//        }
//    }
//
//    void FAndroidMisc::SetCrashHandler(void(*CrashHandler)(const FGenericCrashContext &Context)) {
//#if ANDROID_HAS_RTSIGNALS
//        //UE_LOG(LogAndroid, Log, std::vector("Setting Crash Handler = %p"), CrashHandler);
//
//        GCrashHandlerPointer = CrashHandler;
//
//        FFatalSignalHandler::Release();
//        FThreadCallstackSignalHandler::Release();
//        // Passing -1 will leave these restored and won't trap them
//        if ((PTRINT)CrashHandler == -1)
//        {
//            return;
//        }
//
//        FFatalSignalHandler::Init();
//        FThreadCallstackSignalHandler::Init();
//#endif
//    }

    bool FAndroidMisc::GetUseVirtualJoysticks() {
        // joystick on commandline means don't require virtual joysticks
//        if (FParse::Param(FCommandLine::Get(), std::vector("joystick"))) {
//            return false;
//        }
//
//        // Amazon Fire TV doesn't require virtual joysticks
//        if (FAndroidMisc::GetDeviceMake() == std::string("Amazon")) {
//            if (FAndroidMisc::GetDeviceModel().StartsWith(std::vector("AFT"))) {
//                return false;
//            }
//        }
//
//        // Oculus HMDs don't require virtual joysticks
//        if (FAndroidMisc::GetDeviceMake() == std::string("Oculus")) {
//            return false;
//        }

        return true;
    }

    bool FAndroidMisc::SupportsTouchInput() {
//        // Amazon Fire TV doesn't support touch input
//        if (FAndroidMisc::GetDeviceMake() == std::string("Amazon")) {
//            if (FAndroidMisc::GetDeviceModel().StartsWith(std::vector("AFT"))) {
//                return false;
//            }
//        }
//
//        // Oculus HMDs don't support touch input
//        if (FAndroidMisc::GetDeviceMake() == std::string("Oculus")) {
//            return false;
//        }

        return true;
    }



    void FAndroidMisc::RegisterForRemoteNotifications() {
#if USE_ANDROID_JNI
        AndroidThunkCpp_RegisterForRemoteNotifications();
#endif
    }

    void FAndroidMisc::UnregisterForRemoteNotifications() {
#if USE_ANDROID_JNI
        AndroidThunkCpp_UnregisterForRemoteNotifications();
#endif
    }

    bool FAndroidMisc::IsAllowedRemoteNotifications() {
#if USE_ANDROID_JNI
        return AndroidThunkCpp_IsAllowedRemoteNotifications();
#else
        return false;
#endif
    }
//
//    std::vector <uint8_t> FAndroidMisc::GetSystemFontBytes() {
//#if USE_ANDROID_FILE
//        std::vector<uint8_t> FontBytes;
//        static std::string FullFontPath = GFontPathBase + std::string(std::vector("DroidSans.ttf"));
//        FFileHelper::LoadFileToArray(FontBytes, *FullFontPath);
//        return FontBytes;
//#else
//        return FGenericPlatformMisc::GetSystemFontBytes();
//#endif
//    }

//    class IPlatformChunkInstall *FAndroidMisc::GetPlatformChunkInstall() {
//        static IPlatformChunkInstall *ChunkInstall = nullptr;
//        static bool bIniChecked = false;
//        if (!ChunkInstall || !bIniChecked) {
//            std::string ProviderName;
//            IPlatformChunkInstallModule *PlatformChunkInstallModule = nullptr;
//            if (!GEngineIni.IsEmpty()) {
//                std::string InstallModule;
//                GConfig->GetString(std::vector("StreamingInstall"), std::vector("DefaultProviderName"), InstallModule, GEngineIni);
//                FModuleStatus Status;
//                if (FModuleManager::Get().QueryModule(*InstallModule, Status)) {
//                    PlatformChunkInstallModule = FModuleManager::LoadModulePtr<IPlatformChunkInstallModule>(
//                            *InstallModule);
//                    if (PlatformChunkInstallModule != nullptr) {
//                        // Attempt to grab the platform installer
//                        ChunkInstall = PlatformChunkInstallModule->GetPlatformChunkInstall();
//                    }
//                }
//                bIniChecked = true;
//            }
//            if (!ChunkInstall) {
//                // Placeholder instance
//                ChunkInstall = FGenericPlatformMisc::GetPlatformChunkInstall();
//            }
//        }
//
//        return ChunkInstall;
//    }
//
//    void FAndroidMisc::PrepareMobileHaptics(EMobileHapticsType Type) {
//    }

    void FAndroidMisc::TriggerMobileHaptics() {
#if USE_ANDROID_JNI
        // directly play a small vibration one-shot
        // note: this will do nothing if device is already playing force feedback (non-zero intensity)
        // but will play and not be cancelled by force feedback since it only sends updates when not already above zero
        AndroidThunkCpp_Vibrate(255, 10);
#endif
    }

    void FAndroidMisc::ReleaseMobileHaptics() {

    }

//    void
//    FAndroidMisc::ShareURL(const std::string &URL, const FText &Description, int32_t LocationHintX, int32_t LocationHintY) {
//#if USE_ANDROID_JNI
//        extern void AndroidThunkCpp_ShareURL(const std::string& URL, const FText& Description, const FText& SharePrompt, int32_t LocationHintX, int32_t LocationHintY);
//        AndroidThunkCpp_ShareURL(URL, Description, NSLOCTEXT("AndroidMisc", "ShareURL", "Share URL"), LocationHintX, LocationHintY);
//#endif
//    }
//
//    std::string FAndroidMisc::LoadTextFileFromPlatformPackage(const std::string &RelativePath) {
//#if USE_ANDROID_JNI
//        AAssetManager* AssetMgr = AndroidThunkCpp_GetAssetManager();
//        AAsset* asset = AAssetManager_open(AssetMgr, TCHAR_TO_UTF8(*RelativePath), AASSET_MODE_BUFFER);
//
//        if (asset)
//        {
//            const void* FileContents = (const ANSICHAR*)AAsset_getBuffer(asset);
//            int32_t FileLength = AAsset_getLength(asset);
//
//            std::vector<ANSICHAR> TextContents;
//            TextContents.AddUninitialized(FileLength + 1);
//            FMemory::Memcpy(TextContents.GetData(), FileContents, FileLength);
//            TextContents[FileLength] = 0;
//
//            AAsset_close(asset);
//
//            return std::string(ANSI_TO_TCHAR(TextContents.GetData()));
//        }
//#endif
//        return std::string();
//    }
//
//    bool FAndroidMisc::FileExistsInPlatformPackage(const std::string &RelativePath) {
//#if USE_ANDROID_JNI
//        AAssetManager* AssetMgr = AndroidThunkCpp_GetAssetManager();
//        AAsset* asset = AAssetManager_open(AssetMgr, TCHAR_TO_UTF8(*RelativePath), AASSET_MODE_UNKNOWN);
//        if (asset)
//        {
//            AAsset_close(asset);
//            return true;
//        }
//#endif
//        return false;
//    }

//    void FAndroidMisc::SetVersionInfo(std::string InAndroidVersion, int32_t InTargetSDKVersion, std::string InDeviceMake,
//                                      std::string InDeviceModel, std::string InDeviceBuildNumber, std::string InOSLanguage) {
//        AndroidVersion = InAndroidVersion;
//        AndroidMajorVersion = FCString::Atoi(*InAndroidVersion);
//        TargetSDKVersion = InTargetSDKVersion;
//        DeviceMake = InDeviceMake;
//        DeviceModel = InDeviceModel;
//        DeviceBuildNumber = InDeviceBuildNumber;
//        OSLanguage = InOSLanguage;
//
//        //UE_LOG(LogAndroid, Display, std::vector("Android Version Make Model BuildNumber Language: %s %s %s %s %s"),
//               *AndroidVersion, *DeviceMake, *DeviceModel, *DeviceBuildNumber, *OSLanguage);
//    }

    const std::string FAndroidMisc::GetAndroidVersion() {
        return AndroidVersion;
    }

    int32_t FAndroidMisc::GetAndroidMajorVersion() {
        return AndroidMajorVersion;
    }

    int32_t FAndroidMisc::GetTargetSDKVersion() {
        return TargetSDKVersion;
    }

    const std::string FAndroidMisc::GetDeviceMake() {
        return DeviceMake;
    }

    const std::string FAndroidMisc::GetDeviceModel() {
        return DeviceModel;
    }

    const std::string FAndroidMisc::GetDeviceBuildNumber() {
        return DeviceBuildNumber;
    }

    const std::string FAndroidMisc::GetOSLanguage() {
        return OSLanguage;
    }

//    const std::string FAndroidMisc::GetProjectVersion() {
//        return std::string::FromInt(GAndroidPackageVersion);
//    }

    std::string FAndroidMisc::GetDefaultLocale() {
        return OSLanguage;
    }

    bool FAndroidMisc::GetVolumeButtonsHandledBySystem() {
        return VolumeButtonsHandledBySystem;
    }

    void FAndroidMisc::SetVolumeButtonsHandledBySystem(bool enabled) {
        VolumeButtonsHandledBySystem = enabled;
    }

#if USE_ANDROID_JNI
    int32_t FAndroidMisc::GetAndroidBuildVersion()
    {
        if (AndroidBuildVersion > 0)
        {
            return AndroidBuildVersion;
        }
        if (AndroidBuildVersion <= 0)
        {
            JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
            if (nullptr != JEnv)
            {
                jclass Class = AndroidJavaEnv::FindJavaClassGlobalRef("com/example/blackpearl/MainNativeActivity");
                if (nullptr != Class)
                {
                    jfieldID Field = JEnv->GetStaticFieldID(Class, "ANDROID_BUILD_VERSION", "I");
                    if (nullptr != Field)
                    {
                        AndroidBuildVersion = JEnv->GetStaticIntField(Class, Field);
                    }
                    JEnv->DeleteGlobalRef(Class);
                }
            }
        }
        return AndroidBuildVersion;
    }
#endif

    static bool bForceUnsupported = false;

    void FAndroidMisc::SetForceUnsupported(bool bInOverride) {
        bForceUnsupported = bInOverride;
    }

#if USE_ANDROID_JNI
    bool FAndroidMisc::IsSupportedAndroidDevice()
    {
        static bool bChecked = false;
        static bool bSupported = true;
    
        if (!bChecked)
        {
            bChecked = true;
    
            JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
            if (nullptr != JEnv)
            {
                jclass Class = AndroidJavaEnv::FindJavaClassGlobalRef("com/example/blackpearl/MainNativeActivity");
                if (nullptr != Class)
                {
                    jfieldID Field = JEnv->GetStaticFieldID(Class, "bSupportedDevice", "Z");
                    if (nullptr != Field)
                    {
                        bSupported = (bool)JEnv->GetStaticBooleanField(Class, Field);
                    }
                    JEnv->DeleteGlobalRef(Class);
                }
            }
        }
        return bForceUnsupported ? false : bSupported;
    }
#else

    bool FAndroidMisc::IsSupportedAndroidDevice() {
        return !bForceUnsupported;
    }

#endif

    bool FAndroidMisc::ShouldDisablePluginAtRuntime(const std::string &PluginName) {
#if PLATFORM_ANDROID_ARM64 || PLATFORM_ANDROID_X64
        // disable OnlineSubsystemGooglePlay for unsupported Android architectures
        if (PluginName.Equals(std::vector("OnlineSubsystemGooglePlay")))
        {
            return true;
        }
#endif
        return false;
    }

///////////////////////////////////////////////////////////////////////////////
//
// Extracted from vk_platform.h and vulkan.h with modifications just to allow
// vkCreateInstance/vkDestroyInstance to be called to check if a driver is actually
// available (presence of libvulkan.so only means it may be available, not that
// there is an actual usable one). Cannot wait for VulkanRHI init to do this (too
// late) and vulkan.h header not guaranteed to be available. This part of the header
// is unlikely to change in future so safe enough to use this truncated version.
//

#if PLATFORM_ANDROID_ARM
    // On Android/ARMv7a, Vulkan functions use the armeabi-v7a-hard calling
#define VKAPI_ATTR __attribute__((pcs("aapcs-vfp")))
#define VKAPI_CALL
#define VKAPI_PTR  VKAPI_ATTR
#else
// On other platforms, use the default calling convention
#define VKAPI_ATTR
#define VKAPI_CALL
#define VKAPI_PTR
#endif

#define VK_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

#define VK_VERSION_MAJOR(version) ((uint32_t)(version) >> 22)
#define VK_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3ff)
#define VK_VERSION_PATCH(version) ((uint32_t)(version) & 0xfff)

    typedef uint32_t VkFlags;
    typedef uint32_t VkBool32;

#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;

    VK_DEFINE_HANDLE(VkInstance)
    VK_DEFINE_HANDLE(VkPhysicalDevice)

    typedef enum VkResult {
        VK_SUCCESS = 0,
        VK_NOT_READY = 1,
        VK_TIMEOUT = 2,
        VK_EVENT_SET = 3,
        VK_EVENT_RESET = 4,
        VK_INCOMPLETE = 5,
        VK_ERROR_OUT_OF_HOST_MEMORY = -1,
        VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
        VK_ERROR_INITIALIZATION_FAILED = -3,
        VK_ERROR_DEVICE_LOST = -4,
        VK_ERROR_MEMORY_MAP_FAILED = -5,
        VK_ERROR_LAYER_NOT_PRESENT = -6,
        VK_ERROR_EXTENSION_NOT_PRESENT = -7,
        VK_ERROR_FEATURE_NOT_PRESENT = -8,
        VK_ERROR_INCOMPATIBLE_DRIVER = -9,
        VK_ERROR_TOO_MANY_OBJECTS = -10,
        VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
        VK_ERROR_SURFACE_LOST_KHR = -1000000000,
        VK_ERROR_NATIVE_WINDOW_IN_USE_KHR = -1000000001,
        VK_SUBOPTIMAL_KHR = 1000001003,
        VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
        VK_ERROR_INCOMPATIBLE_DISPLAY_KHR = -1000003001,
        VK_ERROR_VALIDATION_FAILED_EXT = -1000011001,
        VK_ERROR_INVALID_SHADER_NV = -1000012000,
        VK_RESULT_BEGIN_RANGE = VK_ERROR_FORMAT_NOT_SUPPORTED,
        VK_RESULT_END_RANGE = VK_INCOMPLETE,
        VK_RESULT_RANGE_SIZE = (VK_INCOMPLETE - VK_ERROR_FORMAT_NOT_SUPPORTED + 1),
        VK_RESULT_MAX_ENUM = 0x7FFFFFFF
    } VkResult;

    typedef enum VkStructureType {
        VK_STRUCTURE_TYPE_APPLICATION_INFO = 0,
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1,
        VK_STRUCTURE_TYPE_MAX_ENUM = 0x7FFFFFFF
    } VkStructureType;

    typedef VkFlags VkInstanceCreateFlags;

    typedef struct VkApplicationInfo {
        VkStructureType sType;
        const void *pNext;
        const char *pApplicationName;
        uint32_t applicationVersion;
        const char *pEngineName;
        uint engineVersion;
        uint apiVersion;
    } VkApplicationInfo;

    typedef struct VkInstanceCreateInfo {
        VkStructureType sType;
        const void *pNext;
        VkInstanceCreateFlags flags;
        const VkApplicationInfo *pApplicationInfo;
        uint32_t enabledLayerCount;
        const char *const *ppEnabledLayerNames;
        uint32_t enabledExtensionCount;
        const char *const *ppEnabledExtensionNames;
    } VkInstanceCreateInfo;

    typedef struct VkAllocationCallbacks {
        void *pUserData;
        void *pfnAllocation;
        void *pfnReallocation;
        void *pfnFree;
        void *pfnInternalAllocation;
        void *pfnInternalFree;
    } VkAllocationCallbacks;

    typedef uint64_t VkDeviceSize;
    typedef VkFlags VkSampleCountFlags;

    typedef struct VkPhysicalDeviceLimits {
        uint32_t maxImageDimension1D;
        uint32_t maxImageDimension2D;
        uint32_t maxImageDimension3D;
        uint32_t maxImageDimensionCube;
        uint32_t maxImageArrayLayers;
        uint32_t maxTexelBufferElements;
        uint32_t maxUniformBufferRange;
        uint32_t maxStorageBufferRange;
        uint32_t maxPushConstantsSize;
        uint32_t maxMemoryAllocationCount;
        uint32_t maxSamplerAllocationCount;
        VkDeviceSize bufferImageGranularity;
        VkDeviceSize sparseAddressSpaceSize;
        uint32_t maxBoundDescriptorSets;
        uint32_t maxPerStageDescriptorSamplers;
        uint32_t maxPerStageDescriptorUniformBuffers;
        uint32_t maxPerStageDescriptorStorageBuffers;
        uint32_t maxPerStageDescriptorSampledImages;
        uint32_t maxPerStageDescriptorStorageImages;
        uint32_t maxPerStageDescriptorInputAttachments;
        uint32_t maxPerStageResources;
        uint32_t maxDescriptorSetSamplers;
        uint32_t maxDescriptorSetUniformBuffers;
        uint32_t maxDescriptorSetUniformBuffersDynamic;
        uint32_t maxDescriptorSetStorageBuffers;
        uint32_t maxDescriptorSetStorageBuffersDynamic;
        uint32_t maxDescriptorSetSampledImages;
        uint32_t maxDescriptorSetStorageImages;
        uint32_t maxDescriptorSetInputAttachments;
        uint32_t maxVertexInputAttributes;
        uint32_t maxVertexInputBindings;
        uint32_t maxVertexInputAttributeOffset;
        uint32_t maxVertexInputBindingStride;
        uint32_t maxVertexOutputComponents;
        uint32_t maxTessellationGenerationLevel;
        uint32_t maxTessellationPatchSize;
        uint32_t maxTessellationControlPerVertexInputComponents;
        uint32_t maxTessellationControlPerVertexOutputComponents;
        uint32_t maxTessellationControlPerPatchOutputComponents;
        uint32_t maxTessellationControlTotalOutputComponents;
        uint32_t maxTessellationEvaluationInputComponents;
        uint32_t maxTessellationEvaluationOutputComponents;
        uint32_t maxGeometryShaderInvocations;
        uint32_t maxGeometryInputComponents;
        uint32_t maxGeometryOutputComponents;
        uint32_t maxGeometryOutputVertices;
        uint32_t maxGeometryTotalOutputComponents;
        uint32_t maxFragmentInputComponents;
        uint32_t maxFragmentOutputAttachments;
        uint32_t maxFragmentDualSrcAttachments;
        uint32_t maxFragmentCombinedOutputResources;
        uint32_t maxComputeSharedMemorySize;
        uint32_t maxComputeWorkGroupCount[3];
        uint32_t maxComputeWorkGroupInvocations;
        uint32_t maxComputeWorkGroupSize[3];
        uint32_t subPixelPrecisionBits;
        uint32_t subTexelPrecisionBits;
        uint32_t mipmapPrecisionBits;
        uint32_t maxDrawIndexedIndexValue;
        uint32_t maxDrawIndirectCount;
        float maxSamplerLodBias;
        float maxSamplerAnisotropy;
        uint32_t maxViewports;
        uint32_t maxViewportDimensions[2];
        float viewportBoundsRange[2];
        uint32_t viewportSubPixelBits;
        size_t minMemoryMapAlignment;
        VkDeviceSize minTexelBufferOffsetAlignment;
        VkDeviceSize minUniformBufferOffsetAlignment;
        VkDeviceSize minStorageBufferOffsetAlignment;
        int32_t minTexelOffset;
        uint32_t maxTexelOffset;
        int32_t minTexelGatherOffset;
        uint32_t maxTexelGatherOffset;
        float minInterpolationOffset;
        float maxInterpolationOffset;
        uint32_t subPixelInterpolationOffsetBits;
        uint32_t maxFramebufferWidth;
        uint32_t maxFramebufferHeight;
        uint32_t maxFramebufferLayers;
        VkSampleCountFlags framebufferColorSampleCounts;
        VkSampleCountFlags framebufferDepthSampleCounts;
        VkSampleCountFlags framebufferStencilSampleCounts;
        VkSampleCountFlags framebufferNoAttachmentsSampleCounts;
        uint32_t maxColorAttachments;
        VkSampleCountFlags sampledImageColorSampleCounts;
        VkSampleCountFlags sampledImageIntegerSampleCounts;
        VkSampleCountFlags sampledImageDepthSampleCounts;
        VkSampleCountFlags sampledImageStencilSampleCounts;
        VkSampleCountFlags storageImageSampleCounts;
        uint32_t maxSampleMaskWords;
        VkBool32 timestampComputeAndGraphics;
        float timestampPeriod;
        uint32_t maxClipDistances;
        uint32_t maxCullDistances;
        uint32_t maxCombinedClipAndCullDistances;
        uint32_t discreteQueuePriorities;
        float pointSizeRange[2];
        float lineWidthRange[2];
        float pointSizeGranularity;
        float lineWidthGranularity;
        VkBool32 strictLines;
        VkBool32 standardSampleLocations;
        VkDeviceSize optimalBufferCopyOffsetAlignment;
        VkDeviceSize optimalBufferCopyRowPitchAlignment;
        VkDeviceSize nonCoherentAtomSize;
    } VkPhysicalDeviceLimits;

    typedef struct VkPhysicalDeviceSparseProperties {
        VkBool32 residencyStandard2DBlockShape;
        VkBool32 residencyStandard2DMultisampleBlockShape;
        VkBool32 residencyStandard3DBlockShape;
        VkBool32 residencyAlignedMipSize;
        VkBool32 residencyNonResidentStrict;
    } VkPhysicalDeviceSparseProperties;

    typedef enum VkPhysicalDeviceType {
        VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
        VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
        VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
        VK_PHYSICAL_DEVICE_TYPE_BEGIN_RANGE = VK_PHYSICAL_DEVICE_TYPE_OTHER,
        VK_PHYSICAL_DEVICE_TYPE_END_RANGE = VK_PHYSICAL_DEVICE_TYPE_CPU,
        VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE = (VK_PHYSICAL_DEVICE_TYPE_CPU - VK_PHYSICAL_DEVICE_TYPE_OTHER + 1),
        VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM = 0x7FFFFFFF
    } VkPhysicalDeviceType;

#define VK_MAX_PHYSICAL_DEVICE_NAME_SIZE 256
#define VK_UUID_SIZE 16

    typedef struct VkPhysicalDeviceProperties {
        uint32_t apiVersion;
        uint32_t driverVersion;
        uint32_t vendorID;
        uint32_t deviceID;
        VkPhysicalDeviceType deviceType;
        char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
        uint8_t pipelineCacheUUID[VK_UUID_SIZE];
        VkPhysicalDeviceLimits limits;
        VkPhysicalDeviceSparseProperties sparseProperties;
    } VkPhysicalDeviceProperties;

#define VK_MAX_EXTENSION_NAME_SIZE        256
#define VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME "VK_GOOGLE_display_timing"

    typedef struct VkExtensionProperties {
        char extensionName[VK_MAX_EXTENSION_NAME_SIZE];
        uint32_t specVersion;
    } VkExtensionProperties;

    typedef VkResult(VKAPI_PTR *PFN_vkCreateInstance)(const VkInstanceCreateInfo *pCreateInfo,
                                                      const VkAllocationCallbacks *pAllocator, VkInstance *pInstance);

    typedef void (VKAPI_PTR *PFN_vkDestroyInstance)(VkInstance instance, const VkAllocationCallbacks *pAllocator);

    typedef VkResult(VKAPI_PTR *PFN_vkEnumeratePhysicalDevices)(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                                VkPhysicalDevice *pPhysicalDevices);

    typedef void (VKAPI_PTR *PFN_vkGetPhysicalDeviceProperties)(VkPhysicalDevice physicalDevice,
                                                                VkPhysicalDeviceProperties *pProperties);

    typedef VkResult(VKAPI_PTR *PFN_vkEnumerateDeviceExtensionProperties)(VkPhysicalDevice physicalDevice,
                                                                          const char *pLayerName,
                                                                          uint32_t *pPropertyCount,
                                                                          VkExtensionProperties *pProperties);

///////////////////////////////////////////////////////////////////////////////

#define UE_VK_API_VERSION    VK_MAKE_VERSION(1, 1, 0)

    enum class EDeviceVulkanSupportStatus {
        Uninitialized,
        NotSupported,
        Supported
    };

    static std::string VulkanVersionString;
    static EDeviceVulkanSupportStatus VulkanSupport = EDeviceVulkanSupportStatus::Uninitialized;

    static EDeviceVulkanSupportStatus AttemptVulkanInit(void *VulkanLib) {
//        if (VulkanLib == nullptr) {
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//
//        // Try to get required functions to check for driver
//        PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance) dlsym(VulkanLib, "vkCreateInstance");
//        PFN_vkDestroyInstance vkDestroyInstance = (PFN_vkDestroyInstance) dlsym(VulkanLib, "vkDestroyInstance");
//        PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) dlsym(VulkanLib,
//                                                                                                           "vkEnumeratePhysicalDevices");
//        PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) dlsym(
//                VulkanLib, "vkGetPhysicalDeviceProperties");
//        PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) dlsym(
//                VulkanLib, "vkEnumerateDeviceExtensionProperties");
//
//        if (!vkCreateInstance || !vkDestroyInstance || !vkEnumeratePhysicalDevices || !vkGetPhysicalDeviceProperties ||
//            !vkEnumerateDeviceExtensionProperties) {
//            //UE_LOG(LogAndroid, Log,
//                   std::vector("Vulkan not supported: vkCreateInstance: 0x%p, vkDestroyInstance: 0x%p, vkEnumeratePhysicalDevices: 0x%p, vkGetPhysicalDeviceProperties: 0x%p, vkEnumerateDeviceExtensionProperties: 0x%p"),
//                   vkCreateInstance, vkDestroyInstance, vkEnumeratePhysicalDevices, vkGetPhysicalDeviceProperties,
//                   vkEnumerateDeviceExtensionProperties);
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//
//        // try to create instance to verify driver available
//        VkApplicationInfo App;
//        FMemory::Memzero(App);
//        App.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//        App.pApplicationName = "UE";
//        App.applicationVersion = 0;
//        App.pEngineName = "UE";
//        App.engineVersion = 0;
//        App.apiVersion = UE_VK_API_VERSION;
//
//        VkInstanceCreateInfo InstInfo;
//        FMemory::Memzero(InstInfo);
//        InstInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//        InstInfo.pNext = nullptr;
//        InstInfo.pApplicationInfo = &App;
//        InstInfo.enabledExtensionCount = 0;
//        InstInfo.ppEnabledExtensionNames = nullptr;
//
//        VkInstance Instance;
//        VkResult Result = vkCreateInstance(&InstInfo, nullptr, &Instance);
//        if (Result != VK_SUCCESS) {
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//
//        // Determine Vulkan device's API level.
//        uint32_t GpuCount = 0;
//        Result = vkEnumeratePhysicalDevices(Instance, &GpuCount, nullptr);
//        if (Result != VK_SUCCESS || GpuCount == 0) {
//            vkDestroyInstance(Instance, nullptr);
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//
//        std::vector <VkPhysicalDevice> PhysicalDevices;
//        PhysicalDevices.AddZeroed(GpuCount);
//        Result = vkEnumeratePhysicalDevices(Instance, &GpuCount, PhysicalDevices.GetData());
//        if (Result != VK_SUCCESS) {
//            vkDestroyInstance(Instance, nullptr);
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//
//        // Don't care which device - This code is making the assumption that all devices will have same api version.
//        VkPhysicalDeviceProperties DeviceProperties;
//        vkGetPhysicalDeviceProperties(PhysicalDevices[0], &DeviceProperties);
//
//        //for now we are allowing devices without the timing extension to run with a basic CPU frame pacer.
//#if 0
//        bool bHasVKGoogleDisplayTiming = false;
//        {
//            std::vector<VkExtensionProperties> ExtensionProps;
//            do
//            {
//                uint32_t Count = 0;
//                Result = vkEnumerateDeviceExtensionProperties(PhysicalDevices[0], nullptr, &Count, nullptr);
//                GE_ASSERT(Result >= VK_SUCCESS);
//
//                if (Count > 0)
//                {
//                    ExtensionProps.Empty(Count);
//                    ExtensionProps.AddUninitialized(Count);
//                    Result = vkEnumerateDeviceExtensionProperties(PhysicalDevices[0], nullptr, &Count, ExtensionProps.GetData());
//                    GE_ASSERT(Result >= VK_SUCCESS);
//                }
//            } while (Result == VK_INCOMPLETE);
//            GE_ASSERT(Result >= VK_SUCCESS);
//
//
//            for (int32_t i = 0; i < ExtensionProps.Num(); ++i)
//            {
//                //UE_LOG(LogAndroid, Log, std::vector("Checking extension: %s."), ANSI_TO_TCHAR(ExtensionProps[i].extensionName));
//                if (!FCStringAnsi::Strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME, ExtensionProps[i].extensionName))
//                {
//                    bHasVKGoogleDisplayTiming = true;
//                    break;
//                }
//            }
//        }
//        if (!bHasVKGoogleDisplayTiming)
//        {
//            vkDestroyInstance(Instance, nullptr);
//
//            //UE_LOG(LogAndroid, Log, std::vector("Vulkan not supported, cannot find VK_GOOGLE_display_timing extension."));
//            return EDeviceVulkanSupportStatus::NotSupported;
//        }
//#endif
//
//        VulkanVersionString = std::string::Printf(std::vector("%d.%d.%d"), VK_VERSION_MAJOR(DeviceProperties.apiVersion),
//                                              VK_VERSION_MINOR(DeviceProperties.apiVersion),
//                                              VK_VERSION_PATCH(DeviceProperties.apiVersion));
//        vkDestroyInstance(Instance, nullptr);
//
//        return EDeviceVulkanSupportStatus::Supported;

        return EDeviceVulkanSupportStatus::NotSupported;
    }

    bool FAndroidMisc::HasVulkanDriverSupport() {
// @todo Lumin: this isn't really the best #define to check here - but basically, without JNI and other version checking, we can't safely do it - we'll need
// non-JNI platforms that support Vulkan to figure out a way to force it (if they want GL + Vulkan support)
#if !USE_ANDROID_JNI
        VulkanSupport = EDeviceVulkanSupportStatus::NotSupported;
        VulkanVersionString = std::vector("0.0.0");
#else
        // this version does not check for VulkanRHI or disabled by cvars!
        if (VulkanSupport == EDeviceVulkanSupportStatus::Uninitialized)
    
        {
            // assume no
            VulkanSupport = EDeviceVulkanSupportStatus::NotSupported;
            VulkanVersionString = ("0.0.0");
    
            // check for libvulkan.so
            void* VulkanLib = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
            if (VulkanLib != nullptr)
            {
                //UE_LOG(LogAndroid, Log, std::vector("Vulkan library detected, checking for available driver"));
    
                // if Nougat, we can check the Vulkan version
                if (FAndroidMisc::GetAndroidBuildVersion() >= 24)
                {
                    int32_t VulkanVersion = AndroidThunkCpp_GetMetaDataInt(("android.hardware.vulkan.version"));
                    if (VulkanVersion >= UE_VK_API_VERSION)
                    {
                        // final check, try initializing the instance
                        VulkanSupport = AttemptVulkanInit(VulkanLib);
                    }
                }
                else
                {
                    // otherwise, we need to try initializing the instance
                    VulkanSupport = AttemptVulkanInit(VulkanLib);
                }
    
                dlclose(VulkanLib);
    
                if (VulkanSupport == EDeviceVulkanSupportStatus::Supported)
                {
                    //UE_LOG(LogAndroid, Log, std::vector("VulkanRHI is available, Vulkan capable device detected."));
                    return true;
                }
                else
                {
                    //UE_LOG(LogAndroid, Log, std::vector("Vulkan driver NOT available."));
                }
            }
            else
            {
                //UE_LOG(LogAndroid, Log, std::vector("Vulkan library NOT detected."));
            }
        }
#endif
        return VulkanSupport == EDeviceVulkanSupportStatus::Supported;
    }

// Test for device vulkan support.
    static void EstablishVulkanDeviceSupport() {
        // just do this check once
        if (VulkanSupport == EDeviceVulkanSupportStatus::Uninitialized) {
            // this call will initialize VulkanSupport
            FAndroidMisc::HasVulkanDriverSupport();
        }
    }

//    bool IsDesktopVulkanAvailable() {
//        static int CachedDesktopVulkanAvailable = -1;
//
//        if (CachedDesktopVulkanAvailable == -1) {
//            CachedDesktopVulkanAvailable = 0;
//
//            bool bSupportsVulkanSM5 = false;
//
//            GConfig->GetBool(std::vector("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"), std::vector("bSupportsVulkanSM5"),
//                             bSupportsVulkanSM5, GEngineIni);
//
//            if (bSupportsVulkanSM5) {
//                CachedDesktopVulkanAvailable = 1;
//            }
//        }
//
//        return CachedDesktopVulkanAvailable == 1;
//    }
//
//    bool FAndroidMisc::IsVulkanAvailable() {
//        GE_ASSERT(VulkanSupport != EDeviceVulkanSupportStatus::Uninitialized);
//
//        static int CachedVulkanAvailable = -1;
//        if (CachedVulkanAvailable == -1) {
//            CachedVulkanAvailable = 0;
//            if (VulkanSupport == EDeviceVulkanSupportStatus::Supported) {
//                bool bSupportsVulkan = false;
//                GConfig->GetBool(std::vector("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"), std::vector("bSupportsVulkan"),
//                                 bSupportsVulkan, GEngineIni);
//
//                // whether to detect Vulkan by default or require the -detectvulkan command line parameter
//                bool bDetectVulkanByDefault = true;
//                GConfig->GetBool(std::vector("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"),
//                                 std::vector("bDetectVulkanByDefault"), bDetectVulkanByDefault, GEngineIni);
//                const bool bDetectVulkanCmdLine = FParse::Param(FCommandLine::Get(), std::vector("detectvulkan"));
//
//                // @todo Lumin: Double check all this stuff after merging general android Vulkan SM5 from main
//                const bool bSupportsVulkanSM5 = IsDesktopVulkanAvailable();
//
//                const bool bVulkanDisabledCmdLine = FParse::Param(FCommandLine::Get(), std::vector("GL")) ||
//                                                    FParse::Param(FCommandLine::Get(), std::vector("OpenGL"));
//
//                if (!FModuleManager::Get().ModuleExists(std::vector("VulkanRHI"))) {
//                    //UE_LOG(LogAndroid, Log, std::vector("Vulkan not available as VulkanRHI not present."));
//                } else if (!(bSupportsVulkan || bSupportsVulkanSM5)) {
//                    //UE_LOG(LogAndroid, Log,
//                           std::vector("Vulkan not available as project packaged without bSupportsVulkan or bSupportsVulkanSM5."));
//                } else if (bVulkanDisabledCmdLine) {
//                    //UE_LOG(LogAndroid, Log, std::vector("Vulkan API detection is disabled by a command line option."));
//                } else if (!bDetectVulkanByDefault && !bDetectVulkanCmdLine) {
//                    //UE_LOG(LogAndroid, Log,
//                           std::vector("Vulkan available but detection disabled by bDetectVulkanByDefault=False in AndroidRuntimeSettings. Use -detectvulkan to override."));
//                } else {
//                    CachedVulkanAvailable = 1;
//                }
//            }
//        }
//
//        return CachedVulkanAvailable == 1;
//    }
//
//    bool FAndroidMisc::ShouldUseVulkan() {
//        GE_ASSERT(VulkanSupport != EDeviceVulkanSupportStatus::Uninitialized);
//        static int CachedShouldUseVulkan = -1;
//
//        if (CachedShouldUseVulkan == -1) {
//            CachedShouldUseVulkan = 0;
//
//            static const auto CVarDisableVulkan = IConsoleManager::Get().FindTConsoleVariableDataInt(
//                    std::vector("r.Android.DisableVulkanSupport"));
//
//            const bool bVulkanAvailable = IsVulkanAvailable();
//
//            const bool bVulkanDisabledCVar = CVarDisableVulkan->GetValueOnAnyThread() == 1;
//
//            if (bVulkanAvailable && !bVulkanDisabledCVar) {
//                CachedShouldUseVulkan = 1;
//                //UE_LOG(LogAndroid, Log, std::vector("VulkanRHI will be used!"));
//            } else {
//                //UE_LOG(LogAndroid, Log, std::vector("VulkanRHI will NOT be used:"));
//                if (!bVulkanAvailable) {
//                    //UE_LOG(LogAndroid, Log,
//                           std::vector(" ** Vulkan support is not available (Driver, RHI or shaders are missing, or disabled by cmdline, see above logging for details)"));
//                }
//                if (bVulkanDisabledCVar) {
//                    //UE_LOG(LogAndroid, Log, std::vector(" ** Vulkan is disabled via console variable."));
//                }
//                //UE_LOG(LogAndroid, Log, std::vector("OpenGL ES will be used."));
//            }
//        }
//
//        return CachedShouldUseVulkan == 1;
//    }

    bool FAndroidMisc::ShouldUseDesktopVulkan() {
//        static int CachedShouldUseDesktopVulkan = -1;
//
//        if (CachedShouldUseDesktopVulkan == -1) {
//            CachedShouldUseDesktopVulkan = 0;
//
//            const bool bVulkanSM5Enabled = IsDesktopVulkanAvailable();
//
//            static const auto CVarDisableVulkanSM5 = IConsoleManager::Get().FindTConsoleVariableDataInt(
//                    std::vector("r.Android.DisableVulkanSM5Support"));
//            const bool bVulkanSM5Disabled = CVarDisableVulkanSM5->GetValueOnAnyThread() == 1;
//
//            if (bVulkanSM5Enabled && !bVulkanSM5Disabled) {
//                CachedShouldUseDesktopVulkan = 1;
//                //UE_LOG(LogAndroid, Log, std::vector("Vulkan SM5 RHI will be used!"));
//            } else if (bVulkanSM5Disabled) {
//                //UE_LOG(LogAndroid, Log, std::vector("Vulkan SM5 is available but disabled for this device."));
//            } else if (!bVulkanSM5Enabled) {
//                //UE_LOG(LogAndroid, Log,
//                       std::vector("** Vulkan SM5 support is not available (Driver, RHI or shaders are missing, or disabled by cmdline, see above logging for details)"));
//            }
//        }
//
//        return CachedShouldUseDesktopVulkan;

        return false;
    }

    std::string FAndroidMisc::GetVulkanVersion() {
        GE_ASSERT(VulkanSupport != EDeviceVulkanSupportStatus::Uninitialized);
        return VulkanVersionString;
    }

    const std::map <std::string, std::string> &FAndroidMisc::GetConfigRulesTMap() {
        return ConfigRulesVariables;
    }

//    std::string *FAndroidMisc::GetConfigRulesVariable(const std::string &Key) {
//        return ConfigRulesVariables.Find(Key);
//    }

    bool FAndroidMisc::AllowThreadHeartBeat() {
        static uint32_t AllowThreadHeartBeatOnce = -1;
//        if (AllowThreadHeartBeatOnce == -1) {
//            const std::string *AllowThreadHeartBeatConfigVar = GetConfigRulesVariable(("EnableThreadHeartBeat"));
//            if (AllowThreadHeartBeatConfigVar) {
//                AllowThreadHeartBeatOnce = (uint32_t) AllowThreadHeartBeatConfigVar->Equals("true",
//                                                                                          ESearchCase::IgnoreCase);
//            } else {
//                AllowThreadHeartBeatOnce = 0;
//            }
//        }

        return AllowThreadHeartBeatOnce == 1;
    }

    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetConfigRulesVariables(JNIEnv *jenv, jobject thiz,
                                                                                         jobjectArray KeyValuePairs) {
        int32_t Count = jenv->GetArrayLength(KeyValuePairs);
        int32_t Index = 0;
        while (Index < Count) {
            auto javaKey = FJavaHelper::FStringFromLocalRef(jenv, (jstring) (jenv->GetObjectArrayElement(KeyValuePairs,
                                                                                                         Index++)));
            auto javaValue = FJavaHelper::FStringFromLocalRef(jenv,
                                                              (jstring) (jenv->GetObjectArrayElement(KeyValuePairs,
                                                                                                     Index++)));

            FAndroidMisc::ConfigRulesVariables.insert({javaKey, javaValue});
        }
    }

    extern bool AndroidThunkCpp_HasMetaDataKey(const std::string &Key);

    static bool bDetectedDebugger = false;

    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetAndroidStartupState(JNIEnv *jenv, jobject thiz,
                                                                                        jboolean bDebuggerAttached) {
        // if Java debugger attached, mark detected (but don't lose previous trigger state)
        if (bDebuggerAttached) {
            bDetectedDebugger = true;
        }
    }

#if !UE_BUILD_SHIPPING

//    bool FAndroidMisc::IsDebuggerPresent() {
//        if (GIgnoreDebugger) {
//            return false;
//        }
//
//        if (bDetectedDebugger) {
//            return true;
//        }
//
//        // If a process is tracing this one then TracerPid in /proc/self/status will
//        // be the id of the tracing process. Use SignalHandler safe functions
//
//        int StatusFile = open("/proc/self/status", O_RDONLY);
//        if (StatusFile == -1) {
//            // Failed - unknown debugger status.
//            return false;
//        }
//
//        char Buffer[256];
//        ssize_t Length = read(StatusFile, Buffer, sizeof(Buffer));
//
//        bool bDebugging = false;
//        const char *TracerString = "TracerPid:\t";
//        const ssize_t LenTracerString = strlen(TracerString);
//        int i = 0;
//
//        while ((Length - i) > LenTracerString) {
//            // TracerPid is found
//            if (strncmp(&Buffer[i], TracerString, LenTracerString) == 0) {
//                // 0 if no process is tracing.
//                bDebugging = Buffer[i + LenTracerString] != '0';
//                break;
//            }
//            ++i;
//        }
//
//        close(StatusFile);
//
//        // remember if we detected debugger so we can skip check next time
//        if (bDebugging) {
//            bDetectedDebugger = true;
//        }
//
//        return bDebugging;
//    }

#endif

#if STATS || ENABLE_STATNAMEDEVENTS

    void FAndroidMisc::BeginNamedEventFrame()
    {
#if FRAMEPRO_ENABLED
        FFrameProProfiler::FrameStart();
#endif // FRAMEPRO_ENABLED
    }
    
    static void WriteTraceMarkerEvent(const ANSICHAR* Text, int32_t TraceMarkerFileDescriptor)
    {
        if (bUseNativeSystrace)
        {
            ATrace_beginSection(Text);
        }
        else
        {
            const int MAX_TRACE_EVENT_LENGTH = 256;
            ANSICHAR EventBuffer[MAX_TRACE_EVENT_LENGTH];
            int EventLength = snprintf(EventBuffer, MAX_TRACE_EVENT_LENGTH, "B|%d|%s", getpid(), Text);
    
            write(TraceMarkerFileDescriptor, EventBuffer, EventLength);
        }
    }
    
    void FAndroidMisc::BeginNamedEvent(const struct FColor& Color, const char* Text)
    {
#if FRAMEPRO_ENABLED
        FFrameProProfiler::PushEvent(Text);
#endif // FRAMEPRO_ENABLED
        if (bUseNativeSystrace ? !ATrace_isEnabled() : TraceMarkerFileDescriptor == -1)
        {
            return;
        }
    
        const int MAX_TRACE_MESSAGE_LENGTH = 256;
    
        // not static since may be called by different threads
        ANSICHAR TextBuffer[MAX_TRACE_MESSAGE_LENGTH];
    
        const char* SourcePtr = Text;
        ANSICHAR* WritePtr = TextBuffer;
        int32_t RemainingSpace = MAX_TRACE_MESSAGE_LENGTH;
        while (*SourcePtr && --RemainingSpace > 0)
        {
            *WritePtr++ = static_cast<ANSICHAR>(*SourcePtr++);
        }
        *WritePtr = '\0';
    
        WriteTraceMarkerEvent(TextBuffer, TraceMarkerFileDescriptor);
    }
    
    void FAndroidMisc::BeginNamedEvent(const struct FColor& Color, const ANSICHAR* Text)
    {
#if FRAMEPRO_ENABLED
        FFrameProProfiler::PushEvent(Text);
#endif // FRAMEPRO_ENABLED
        if (bUseNativeSystrace ? !ATrace_isEnabled() : TraceMarkerFileDescriptor == -1)
        {
            return;
        }
    
        WriteTraceMarkerEvent(Text, TraceMarkerFileDescriptor);
    }
    
    void FAndroidMisc::EndNamedEvent()
    {
#if FRAMEPRO_ENABLED
        FFrameProProfiler::PopEvent();
#endif // FRAMEPRO_ENABLED
        if (bUseNativeSystrace ? !ATrace_isEnabled() : TraceMarkerFileDescriptor == -1)
        {
            return;
        }
    
        if (bUseNativeSystrace)
        {
            ATrace_endSection();
        }
        else
        {
            const ANSICHAR EventTerminatorChar = 'E';
            write(TraceMarkerFileDescriptor, &EventTerminatorChar, 1);
        }
    }
    
    void FAndroidMisc::CustomNamedStat(const char* Text, float Value, const char* Graph, const char* Unit)
    {
        FRAMEPRO_DYNAMIC_CUSTOM_STAT(TCHAR_TO_WCHAR(Text), Value, TCHAR_TO_WCHAR(Graph), TCHAR_TO_WCHAR(Unit), FRAMEPRO_COLOUR(255,255,255) );
    }
    
    void FAndroidMisc::CustomNamedStat(const ANSICHAR* Text, float Value, const ANSICHAR* Graph, const ANSICHAR* Unit)
    {
        FRAMEPRO_DYNAMIC_CUSTOM_STAT(Text, Value, Graph, Unit, FRAMEPRO_COLOUR(255,255,255));
    }

#endif // STATS || ENABLE_STATNAMEDEVENTS

    int FAndroidMisc::GetVolumeState(double *OutTimeOfChangeInSec) {
        int v;
        ReceiversLock.Lock();
        v = CurrentVolume.Volume;
        if (OutTimeOfChangeInSec) {
            *OutTimeOfChangeInSec = CurrentVolume.TimeOfChange;
        }
        ReceiversLock.Unlock();
        return v;
    }

    int32_t FAndroidMisc::GetDeviceVolume() {
        //FAndroidMisc::GetVolumeState returns 0-15, scale to 0-100
        int32_t BaseVolume = FAndroidMisc::GetVolumeState();
        int32_t ScaledVolume = (BaseVolume * 100) / 15;
        return ScaledVolume;
    }

#if USE_ANDROID_FILE
    const char* FAndroidMisc::GamePersistentDownloadDir()
    {
        extern std::string GExternalFilePath;
        return *GExternalFilePath;
    }
    
    std::string FAndroidMisc::GetLoginId()
    {
        static std::string LoginId = std::vector("");
    
        // Return already loaded or generated Id
        if (!LoginId.IsEmpty())
        {
            return LoginId;
        }
    
        // Check for existing identifier file
        extern std::string GInternalFilePath;
        extern std::string GExternalFilePath;
        std::string InternalLoginIdFilename = GInternalFilePath / std::vector("login-identifier.txt");
        if (FPaths::FileExists(InternalLoginIdFilename))
        {
            if (FFileHelper::LoadFileToString(LoginId, *InternalLoginIdFilename))
            {
                return LoginId;
            }
        }
        std::string LoginIdFilename = GExternalFilePath / std::vector("login-identifier.txt");
        if (FPaths::FileExists(LoginIdFilename))
        {
            if (FFileHelper::LoadFileToString(LoginId, *LoginIdFilename))
            {
                FFileHelper::SaveStringToFile(LoginId, *InternalLoginIdFilename);
                return LoginId;
            }
        }
    
        // Generate a new one and write to file
        FGuid DeviceGuid;
        FPlatformMisc::CreateGuid(DeviceGuid);
        LoginId = DeviceGuid.ToString();
        FFileHelper::SaveStringToFile(LoginId, *InternalLoginIdFilename);
    
        return LoginId;
    }
#endif

#if USE_ANDROID_JNI
    std::string FAndroidMisc::GetDeviceId()
    {
#if GET_DEVICE_ID_UNAVAILABLE
        return std::string();
#else
        static std::string DeviceId = AndroidThunkCpp_GetAndroidId();
    
        // note: this can be empty or NOT unique depending on the OEM implementation!
        return DeviceId;
#endif
    }
    
    std::string FAndroidMisc::GetUniqueAdvertisingId()
    {
        static std::string AdvertisingId = AndroidThunkCpp_GetAdvertisingId();

        // note: this can be empty if Google Play not installed, or user is blocking it!
        return AdvertisingId;
    }
#endif

    FAndroidMisc::FBatteryState FAndroidMisc::GetBatteryState() {
        FBatteryState CurState;
        ReceiversLock.Lock();
        CurState = CurrentBatteryState;
        ReceiversLock.Unlock();
        return CurState;
    }

    int FAndroidMisc::GetBatteryLevel() {
        FBatteryState BatteryState = GetBatteryState();
        return BatteryState.Level;
    }

    bool FAndroidMisc::IsRunningOnBattery() {
        FBatteryState BatteryState = GetBatteryState();
        return BatteryState.State == BATTERY_STATE_DISCHARGING;
    }

//    bool FAndroidMisc::IsInLowPowerMode() {
//        FBatteryState BatteryState = GetBatteryState();
//        return BatteryState.Level <= GAndroidLowPowerBatteryThreshold;
//    }

    float FAndroidMisc::GetDeviceTemperatureLevel() {
        return GetBatteryState().Temperature;
    }

    bool FAndroidMisc::AreHeadPhonesPluggedIn() {
        return HeadPhonesArePluggedIn;
    }

#define ANDROIDTHUNK_CONNECTION_TYPE_NONE 0
#define ANDROIDTHUNK_CONNECTION_TYPE_AIRPLANEMODE 1
#define ANDROIDTHUNK_CONNECTION_TYPE_ETHERNET 2
#define ANDROIDTHUNK_CONNECTION_TYPE_CELL 3
#define ANDROIDTHUNK_CONNECTION_TYPE_WIFI 4
#define ANDROIDTHUNK_CONNECTION_TYPE_WIMAX 5
#define ANDROIDTHUNK_CONNECTION_TYPE_BLUETOOTH 6

//    ENetworkConnectionType FAndroidMisc::GetNetworkConnectionType() {
//#if USE_ANDROID_JNI
//        extern int32_t AndroidThunkCpp_GetNetworkConnectionType();
//
//        switch (AndroidThunkCpp_GetNetworkConnectionType())
//        {
//            case ANDROIDTHUNK_CONNECTION_TYPE_NONE:				return ENetworkConnectionType::None;
//            case ANDROIDTHUNK_CONNECTION_TYPE_AIRPLANEMODE:		return ENetworkConnectionType::AirplaneMode;
//            case ANDROIDTHUNK_CONNECTION_TYPE_ETHERNET:			return ENetworkConnectionType::Ethernet;
//            case ANDROIDTHUNK_CONNECTION_TYPE_CELL:				return ENetworkConnectionType::Cell;
//            case ANDROIDTHUNK_CONNECTION_TYPE_WIFI:				return ENetworkConnectionType::WiFi;
//            case ANDROIDTHUNK_CONNECTION_TYPE_WIMAX:			return ENetworkConnectionType::WiMAX;
//            case ANDROIDTHUNK_CONNECTION_TYPE_BLUETOOTH:		return ENetworkConnectionType::Bluetooth;
//        }
//#endif
//        return ENetworkConnectionType::Unknown;
//    }
//
//#if USE_ANDROID_JNI
//    bool FAndroidMisc::HasActiveWiFiConnection()
//    {
//        ENetworkConnectionType ConnectionType = GetNetworkConnectionType();
//        return (ConnectionType == ENetworkConnectionType::WiFi ||
//                ConnectionType == ENetworkConnectionType::WiMAX);
//    }
//#endif

    JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeNetworkChanged(JNIEnv *jenv, jobject thiz) {
//        if (FTaskGraphInterface::IsRunning()) {
//            FFunctionGraphTask::CreateAndDispatchWhenReady([]() {
//                FCoreDelegates::OnNetworkConnectionChanged.Broadcast(FAndroidMisc::GetNetworkConnectionType());
//            }, TStatId(), NULL, ENamedThreads::GameThread);
//        }
    }

    static FAndroidMisc::ReInitWindowCallbackType OnReInitWindowCallback;

    FAndroidMisc::ReInitWindowCallbackType FAndroidMisc::GetOnReInitWindowCallback() {
        return OnReInitWindowCallback;
    }

    void FAndroidMisc::SetOnReInitWindowCallback(FAndroidMisc::ReInitWindowCallbackType InOnReInitWindowCallback) {
        OnReInitWindowCallback = InOnReInitWindowCallback;
    }

    static FAndroidMisc::ReleaseWindowCallbackType OnReleaseWindowCallback;

    FAndroidMisc::ReleaseWindowCallbackType FAndroidMisc::GetOnReleaseWindowCallback() {
        return OnReleaseWindowCallback;
    }

    void FAndroidMisc::SetOnReleaseWindowCallback(FAndroidMisc::ReleaseWindowCallbackType InOnReleaseWindowCallback) {
        OnReleaseWindowCallback = InOnReleaseWindowCallback;
    }

    static FAndroidMisc::OnPauseCallBackType OnPauseCallback;

    FAndroidMisc::OnPauseCallBackType FAndroidMisc::GetOnPauseCallback() {
        return OnPauseCallback;
    }

    void FAndroidMisc::SetOnPauseCallback(FAndroidMisc::OnPauseCallBackType InOnPauseCallback) {
        OnPauseCallback = InOnPauseCallback;
    }

    std::string FAndroidMisc::GetCPUVendor() {
        return DeviceMake;
    }

    std::string FAndroidMisc::GetCPUBrand() {
        return DeviceModel;
    }

    std::string FAndroidMisc::GetCPUChipset() {
        static std::string *Chipset = nullptr;//FAndroidMisc::GetConfigRulesVariable("hardware");
        return (Chipset == NULL) ? "": *Chipset;
        //return (Chipset == NULL) ? FGenericPlatformMisc::GetCPUChipset() : *Chipset;

    }

    std::string FAndroidMisc::GetPrimaryGPUBrand() {
        return FAndroidMisc::GetGPUFamily();
    }

    void FAndroidMisc::GetOSVersions(std::string &out_OSVersionLabel, std::string &out_OSSubVersionLabel) {
        out_OSVersionLabel = ("Android");
        out_OSSubVersionLabel = AndroidVersion;
    }

    std::string FAndroidMisc::GetOSVersion() {
        return AndroidVersion;
    }

    bool FAndroidMisc::GetDiskTotalAndFreeSpace(const std::string &InPath, uint64_t &TotalNumberOfBytes,
                                                uint64_t &NumberOfFreeBytes) {
#if USE_ANDROID_FILE
        extern std::string GExternalFilePath;
        struct statfs FSStat = { 0 };
        FTCHARToUTF8 Converter(*GExternalFilePath);
        int Err = statfs((ANSICHAR*)Converter.Get(), &FSStat);
    
        if (Err == 0)
        {
            TotalNumberOfBytes = FSStat.f_blocks * FSStat.f_bsize;
            NumberOfFreeBytes = FSStat.f_bavail * FSStat.f_bsize;
        }
        else
        {
            int ErrNo = errno;
            //UE_LOG(LogAndroid, Warning, std::vector("Unable to statfs('%s'): errno=%d (%s)"), *GExternalFilePath, ErrNo, UTF8_TO_TCHAR(strerror(ErrNo)));
        }
    
        return (Err == 0);
#else
        return false;
#endif
    }

    uint32_t FAndroidMisc::GetCoreFrequency(int32_t CoreIndex, ECoreFrequencyProperty CoreFrequencyProperty) {
        uint32_t ReturnFrequency = 0;
        char QueryFile[256];
        const char *FreqProperty = nullptr;
        static const char *CurrentFrequencyString = "scaling_cur_freq";
        static const char *MaxFrequencyString = "cpuinfo_max_freq";
        static const char *MinFrequencyString = "cpuinfo_min_freq";
        switch (CoreFrequencyProperty) {
            case ECoreFrequencyProperty::MaxFrequency:
                FreqProperty = MaxFrequencyString;
                break;
            case ECoreFrequencyProperty::MinFrequency:
                FreqProperty = MinFrequencyString;
                break;
            default:
            case ECoreFrequencyProperty::CurrentFrequency:
                FreqProperty = CurrentFrequencyString;
                break;
        }
        sprintf(QueryFile, "/sys/devices/system/cpu/cpu%d/cpufreq/%s", CoreIndex, FreqProperty);

        if (FILE * CoreFreqStateFile = fopen(QueryFile, "r")) {
            char CurrCoreFreq[32] = {0};
            if (fgets(CurrCoreFreq, (32), CoreFreqStateFile) != nullptr) {
                ReturnFrequency = atol(CurrCoreFreq);
            }
            fclose(CoreFreqStateFile);
        }
        return ReturnFrequency;
    }

    float FAndroidMisc::GetCPUTemperature() {
        float Temp = 0.0f;
        if (*AndroidCpuThermalSensorFileBuf == 0) {
            return Temp;
        }

        if (FILE * Thermals = fopen(AndroidCpuThermalSensorFileBuf, "r")) {
            char Buf[256];
            if (fgets(Buf, 256, Thermals)) {
                // sensor temp file can contain whitespace symbols at the end of the line, count length only for digit symbols
                char *p = Buf;
                uint32_t Len = 0;
                while (isdigit(*p)) {
                    ++Len;
                    ++p;
                }

                // Temperature is reported by different sensors in different ways, some report it as XXX, some - as XXXXX. Reduce it to standard XX.X
                const uint32_t StandardLen = 2;
                const float Divider = pow(10.0f, (float) (Len - StandardLen));
                Temp = (float) atol(Buf) / Divider;
            }
            fclose(Thermals);
        }

        return Temp;
    }

    bool FAndroidMisc::Expand16BitIndicesTo32BitOnLoad() {
       // return (CVarMaliMidgardIndexingBug.GetValueOnAnyThread() > 0);
       return false;
    }

    int FAndroidMisc::GetMobilePropagateAlphaSetting() {
        return GAndroidPropagateAlpha;
    }

    std::vector <int32_t> FAndroidMisc::GetSupportedNativeDisplayRefreshRates() {
        std::vector <int32_t> Result;
#if USE_ANDROID_JNI
        Result = AndroidThunkCpp_GetSupportedNativeDisplayRefreshRates();
#else
        Result.Add(60);
#endif
        return Result;
    }

    bool FAndroidMisc::SetNativeDisplayRefreshRate(int32_t RefreshRate) {
#if USE_ANDROID_JNI
        return AndroidThunkCpp_SetNativeDisplayRefreshRate(RefreshRate);
#else
        return RefreshRate == 60;
#endif
    }

    int32_t FAndroidMisc::GetNativeDisplayRefreshRate() {
#if USE_ANDROID_JNI
        return AndroidThunkCpp_GetNativeDisplayRefreshRate();
#else
        return 60;
#endif

    }

    FORCEINLINE bool ValueOutsideThreshold(float Value, float BaseLine, float Threshold) {
        return Value > BaseLine * (1.0f + Threshold)
               || Value < BaseLine * (1.0f - Threshold);
    }

   // void (*GMemoryWarningHandler)(const FGenericMemoryWarningContext &Context) = NULL;

//    void FAndroidMisc::SetMemoryWarningHandler(void (*InHandler)(const FGenericMemoryWarningContext &Context)) {
//        GE_ASSERT(IsInGameThread());
//        GMemoryWarningHandler = InHandler;
//    }

    bool FAndroidMisc::HasMemoryWarningHandler() {
//        GE_ASSERT(IsInGameThread());
//        return GMemoryWarningHandler != nullptr;
        return false;
    }

    bool FAndroidMisc::SupportsBackbufferSampling() {
        static int32_t CachedAndroidOpenGLSupportsBackbufferSampling = -1;

        if (CachedAndroidOpenGLSupportsBackbufferSampling == -1) {
            bool bAndroidOpenGLSupportsBackbufferSampling = false;
//            GConfig->GetBool(std::vector("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"),
//                             std::vector("bAndroidOpenGLSupportsBackbufferSampling"), bAndroidOpenGLSupportsBackbufferSampling,
//                             GEngineIni);

            CachedAndroidOpenGLSupportsBackbufferSampling = (bAndroidOpenGLSupportsBackbufferSampling ||
                                                             FAndroidMisc::ShouldUseVulkan()) ? 1 : 0;
        }

        return CachedAndroidOpenGLSupportsBackbufferSampling == 1;
    }
    bool FAndroidMisc::ShouldUseVulkan() {
        return false;
    }
    void FAndroidMisc::NonReentrantRequestExit() {
//#if UE_SET_REQUEST_EXIT_ON_TICK_ONLY
//        // Cheating here to grab access to this. This function should only be used in extreme cases in which non-reentrant functions are needed (ie. crash handling/signal handler)
//        extern bool GShouldRequestExit;
//        GShouldRequestExit = true;
//#else
//        PRAGMA_DISABLE_DEPRECATION_WARNINGS
//                GIsRequestingExit = true;
//        PRAGMA_ENABLE_DEPRECATION_WARNINGS
//#endif // UE_SET_REQUEST_EXIT_ON_TICK_ONLY
    }

    void FAndroidMisc::RegisterThreadName(const char *Name, uint32_t ThreadId) {
       // FScopeLock Lock(&AndroidThreadNamesLock);
        if (AndroidThreadNames.find(ThreadId) == AndroidThreadNames.end() ) {
            AndroidThreadNames.insert({ThreadId, Name});
        }
    }

    const char *FAndroidMisc::GetThreadName(uint32_t ThreadId) {
        FScopeLock Lock(&AndroidThreadNamesLock);
         if(AndroidThreadNames.find(ThreadId)!=AndroidThreadNames.end()) {
             const char *ThreadName =AndroidThreadNames[ThreadId];
             return ThreadName;
        }
         return nullptr;

    }

//    void FAndroidMisc::SetDeviceOrientation(EDeviceScreenOrientation NewDeviceOrentation) {
//        SetAllowedDeviceOrientation(NewDeviceOrentation);
//    }

    void FAndroidMisc::SetCellularPreference(int32_t Value) {
#if USE_ANDROID_JNI
        AndroidThunkCpp_SetCellularPreference(Value);
#endif // USE_ANDROID_JNI
    }

    int32_t FAndroidMisc::GetCellularPreference() {
        int32_t value = 0;
#if USE_ANDROID_JNI
        value = AndroidThunkCpp_GetCellularPreference();
#endif // USE_ANDROID_JNI
        return value;
    }

//    void FAndroidMisc::SetAllowedDeviceOrientation(EDeviceScreenOrientation NewAllowedDeviceOrientation) {
//        AllowedDeviceOrientation = NewAllowedDeviceOrientation;
//
//#if USE_ANDROID_JNI
//        AndroidThunkCpp_SetOrientation(GetAndroidScreenOrientation(NewAllowedDeviceOrientation));
//#endif // USE_ANDROID_JNI
//    }
//
//#if USE_ANDROID_JNI
//    int32_t FAndroidMisc::GetAndroidScreenOrientation(EDeviceScreenOrientation ScreenOrientation)
//    {
//        EAndroidScreenOrientation AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_UNSPECIFIED;
//        switch (ScreenOrientation)
//        {
//        case EDeviceScreenOrientation::Unknown:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_UNSPECIFIED;
//            break;
//        case EDeviceScreenOrientation::Portrait:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_PORTRAIT;
//            break;
//        case EDeviceScreenOrientation::PortraitUpsideDown:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_REVERSE_PORTRAIT;
//            break;
//        case EDeviceScreenOrientation::LandscapeLeft:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_LANDSCAPE;
//            break;
//        case EDeviceScreenOrientation::LandscapeRight:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
//            break;
//        case EDeviceScreenOrientation::FaceUp:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_UNSPECIFIED;
//            break;
//        case EDeviceScreenOrientation::FaceDown:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_UNSPECIFIED;
//            break;
//        case EDeviceScreenOrientation::PortraitSensor:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_SENSOR_PORTRAIT;
//            break;
//        case EDeviceScreenOrientation::LandscapeSensor:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_SENSOR_LANDSCAPE;
//            break;
//        case EDeviceScreenOrientation::FullSensor:
//            AndroidScreenOrientation = EAndroidScreenOrientation::SCREEN_ORIENTATION_SENSOR;
//            break;
//        }
//
//        return static_cast<int32_t>(AndroidScreenOrientation);
//    }
//#endif // USE_ANDROID_JNI


    void FAndroidMisc::ShowConsoleWindow() {
#if !UE_BUILD_SHIPPING && USE_ANDROID_JNI
        AndroidThunkCpp_ShowConsoleWindow();
#endif // !UE_BUILD_SHIPPING && USE_ANDROID_JNI
    }

//    FDelegateHandle
//    FAndroidMisc::AddNetworkListener(FCoreDelegates::FOnNetworkConnectionChanged::FDelegate &&InNewDelegate) {
//        if (!FCoreDelegates::OnNetworkConnectionChanged.IsBound()) {
//#if USE_ANDROID_JNI
//            extern void AndroidThunkJava_AddNetworkListener();
//            AndroidThunkJava_AddNetworkListener();
//#endif
//        }
//
//        return FCoreDelegates::OnNetworkConnectionChanged.Add(MoveTemp(InNewDelegate));
//    }
//
//    bool FAndroidMisc::RemoveNetworkListener(FDelegateHandle Handle) {
//        bool bSuccess = FCoreDelegates::OnNetworkConnectionChanged.Remove(Handle);
//
//        if (!FCoreDelegates::OnNetworkConnectionChanged.IsBound()) {
//#if USE_ANDROID_JNI
//            extern void AndroidThunkJava_RemoveNetworkListener();
//            AndroidThunkJava_RemoveNetworkListener();
//#endif
//        }
//
//        return bSuccess;
//    }

}
}
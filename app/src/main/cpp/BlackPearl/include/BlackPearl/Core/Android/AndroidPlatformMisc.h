//
// Created by DXT00 on 2025/4/11.
//

#pragma once
#include "stdint.h"
#include "../Platform.h"
#include "../Generic/GenericPlatformMisc.h"
namespace BlackPearl {

//The android configuration orientations defined in C++ map to the values in https://developer.android.com/reference/android/content/res/Configuration#ORIENTATION_PORTRAIT
    namespace EAndroidConfigurationOrientation
    {
        enum Type
        {
            ORIENTATION_PORTRAIT = 0x00000001,
            ORIENTATION_LANDSCAPE = 0x00000002,
        };
    };

//The android surface rotations defined in C++ map to the values in https://developer.android.com/reference/android/view/Surface#ROTATION_0
    namespace EAndroidSurfaceRotation
    {
        enum Type
        {
            ROTATION_0,
            ROTATION_90,
            ROTATION_180,
            ROTATION_270,
        };
    };


/**
 * Android implementation of the misc OS functions
 */
    struct FAndroidMisc : public GenericPlatformMisc//: public FGenericPlatformMisc
    {
        static void RequestExit(bool Force, const char *CallSite = nullptr);

        static bool RestartApplication();

        static void LocalPrint(const char *Message);

        static bool IsLocalPrintThreadSafe() { return true; }

        static void PlatformPreInit();

        static void PlatformInit();

        static void PlatformTearDown();

        //static void PlatformHandleSplashScreen(bool ShowSplashScreen);

        // static EDeviceScreenOrientation GetDeviceOrientation() { return DeviceOrientation; }
        //  UE_DEPRECATED(5.1, "SetDeviceOrientation is deprecated. Use SetAllowedDeviceOrientation instead.")
       // static void SetDeviceOrientation(EDeviceScreenOrientation NewDeviceOrentation);
        //  static  void SetAllowedDeviceOrientation(EDeviceScreenOrientation NewAllowedDeviceOrientation);

        // Change this to an Enum with Always allow, allow and deny
        static void SetCellularPreference(int32_t Value);

        static int32_t GetCellularPreference();

        FORCEINLINE static int32_t GetMaxPathLength() {
            return ANDROID_MAX_PATH;
        }

        // UE_DEPRECATED(4.21, "void FPlatformMisc::GetEnvironmentVariable(Name, Result, Length) is deprecated. Use FString FPlatformMisc::GetEnvironmentVariable(Name) instead.")
        static void GetEnvironmentVariable(const char *VariableName, char *Result, int32_t ResultLength);

        static std::string GetEnvironmentVariable(const char *VariableName);

        static const char *GetSystemErrorMessage(char *OutBuffer, int32_t BufferCount, int32_t Error);

        //  static  EAppReturnType::Type MessageBoxExt( EAppMsgType::Type MsgType, const char* Text, const char* Caption );
       // static bool UseRenderThread();

        //static bool HasPlatformFeature(const char *FeatureName);

        static bool ShouldDisablePluginAtRuntime(const std::string &PluginName);

        static bool SupportsES30();

    public:

        static bool AllowThreadHeartBeat();

        struct FCPUStatTime {
            uint64_t TotalTime;
            uint64_t UserTime;
            uint64_t NiceTime;
            uint64_t SystemTime;
            uint64_t SoftIRQTime;
            uint64_t IRQTime;
            uint64_t IdleTime;
            uint64_t IOWaitTime;
        };

        struct FCPUState {
            const static int32_t MaxSupportedCores = 16; //Core count 16 is maximum for now
            int32_t CoreCount;
            int32_t ActivatedCoreCount;
            char Name[6];
            FAndroidMisc::FCPUStatTime CurrentUsage[MaxSupportedCores];
            FAndroidMisc::FCPUStatTime PreviousUsage[MaxSupportedCores];
            int32_t Status[MaxSupportedCores];
            double Utilization[MaxSupportedCores];
            double AverageUtilization;

        };

        static FCPUState &GetCPUState();

       // static int32_t NumberOfCores();

      //  static int32_t NumberOfCoresIncludingHyperthreads();

        static bool SupportsLocalCaching();

       // static void CreateGuid(struct FGuid &Result);

        //static  void SetCrashHandler(void (* CrashHandler)(const FGenericCrashContext& Context));
        // NOTE: THIS FUNCTION IS DEFINED IN ANDROIDOPENGL.CPP
     //   static void GetValidTargetPlatforms(class std::vector<std::string> &TargetPlatformNames);

        static bool GetUseVirtualJoysticks();

        static bool SupportsTouchInput();

        static const char *GetDefaultDeviceProfileName() { return ("Android_Default"); }

        static bool GetVolumeButtonsHandledBySystem();

        static void SetVolumeButtonsHandledBySystem(bool enabled);

        // Returns current volume, 0-15
        static int GetVolumeState(double *OutTimeOfChangeInSec = nullptr);

        static int32_t GetDeviceVolume();

#if USE_ANDROID_FILE
        static  const char* GamePersistentDownloadDir();
        static  std::string GetLoginId();
#endif
#if USE_ANDROID_JNI
        static  std::string GetDeviceId();
        static  std::string GetUniqueAdvertisingId();
#endif

        static std::string GetCPUVendor();

        static std::string GetCPUBrand();

        static std::string GetCPUChipset();

        static std::string GetPrimaryGPUBrand();

        static void GetOSVersions(std::string &out_OSVersionLabel, std::string &out_OSSubVersionLabel);

        static bool
        GetDiskTotalAndFreeSpace(const std::string &InPath, uint64_t &TotalNumberOfBytes, uint64_t &NumberOfFreeBytes);

        enum EBatteryState {
            BATTERY_STATE_UNKNOWN = 1,
            BATTERY_STATE_CHARGING,
            BATTERY_STATE_DISCHARGING,
            BATTERY_STATE_NOT_CHARGING,
            BATTERY_STATE_FULL
        };
        struct FBatteryState {
            FAndroidMisc::EBatteryState State;
            int Level;          // in range [0,100]
            float Temperature;    // in degrees of Celsius
        };

        static FBatteryState GetBatteryState();

        static int GetBatteryLevel();

        static bool IsRunningOnBattery();

      //  static bool IsInLowPowerMode();

        static float GetDeviceTemperatureLevel();

        static bool AreHeadPhonesPluggedIn();

       // static ENetworkConnectionType GetNetworkConnectionType();

//#if USE_ANDROID_JNI
//        static  bool HasActiveWiFiConnection();
//#endif

        static void RegisterForRemoteNotifications();

        static void UnregisterForRemoteNotifications();

        static bool IsAllowedRemoteNotifications();

        /** @return Memory representing a true type or open type font provided by the platform as a default font for unreal to consume; empty array if the default font failed to load. */
       // static std::vector<uint8_t> GetSystemFontBytes();

        //  static  IPlatformChunkInstall* GetPlatformChunkInstall();

        // static  void PrepareMobileHaptics(EMobileHapticsType Type);
        static void TriggerMobileHaptics();

        static void ReleaseMobileHaptics();
        //   static  void ShareURL(const std::string& URL, const FText& Description, int32_t LocationHintX, int32_t LocationHintY);

        //static std::string LoadTextFileFromPlatformPackage(const std::string &RelativePath);

       // static bool FileExistsInPlatformPackage(const std::string &RelativePath);

        // ANDROID ONLY:
//        static void SetVersionInfo(std::string AndroidVersion, int32_t InTargetSDKVersion, std::string DeviceMake,
//                                   std::string DeviceModel, std::string DeviceBuildNumber, std::string OSLanguage);

        static const std::string GetAndroidVersion();

        static int32_t GetAndroidMajorVersion();

        static int32_t GetTargetSDKVersion();

        static const std::string GetDeviceMake();

        static const std::string GetDeviceModel();

        static const std::string GetOSLanguage();

        static const std::string GetDeviceBuildNumber();

       // static const std::string GetProjectVersion();

        static std::string GetDefaultLocale();

        static std::string GetGPUFamily();

        static std::string GetGLVersion();

        static bool SupportsFloatingPointRenderTargets();

        static bool SupportsShaderFramebufferFetch();

        static bool SupportsShaderIOBlocks();

#if USE_ANDROID_JNI
        static  int GetAndroidBuildVersion();
#endif

        static bool IsSupportedAndroidDevice();

        static void SetForceUnsupported(bool bInOverride);

        static const std::map<std::string, std::string> &GetConfigRulesTMap();

      //  static std::string *GetConfigRulesVariable(const std::string &Key);

        /* HasVulkanDriverSupport
         * @return true if this Android device supports a Vulkan API Unreal could use
         */
        static bool HasVulkanDriverSupport();

        /* IsVulkanAvailable
         * @return	true if there is driver support, we have an RHI, we are packaged with Vulkan support,
         *			and not we are not forcing GLES with a command line switch
         */
        static bool IsVulkanAvailable();

        /* ShouldUseVulkan
         * @return true if Vulkan is available, and not disabled by device profile cvar
         */
        static bool ShouldUseVulkan();

        static bool ShouldUseDesktopVulkan();

        static std::string GetVulkanVersion();

        typedef std::function<void(void *NewNativeHandle)> ReInitWindowCallbackType;

        static ReInitWindowCallbackType GetOnReInitWindowCallback();

        static void SetOnReInitWindowCallback(ReInitWindowCallbackType InOnReInitWindowCallback);

        typedef std::function<void()> ReleaseWindowCallbackType;

        static ReleaseWindowCallbackType GetOnReleaseWindowCallback();

        static void SetOnReleaseWindowCallback(ReleaseWindowCallbackType InOnReleaseWindowCallback);

        static std::string GetOSVersion();

        static bool GetOverrideResolution(int32_t &ResX, int32_t &ResY) { return false; }

        typedef std::function<void()> OnPauseCallBackType;

        static OnPauseCallBackType GetOnPauseCallback();

        static void SetOnPauseCallback(OnPauseCallBackType InOnPauseCallback);
        // static  void TriggerCrashHandler(ECrashContextType InType, const char* InErrorMessage, const char* OverrideCallstack = nullptr);

        // To help track down issues with failing crash handler.
        static std::string GetFatalSignalMessage(int Signal, siginfo *Info);

        static void OverrideFatalSignalHandler(
                void (*FatalSignalHandlerOverrideFunc)(int Signal, struct siginfo *Info, void *Context,
                                                       uint32_t CrashingThreadId));
        // To help track down issues with failing crash handler.

        static bool IsInSignalHandler();


        FORCEINLINE static void MemoryBarrier() {
            __sync_synchronize();
        }

        

        // run time compatibility information
        static std::string AndroidVersion; // version of android we are running eg "4.0.4"
        static int32_t AndroidMajorVersion; // integer major version of Android we are running, eg 10
        static int32_t TargetSDKVersion; // Target SDK version, eg 29.
        static std::string DeviceMake; // make of the device we are running on eg. "samsung"
        static std::string DeviceModel; // model of the device we are running on eg "SAMSUNG-SGH-I437"
        static std::string DeviceBuildNumber; // platform image build number of device "R16NW.G960NKSU1ARD6"
        static std::string OSLanguage; // language code the device is set to

        // Build version of Android, i.e. API level.
        static int32_t AndroidBuildVersion;

        // Key/Value pair variables from the optional configuration.txt
        static std::map<std::string, std::string> ConfigRulesVariables;

        static bool VolumeButtonsHandledBySystem;

        static bool bNeedsRestartAfterPSOPrecompile;

        enum class ECoreFrequencyProperty {
            CurrentFrequency,
            MaxFrequency,
            MinFrequency,
        };

        static uint32_t GetCoreFrequency(int32_t CoreIndex, ECoreFrequencyProperty CoreFrequencyProperty);

        // Returns CPU temperature read from one of the configurable CPU sensors via android.CPUThermalSensorFilePath CVar or AndroidEngine.ini, [ThermalSensors] section.
        // Doesn't guarantee to work on all devices. Some devices require root access rights to read sensors information, in that case 0.0 will be returned
        static float GetCPUTemperature();

        static void UpdateDeviceOrientation();

//        static void
//        SaveDeviceOrientation(EDeviceScreenOrientation NewDeviceOrentation) { DeviceOrientation = NewDeviceOrentation; }

        // Window access is locked by the game thread before preinit and unlocked here after RHIInit (PlatformCreateDynamicRHI).
        static void UnlockAndroidWindow();

        static std::vector<int32_t> GetSupportedNativeDisplayRefreshRates();

        static bool SetNativeDisplayRefreshRate(int32_t RefreshRate);

        static int32_t GetNativeDisplayRefreshRate();

        /**
         * Returns whether or not a 16 bit index buffer should be promoted to 32 bit on load, needed for some Android devices
         */
        static bool Expand16BitIndicesTo32BitOnLoad();

        /**
         * Will return true if we wish to propagate the alpha to the backbuffer
         */
        static int GetMobilePropagateAlphaSetting();

        static bool SupportsBackbufferSampling();

     //   static void SetMemoryWarningHandler(void (*Handler)(const FGenericMemoryWarningContext &Context));

        static bool HasMemoryWarningHandler();

        // Android specific requesting of exit, *ONLY* use this function in signal handling code. Otherwise normal RequestExit functions
        static void NonReentrantRequestExit();

        // Register/Get thread names for Android specific threads
        static void RegisterThreadName(const char *Name, uint32_t ThreadId);

        static const char *GetThreadName(uint32_t ThreadId);

        static void ShowConsoleWindow();

//        static FDelegateHandle AddNetworkListener(FOnNetworkConnectionChangedDelegate &&InNewDelegate);
//
//        static bool RemoveNetworkListener(FDelegateHandle Handle);

    private:
        static const char *CodeToString(int Signal, int si_code);

        //static EDeviceScreenOrientation DeviceOrientation;

#if USE_ANDROID_JNI
        enum class EAndroidScreenOrientation
        {
            SCREEN_ORIENTATION_UNSPECIFIED = -1,
            SCREEN_ORIENTATION_LANDSCAPE = 0,
            SCREEN_ORIENTATION_PORTRAIT = 1,
            SCREEN_ORIENTATION_USER = 2,
            SCREEN_ORIENTATION_BEHIND = 3,
            SCREEN_ORIENTATION_SENSOR = 4,
            SCREEN_ORIENTATION_NOSENSOR = 5,
            SCREEN_ORIENTATION_SENSOR_LANDSCAPE = 6,
            SCREEN_ORIENTATION_SENSOR_PORTRAIT = 7,
            SCREEN_ORIENTATION_REVERSE_LANDSCAPE = 8,
            SCREEN_ORIENTATION_REVERSE_PORTRAIT = 9,
            SCREEN_ORIENTATION_FULL_SENSOR = 10,
            SCREEN_ORIENTATION_USER_LANDSCAPE = 11,
            SCREEN_ORIENTATION_USER_PORTRAIT = 12,
        };

        //static  int32_t GetAndroidScreenOrientation(EDeviceScreenOrientation ScreenOrientation);
#endif // USE_ANDROID_JNI
    };

    typedef FAndroidMisc FPlatformMisc;
}
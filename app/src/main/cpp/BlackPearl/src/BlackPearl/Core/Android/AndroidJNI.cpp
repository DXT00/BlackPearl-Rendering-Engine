//
// Created by DXT00 on 2025/4/14.
//
#include "pch.h"

#include "Core/Android/AndroidJNI.h"
#include "ApplicationCore/Android/AndroidApplication.h"
#include "Core/Android/AndroidJavaEnv.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/ApplicationCore/Android/AndroidApplication.h"
// Copyright Epic Games, Inc. All Rights Reserved.

#include <string>
#if USE_ANDROID_JNI


#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace BlackPearl{
    

#define JNI_CURRENT_VERSION JNI_VERSION_1_6

JavaVM* GJavaVM = nullptr;
jclass GGameActivityClassID;
jobject GGameActivityThis;

// Pointer to target widget for virtual keyboard contents
//static TWeakPtr<IVirtualKeyboardEntry> VirtualKeyboardWidget;

//virtualKeyboard shown
static volatile bool GVirtualKeyboardShown = false;
//WebView shown
static volatile bool GWebViewShown = false;

// External File Path base - setup during load
    std::string GFilePathBase;
// Obb File Path base - setup during load
    std::string GOBBFilePathBase;
// Obb Main filepath
    std::string GOBBMainFilePath;
// Obb Patch filepath
    std::string GOBBPatchFilePath;
    std::string GOBBOverflow1FilePath;

std::string GInternalFilePath;
 std::string GExternalFilePath;
 std::string GFontPathBase;
 bool GOBBinAPK;
 bool GOverrideAndroidLogDir;
 std::string GOBBOverflow2FilePath;
 std::string GAPKFilename;

//FOnActivityResult FJavaWrapper::OnActivityResultDelegate;
//FOnSafetyNetAttestationResult FJavaWrapper::OnSafetyNetAttestationResultDelegate;
//FOnRouteServiceIntent FJavaWrapper::OnRouteServiceIntentDelegate;

//////////////////////////////////////////////////////////////////////////

#if UE_BUILD_SHIPPING
// always clear any exceptions in Shipping
#define CHECK_JNI_RESULT(Id) if (Id == 0) { Env->ExceptionClear(); }
#else
#define CHECK_JNI_RESULT(Id) \
if (Id == 0) \
{ \
	if (bIsOptional) { Env->ExceptionClear(); } \
	else { Env->ExceptionDescribe(); GE_ASSERT(Id != 0, ("Failed to find " #Id)); } \
}
#endif

#define CHECK_JNI_METHOD(Id) GE_ASSERT(Id != nullptr, TEXT("Failed to find " #Id));

void FJavaWrapper::FindClassesAndMethods(JNIEnv* Env)
{
    auto bIsOptional = false;
    GGameActivityClassID = GameActivityClassID = FindClassGlobalRef(Env, "com/epicgames/unreal/GameActivity", bIsOptional);
    AndroidThunkJava_ShowConsoleWindow = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShowConsoleWindow", "(Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_ShowVirtualKeyboardInputDialog = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShowVirtualKeyboardInputDialog", "(ILjava/lang/String;Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_HideVirtualKeyboardInputDialog = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_HideVirtualKeyboardInputDialog", "()V", bIsOptional);
    AndroidThunkJava_ShowVirtualKeyboardInput = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShowVirtualKeyboardInput", "(ILjava/lang/String;Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_HideVirtualKeyboardInput = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_HideVirtualKeyboardInput", "()V", bIsOptional);
    AndroidThunkJava_LaunchURL = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LaunchURL", "(Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_GetAssetManager = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetAssetManager", "()Landroid/content/res/AssetManager;", bIsOptional);
    AndroidThunkJava_Minimize = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_Minimize", "()V", bIsOptional);
    AndroidThunkJava_ClipboardCopy = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ClipboardCopy", "(Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_ClipboardPaste = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ClipboardPaste", "()Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_ForceQuit = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ForceQuit", "()V", bIsOptional);
    AndroidThunkJava_GetFontDirectory = FindStaticMethod(Env, GameActivityClassID, "AndroidThunkJava_GetFontDirectory", "()Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_Vibrate = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_Vibrate", "(II)V", bIsOptional);
    AndroidThunkJava_IsMusicActive = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsMusicActive", "()Z", bIsOptional);
    AndroidThunkJava_IsScreensaverEnabled = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsScreensaverEnabled", "()Z", bIsOptional);
    AndroidThunkJava_KeepScreenOn = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_KeepScreenOn", "(Z)V", bIsOptional);
    AndroidThunkJava_InitHMDs = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_InitHMDs", "()V", bIsOptional);
    AndroidThunkJava_DismissSplashScreen = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_DismissSplashScreen", "()V", bIsOptional);
    AndroidThunkJava_ShowProgressDialog = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShowProgressDialog", "(ZLjava/lang/String;ZI)V", bIsOptional);
    AndroidThunkJava_UpdateProgressDialog = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_UpdateProgressDialog", "(I)V", bIsOptional);
    AndroidThunkJava_GetInputDeviceInfo = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetInputDeviceInfo", "(I)Lcom/epicgames/unreal/GameActivity$InputDeviceInfo;", bIsOptional);
    AndroidThunkJava_SetInputDeviceVibrators = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetInputDeviceVibrators", "(IIIII)Z", bIsOptional);
    AndroidThunkJava_IsGamepadAttached = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsGamepadAttached", "()Z", bIsOptional);
    AndroidThunkJava_HasMetaDataKey = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_HasMetaDataKey", "(Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_GetMetaDataBoolean = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetMetaDataBoolean", "(Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_GetMetaDataInt = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetMetaDataInt", "(Ljava/lang/String;)I", bIsOptional);
    AndroidThunkJava_GetMetaDataLong = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetMetaDataLong", "(Ljava/lang/String;)J", bIsOptional);
    AndroidThunkJava_GetMetaDataFloat = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetMetaDataFloat", "(Ljava/lang/String;)F", bIsOptional);
    AndroidThunkJava_GetMetaDataString = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetMetaDataString", "(Ljava/lang/String;)Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_SetSustainedPerformanceMode = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetSustainedPerformanceMode", "(Z)V", bIsOptional);
    AndroidThunkJava_ShowHiddenAlertDialog = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShowHiddenAlertDialog", "()V", bIsOptional);
    AndroidThunkJava_LocalNotificationScheduleAtTime = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationScheduleAtTime", "(Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)I", bIsOptional);
    AndroidThunkJava_LocalNotificationClearAll = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationClearAll", "()V", bIsOptional);
    AndroidThunkJava_LocalNotificationExists = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationExists", "(I)Z", bIsOptional);
    AndroidThunkJava_LocalNotificationGetLaunchNotification = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationGetLaunchNotification", "()Lcom/epicgames/unreal/GameActivity$LaunchNotification;", bIsOptional);
    AndroidThunkJava_LocalNotificationDestroyIfExists = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LocalNotificationDestroyIfExists", "(I)Z", bIsOptional);
    AndroidThunkJava_GetNetworkConnectionType = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetNetworkConnectionType", "()I", bIsOptional);
    AndroidThunkJava_AddNetworkListener = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_AddNetworkListener", "()V", bIsOptional);
    AndroidThunkJava_RemoveNetworkListener = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_AddNetworkListener", "()V", bIsOptional);
    AndroidThunkJava_GetAndroidId = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetAndroidId", "()Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_ShareURL = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ShareURL", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;II)V", bIsOptional);
    AndroidThunkJava_IsPackageInstalled = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsPackageInstalled", "(Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_LaunchPackage = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_LaunchPackage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_SendBroadcast = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SendBroadcast", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z", bIsOptional);
    AndroidThunkJava_HasIntentExtrasKey = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_HasIntentExtrasKey", "(Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_GetIntentExtrasBoolean = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetIntentExtrasBoolean", "(Ljava/lang/String;)Z", bIsOptional);
    AndroidThunkJava_GetIntentExtrasInt = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetIntentExtrasInt", "(Ljava/lang/String;)I", bIsOptional);
    AndroidThunkJava_GetIntentExtrasString = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetIntentExtrasString", "(Ljava/lang/String;)Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_PushSensorEvents = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_PushSensorEvents", "()V", bIsOptional);
    AndroidThunkJava_SetOrientation = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetOrientation", "(I)V", bIsOptional);
    AndroidThunkJava_SetCellularPreference = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetCellularPreference", "(I)V", bIsOptional);
    AndroidThunkJava_GetCellularPreference = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetCellularPreference", "()I", bIsOptional);

    // Screen capture/recording permission
    AndroidThunkJava_IsScreenCaptureDisabled = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsScreenCaptureDisabled", "()Z", bIsOptional);
    AndroidThunkJava_DisableScreenCapture = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_DisableScreenCapture", "(Z)V", bIsOptional);


    // this is optional - only inserted if Oculus Mobile plugin enabled
    AndroidThunkJava_IsOculusMobileApplication = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsOculusMobileApplication", "()Z", true);

    // this is optional - only inserted if GCM plugin enabled
    AndroidThunkJava_RegisterForRemoteNotifications = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_RegisterForRemoteNotifications", "()V", true);
    AndroidThunkJava_UnregisterForRemoteNotifications = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_UnregisterForRemoteNotifications", "()V", true);
    AndroidThunkJava_IsAllowedRemoteNotifications = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_IsAllowedRemoteNotifications", "()Z", true);

    // get field IDs for InputDeviceInfo class members
    InputDeviceInfoClass = FindClassGlobalRef(Env, "com/epicgames/unreal/GameActivity$InputDeviceInfo", bIsOptional);
    InputDeviceInfo_VendorId = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "vendorId", "I", bIsOptional);
    InputDeviceInfo_ProductId = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "productId", "I", bIsOptional);
    InputDeviceInfo_ControllerId = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "controllerId", "I", bIsOptional);
    InputDeviceInfo_Name = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "name", "Ljava/lang/String;", bIsOptional);
    InputDeviceInfo_Descriptor = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "descriptor", "Ljava/lang/String;", bIsOptional);
    InputDeviceInfo_FeedbackMotorCount = FJavaWrapper::FindField(Env, InputDeviceInfoClass, "feedbackMotorCount", "I", bIsOptional);

    /** GooglePlay services */
    FindGooglePlayMethods(Env);
    /** GooglePlay billing services */
    FindGooglePlayBillingMethods(Env);

    // get field IDs for LaunchNotificationClass class members
    LaunchNotificationClass = FindClassGlobalRef(Env, "com/epicgames/unreal/GameActivity$LaunchNotification", bIsOptional);
    LaunchNotificationUsed = FJavaWrapper::FindField(Env, LaunchNotificationClass, "used", "Z", bIsOptional);
    LaunchNotificationEvent = FJavaWrapper::FindField(Env, LaunchNotificationClass, "event", "Ljava/lang/String;", bIsOptional);
    LaunchNotificationFireDate = FJavaWrapper::FindField(Env, LaunchNotificationClass, "fireDate", "I", bIsOptional);

    ThreadClass = FindClassGlobalRef(Env, "java/lang/Thread", bIsOptional);
    CurrentThreadMethod = FindStaticMethod(Env, ThreadClass, "currentThread", "()Ljava/lang/Thread;", bIsOptional);
    SetNameMethod = FindMethod(Env, ThreadClass, "setName", "(Ljava/lang/String;)V", bIsOptional);

    AndroidThunkJava_RestartApplication = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_RestartApplication", "(Ljava/lang/String;)V", bIsOptional);

    // display refresh rates
    AndroidThunkJava_GetNativeDisplayRefreshRate = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetNativeDisplayRefreshRate", "()I", bIsOptional);
    AndroidThunkJava_SetNativeDisplayRefreshRate = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetNativeDisplayRefreshRate", "(I)Z", bIsOptional);
    AndroidThunkJava_GetSupportedNativeDisplayRefreshRates = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_GetSupportedNativeDisplayRefreshRates", "()[I", bIsOptional);

    // motion controls
    AndroidThunkJava_EnableMotion = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_EnableMotion", "(Z)V", bIsOptional);

    // the rest are optional
    bIsOptional = true;

    // SurfaceView functionality for view scaling on some devices
    AndroidThunkJava_UseSurfaceViewWorkaround = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_UseSurfaceViewWorkaround", "()V", bIsOptional);
    AndroidThunkJava_SetDesiredViewSize = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_SetDesiredViewSize", "(II)V", bIsOptional);

    AndroidThunkJava_VirtualInputIgnoreClick = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_VirtualInputIgnoreClick", "(II)Z", bIsOptional);

    // Multicast lock handling
    AndroidThunkJava_AcquireWifiManagerMulticastLock = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_AcquireWifiManagerMulticastLock", "()Z", bIsOptional);
    AndroidThunkJava_ReleaseWifiManagerMulticastLock = FindMethod(Env, GameActivityClassID, "AndroidThunkJava_ReleaseWifiManagerMulticastLock", "()V", bIsOptional);

    SetupEmbeddedCommunication(Env);
}

void FJavaWrapper::FindGooglePlayMethods(JNIEnv* Env)
{
    bool bIsOptional = true;

    // @todo split GooglePlay
    //	GoogleServicesClassID = FindClass(Env, "com/epicgames/unreal/GoogleServices", bIsOptional);
    GoogleServicesClassID = GameActivityClassID;
    AndroidThunkJava_ResetAchievements = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_ResetAchievements", "()V", bIsOptional);
    AndroidThunkJava_ShowAdBanner = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_ShowAdBanner", "(Ljava/lang/String;Z)V", bIsOptional);
    AndroidThunkJava_HideAdBanner = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_HideAdBanner", "()V", bIsOptional);
    AndroidThunkJava_CloseAdBanner = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_CloseAdBanner", "()V", bIsOptional);
    AndroidThunkJava_LoadInterstitialAd = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_LoadInterstitialAd", "(Ljava/lang/String;)V", bIsOptional);
    AndroidThunkJava_IsInterstitialAdAvailable = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IsInterstitialAdAvailable", "()Z", bIsOptional);
    AndroidThunkJava_IsInterstitialAdRequested = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IsInterstitialAdRequested", "()Z", bIsOptional);
    AndroidThunkJava_ShowInterstitialAd = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_ShowInterstitialAd", "()V", bIsOptional);
    AndroidThunkJava_GetAdvertisingId = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_GetAdvertisingId", "()Ljava/lang/String;", bIsOptional);
    AndroidThunkJava_GoogleClientConnect = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_GoogleClientConnect", "()V", bIsOptional);
    AndroidThunkJava_GoogleClientDisconnect = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_GoogleClientDisconnect", "()V", bIsOptional);
}
void FJavaWrapper::FindGooglePlayBillingMethods(JNIEnv* Env)
{
    // In app purchase functionality
    bool bSupportsInAppPurchasing = false;
//    if (!GConfig->GetBool(TEXT("OnlineSubsystemGooglePlay.Store"), TEXT("bSupportsInAppPurchasing"), bSupportsInAppPurchasing, GEngineIni))
//    {
//        FPlatformMisc::LowLevelOutputDebugString(TEXT("[JNI] - Failed to determine if app purchasing is enabled!"));
//    }
    bool bIsStoreOptional = !bSupportsInAppPurchasing;

    JavaStringClass = FindClassGlobalRef(Env, "java/lang/String", false);
    AndroidThunkJava_IapSetupService = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapSetupService", "(Ljava/lang/String;)V", bIsStoreOptional);
    AndroidThunkJava_IapQueryInAppPurchases = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapQueryInAppPurchases", "([Ljava/lang/String;)Z", bIsStoreOptional);
    AndroidThunkJava_IapBeginPurchase = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapBeginPurchase", "([Ljava/lang/String;Ljava/lang/String;)Z", bIsStoreOptional);
    AndroidThunkJava_IapIsAllowedToMakePurchases = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapIsAllowedToMakePurchases", "()Z", bIsStoreOptional);
    AndroidThunkJava_IapAcknowledgePurchase = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapAcknowledgePurchase", "(Ljava/lang/String;)Z", bIsStoreOptional);
    AndroidThunkJava_IapConsumePurchase = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapConsumePurchase", "(Ljava/lang/String;)Z", bIsStoreOptional);
    AndroidThunkJava_IapQueryExistingPurchases = FindMethod(Env, GoogleServicesClassID, "AndroidThunkJava_IapQueryExistingPurchases", "()Z", bIsStoreOptional);
}

jclass FJavaWrapper::FindClass(JNIEnv* Env, const char* ClassName, bool bIsOptional)
{
    jclass Class = Env->FindClass(ClassName);
    CHECK_JNI_RESULT(Class);
    return Class;
}

jclass FJavaWrapper::FindClassGlobalRef(JNIEnv* Env, const char* ClassName, bool bIsOptional)
{
    auto LocalClass = NewScopedJavaObject(Env, Env->FindClass(ClassName));
    CHECK_JNI_RESULT(*LocalClass);
    if (LocalClass)
    {
        return (jclass)Env->NewGlobalRef(*LocalClass);
    }
    return nullptr;
}

jmethodID FJavaWrapper::FindMethod(JNIEnv* Env, jclass Class, const char* MethodName, const char* MethodSignature, bool bIsOptional)
{
    jmethodID Method = Class == NULL ? NULL : Env->GetMethodID(Class, MethodName, MethodSignature);
    CHECK_JNI_RESULT(Method);
    return Method;
}

jmethodID FJavaWrapper::FindStaticMethod(JNIEnv* Env, jclass Class, const char* MethodName, const char* MethodSignature, bool bIsOptional)
{
    jmethodID Method = Class == NULL ? NULL : Env->GetStaticMethodID(Class, MethodName, MethodSignature);
    CHECK_JNI_RESULT(Method);
    return Method;
}

jfieldID FJavaWrapper::FindField(JNIEnv* Env, jclass Class, const char* FieldName, const char* FieldType, bool bIsOptional)
{
    jfieldID Field = Class == NULL ? NULL : Env->GetFieldID(Class, FieldName, FieldType);
    CHECK_JNI_RESULT(Field);
    return Field;
}

void FJavaWrapper::CallVoidMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    // make sure the function exists
    if (Method == NULL || Object == NULL)
    {
        return;
    }

    va_list Args;
    va_start(Args, Method);
    Env->CallVoidMethodV(Object, Method, Args);
    va_end(Args);
}

jobject FJavaWrapper::CallObjectMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return NULL;
    }

    va_list Args;
    va_start(Args, Method);
    jobject Return = Env->CallObjectMethodV(Object, Method, Args);
    va_end(Args);

    return Return;
}

int32_t FJavaWrapper::CallIntMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return 0;
    }

    va_list Args;
    va_start(Args, Method);
    jint Return = Env->CallIntMethodV(Object, Method, Args);
    va_end(Args);

    return (int32_t)Return;
}

int64_t FJavaWrapper::CallLongMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return 0;
    }

    va_list Args;
    va_start(Args, Method);
    jlong Return = Env->CallLongMethodV(Object, Method, Args);
    va_end(Args);

    return (int64_t)Return;
}

float FJavaWrapper::CallFloatMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return 0.0f;
    }

    va_list Args;
    va_start(Args, Method);
    jfloat Return = Env->CallFloatMethodV(Object, Method, Args);
    va_end(Args);

    return (float)Return;
}

double FJavaWrapper::CallDoubleMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return 0.0;
    }

    va_list Args;
    va_start(Args, Method);
    jdouble Return = Env->CallDoubleMethodV(Object, Method, Args);
    va_end(Args);

    return (double)Return;
}

bool FJavaWrapper::CallBooleanMethod(JNIEnv* Env, jobject Object, jmethodID Method, ...)
{
    if (Method == NULL || Object == NULL)
    {
        return false;
    }

    va_list Args;
    va_start(Args, Method);
    jboolean Return = Env->CallBooleanMethodV(Object, Method, Args);
    va_end(Args);

    return (bool)Return;
}

//Declare all the static members of the class defs
jclass FJavaWrapper::GameActivityClassID;
jobject FJavaWrapper::GameActivityThis;
jmethodID FJavaWrapper::AndroidThunkJava_ShowConsoleWindow;
jmethodID FJavaWrapper::AndroidThunkJava_ShowVirtualKeyboardInputDialog;
jmethodID FJavaWrapper::AndroidThunkJava_HideVirtualKeyboardInputDialog;
jmethodID FJavaWrapper::AndroidThunkJava_ShowVirtualKeyboardInput;
jmethodID FJavaWrapper::AndroidThunkJava_HideVirtualKeyboardInput;
jmethodID FJavaWrapper::AndroidThunkJava_LaunchURL;
jmethodID FJavaWrapper::AndroidThunkJava_GetAssetManager;
jmethodID FJavaWrapper::AndroidThunkJava_Minimize;
jmethodID FJavaWrapper::AndroidThunkJava_ClipboardCopy;
jmethodID FJavaWrapper::AndroidThunkJava_ClipboardPaste;
jmethodID FJavaWrapper::AndroidThunkJava_ForceQuit;
jmethodID FJavaWrapper::AndroidThunkJava_GetFontDirectory;
jmethodID FJavaWrapper::AndroidThunkJava_Vibrate;
jmethodID FJavaWrapper::AndroidThunkJava_IsMusicActive;
jmethodID FJavaWrapper::AndroidThunkJava_IsScreensaverEnabled;
jmethodID FJavaWrapper::AndroidThunkJava_KeepScreenOn;
jmethodID FJavaWrapper::AndroidThunkJava_InitHMDs;
jmethodID FJavaWrapper::AndroidThunkJava_DismissSplashScreen;
jmethodID FJavaWrapper::AndroidThunkJava_ShowProgressDialog;
jmethodID FJavaWrapper::AndroidThunkJava_UpdateProgressDialog;
jmethodID FJavaWrapper::AndroidThunkJava_GetInputDeviceInfo;
jmethodID FJavaWrapper::AndroidThunkJava_SetInputDeviceVibrators;
jmethodID FJavaWrapper::AndroidThunkJava_IsGamepadAttached;
jmethodID FJavaWrapper::AndroidThunkJava_HasMetaDataKey;
jmethodID FJavaWrapper::AndroidThunkJava_GetMetaDataBoolean;
jmethodID FJavaWrapper::AndroidThunkJava_GetMetaDataInt;
jmethodID FJavaWrapper::AndroidThunkJava_GetMetaDataLong;
jmethodID FJavaWrapper::AndroidThunkJava_GetMetaDataFloat;
jmethodID FJavaWrapper::AndroidThunkJava_GetMetaDataString;
jmethodID FJavaWrapper::AndroidThunkJava_IsOculusMobileApplication;
jmethodID FJavaWrapper::AndroidThunkJava_RegisterForRemoteNotifications;
jmethodID FJavaWrapper::AndroidThunkJava_UnregisterForRemoteNotifications;
jmethodID FJavaWrapper::AndroidThunkJava_IsAllowedRemoteNotifications;
jmethodID FJavaWrapper::AndroidThunkJava_ShowHiddenAlertDialog;
jmethodID FJavaWrapper::AndroidThunkJava_LocalNotificationScheduleAtTime;
jmethodID FJavaWrapper::AndroidThunkJava_LocalNotificationClearAll;
jmethodID FJavaWrapper::AndroidThunkJava_LocalNotificationExists;
jmethodID FJavaWrapper::AndroidThunkJava_LocalNotificationGetLaunchNotification;
jmethodID FJavaWrapper::AndroidThunkJava_LocalNotificationDestroyIfExists;
jmethodID FJavaWrapper::AndroidThunkJava_GetNetworkConnectionType;
jmethodID FJavaWrapper::AndroidThunkJava_GetAndroidId;
jmethodID FJavaWrapper::AndroidThunkJava_ShareURL;
jmethodID FJavaWrapper::AndroidThunkJava_IsPackageInstalled;
jmethodID FJavaWrapper::AndroidThunkJava_LaunchPackage;
jmethodID FJavaWrapper::AndroidThunkJava_SendBroadcast;
jmethodID FJavaWrapper::AndroidThunkJava_HasIntentExtrasKey;
jmethodID FJavaWrapper::AndroidThunkJava_GetIntentExtrasBoolean;
jmethodID FJavaWrapper::AndroidThunkJava_GetIntentExtrasInt;
jmethodID FJavaWrapper::AndroidThunkJava_GetIntentExtrasString;
jmethodID FJavaWrapper::AndroidThunkJava_SetSustainedPerformanceMode;
jmethodID FJavaWrapper::AndroidThunkJava_PushSensorEvents;
jmethodID FJavaWrapper::AndroidThunkJava_IsScreenCaptureDisabled;
jmethodID FJavaWrapper::AndroidThunkJava_DisableScreenCapture;
jmethodID FJavaWrapper::AndroidThunkJava_SetOrientation;
jmethodID FJavaWrapper::AndroidThunkJava_SetCellularPreference;
jmethodID FJavaWrapper::AndroidThunkJava_GetCellularPreference;

jclass FJavaWrapper::InputDeviceInfoClass;
jfieldID FJavaWrapper::InputDeviceInfo_VendorId;
jfieldID FJavaWrapper::InputDeviceInfo_ProductId;
jfieldID FJavaWrapper::InputDeviceInfo_ControllerId;
jfieldID FJavaWrapper::InputDeviceInfo_Name;
jfieldID FJavaWrapper::InputDeviceInfo_Descriptor;
jfieldID FJavaWrapper::InputDeviceInfo_FeedbackMotorCount;

jclass FJavaWrapper::GoogleServicesClassID;
jobject FJavaWrapper::GoogleServicesThis;
jmethodID FJavaWrapper::AndroidThunkJava_ResetAchievements;
jmethodID FJavaWrapper::AndroidThunkJava_ShowAdBanner;
jmethodID FJavaWrapper::AndroidThunkJava_HideAdBanner;
jmethodID FJavaWrapper::AndroidThunkJava_CloseAdBanner;
jmethodID FJavaWrapper::AndroidThunkJava_LoadInterstitialAd;
jmethodID FJavaWrapper::AndroidThunkJava_IsInterstitialAdAvailable;
jmethodID FJavaWrapper::AndroidThunkJava_IsInterstitialAdRequested;
jmethodID FJavaWrapper::AndroidThunkJava_ShowInterstitialAd;
jmethodID FJavaWrapper::AndroidThunkJava_GetAdvertisingId;
jmethodID FJavaWrapper::AndroidThunkJava_GoogleClientConnect;
jmethodID FJavaWrapper::AndroidThunkJava_GoogleClientDisconnect;

jclass FJavaWrapper::JavaStringClass;
jmethodID FJavaWrapper::AndroidThunkJava_IapSetupService;
jmethodID FJavaWrapper::AndroidThunkJava_IapQueryInAppPurchases;
jmethodID FJavaWrapper::AndroidThunkJava_IapBeginPurchase;
jmethodID FJavaWrapper::AndroidThunkJava_IapIsAllowedToMakePurchases;
jmethodID FJavaWrapper::AndroidThunkJava_IapQueryExistingPurchases;
jmethodID FJavaWrapper::AndroidThunkJava_IapAcknowledgePurchase;
jmethodID FJavaWrapper::AndroidThunkJava_IapConsumePurchase;

jmethodID FJavaWrapper::AndroidThunkJava_UseSurfaceViewWorkaround;
jmethodID FJavaWrapper::AndroidThunkJava_SetDesiredViewSize;

jmethodID FJavaWrapper::AndroidThunkJava_VirtualInputIgnoreClick;

jmethodID FJavaWrapper::AndroidThunkJava_RestartApplication;

jmethodID FJavaWrapper::AndroidThunkJava_GetSupportedNativeDisplayRefreshRates;
jmethodID FJavaWrapper::AndroidThunkJava_GetNativeDisplayRefreshRate;
jmethodID FJavaWrapper::AndroidThunkJava_SetNativeDisplayRefreshRate;

jmethodID FJavaWrapper::AndroidThunkJava_AddNetworkListener;
jmethodID FJavaWrapper::AndroidThunkJava_RemoveNetworkListener;

jmethodID FJavaWrapper::AndroidThunkJava_EnableMotion;

jclass FJavaWrapper::LaunchNotificationClass;
jfieldID FJavaWrapper::LaunchNotificationUsed;
jfieldID FJavaWrapper::LaunchNotificationEvent;
jfieldID FJavaWrapper::LaunchNotificationFireDate;

jclass FJavaWrapper::ThreadClass;
jmethodID FJavaWrapper::CurrentThreadMethod;
jmethodID FJavaWrapper::SetNameMethod;

jmethodID FJavaWrapper::AndroidThunkJava_AcquireWifiManagerMulticastLock;
jmethodID FJavaWrapper::AndroidThunkJava_ReleaseWifiManagerMulticastLock;

//Game-specific crash reporter
//void EngineCrashHandler(const FGenericCrashContext& GenericContext)
//{
//    const FAndroidCrashContext& Context = static_cast<const FAndroidCrashContext&>(GenericContext);
//
//    static int32_t bHasEntered = 0;
//    if (FPlatformAtomics::InterlockedCompareExchange(&bHasEntered, 1, 0) == 0)
//    {
//        const SIZE_T StackTraceSize = 65535;
//        char StackTrace[StackTraceSize];
//        StackTrace[0] = 0;
//
//        // Walk the stack and dump it to the allocated memory.
//        FPlatformStackWalk::StackWalkAndDump(StackTrace, StackTraceSize, 0, Context.Context);
//        UE_LOG(LogEngine, Error, TEXT("\n%s\n"), ANSI_TO_TCHAR(StackTrace));
//
//        if (GLog)
//        {
//            GLog->Panic();
//        }
//
//        if (GWarn)
//        {
//            GWarn->Flush();
//        }
//    }
//}

bool AndroidThunkCpp_IsScreensaverEnabled()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsScreensaverEnabled);
    }
    return true;
}

void AndroidThunkCpp_KeepScreenOn(bool Enable)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_KeepScreenOn, Enable);
    }
}

void AndroidThunkCpp_Vibrate(int32_t Intensity, int32_t Duration)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_Vibrate, Intensity, Duration);
    }
}

// Call the Java side code for initializing VR HMD modules
void AndroidThunkCpp_InitHMDs()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_InitHMDs);
    }
}

void AndroidThunkCpp_DismissSplashScreen()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_DismissSplashScreen);
    }
}

void AndroidThunkCpp_ShowProgressDialog(bool bShow, const std::string& Message, bool bHorizontal, int32_t MaxValue)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto JavaMessage = FJavaHelper::ToJavaString(Env, Message);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShowProgressDialog, bShow, *JavaMessage, bHorizontal, MaxValue);
    }
}

void AndroidThunkCpp_UpdateProgressDialog(int32_t Value)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_UpdateProgressDialog, Value);
    }
}

//bool AndroidThunkCpp_GetInputDeviceInfo(int32_t deviceId, FAndroidInputDeviceInfo &results)
//{
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        auto deviceInfo = NewScopedJavaObject(Env, (jobject)Env->CallObjectMethod(FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetInputDeviceInfo, deviceId));
//        if (Env->ExceptionCheck())
//        {
//            Env->ExceptionDescribe();
//            Env->ExceptionClear();
//        }
//        else if (deviceInfo)
//        {
//            bool bIsOptional = false;
//            results.DeviceId = deviceId;
//            results.VendorId = (int32_t)Env->GetIntField(*deviceInfo, FJavaWrapper::InputDeviceInfo_VendorId);
//            results.ProductId = (int32_t)Env->GetIntField(*deviceInfo, FJavaWrapper::InputDeviceInfo_ProductId);
//            results.ControllerId = (int32_t)Env->GetIntField(*deviceInfo, FJavaWrapper::InputDeviceInfo_ControllerId);
//
//            results.Name = FJavaHelper::FStringFromLocalRef(Env, (jstring)Env->GetObjectField(*deviceInfo, FJavaWrapper::InputDeviceInfo_Name));
//            results.Descriptor = FJavaHelper::FStringFromLocalRef(Env, (jstring)Env->GetObjectField(*deviceInfo, FJavaWrapper::InputDeviceInfo_Descriptor));
//
//            results.FeedbackMotorCount = (int32_t)Env->GetIntField(*deviceInfo, FJavaWrapper::InputDeviceInfo_FeedbackMotorCount);
//
//            return true;
//        }
//    }
//
//    // failed
//    results.DeviceId = deviceId;
//    results.VendorId = 0;
//    results.ProductId = 0;
//    results.ControllerId = -1;
//    results.Name = std::string("Unknown");
//    results.Descriptor = std::string("Unknown");
//    results.FeedbackMotorCount = 0;
//    return false;
//}

bool AndroidThunkCpp_SetInputDeviceVibrators(int32_t deviceId, int32_t leftIntensity, int32_t leftDuration, int32_t rightIntensity, int32_t rightDuration)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetInputDeviceVibrators, deviceId, leftIntensity, leftDuration, rightIntensity, rightDuration);
    }
    return false;
}

bool AndroidThunkCpp_VirtualInputIgnoreClick(int32_t x, int32_t y)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_VirtualInputIgnoreClick, x, y);
    }
    return Result;
}

void AndroidThunkCpp_RestartApplication(const std::string& IntentString)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, IntentString);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_RestartApplication, *Argument);
    }
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_WebViewVisible(JNIEnv* jenv, jobject thiz, jboolean bShown)
{
    GWebViewShown = bShown;
}

bool AndroidThunkCpp_IsWebViewShown()
{
    return GWebViewShown;
}

//Set GVirtualKeyboardShown.This function is declared in the Java-defined class, GameActivity.java: "public native void nativeVirtualKeyboardVisible(boolean bShown)"
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardVisible(JNIEnv* jenv, jobject thiz, jboolean bShown)
{
    GVirtualKeyboardShown = bShown;

    //remove reference so the object can be clicked again to show the virtual keyboard
    if (!bShown)
    {
        //VirtualKeyboardWidget.Reset();
    }
}

bool AndroidThunkCpp_IsVirtualKeyboardShown()
{
    return GVirtualKeyboardShown;
}

bool AndroidThunkCpp_IsGamepadAttached()
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsGamepadAttached);
    }
    return Result;
}

bool AndroidThunkCpp_HasMetaDataKey(const std::string& Key)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_HasMetaDataKey, *Argument);
    }
    return Result;
}

bool AndroidThunkCpp_GetMetaDataBoolean(const std::string& Key)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetMetaDataBoolean, *Argument);
    }
    return Result;
}

int32_t AndroidThunkCpp_GetMetaDataInt(const std::string& Key)
{
    int32_t Result = 0;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetMetaDataInt, *Argument);
    }
    return Result;
}

int64_t AndroidThunkCpp_GetMetaDataLong(const std::string& Key)
{
    int64_t Result = 0;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = (int64_t)FJavaWrapper::CallLongMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetMetaDataLong, *Argument);
    }
    return Result;
}

float AndroidThunkCpp_GetMetaDataFloat(const std::string& Key)
{
    float Result = 0.0f;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = (float)FJavaWrapper::CallFloatMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetMetaDataFloat, *Argument);
    }
    return Result;
}

std::string AndroidThunkCpp_GetMetaDataString(const std::string& Key)
{
    std::string Result = std::string("");
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaHelper::FStringFromLocalRef(Env, (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetMetaDataString, *Argument));
    }
    return Result;
}

bool AndroidThunkCpp_HasIntentExtrasKey(const std::string& Key)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_HasIntentExtrasKey, *Argument);
    }
    return Result;
}

bool AndroidThunkCpp_GetIntentExtrasBoolean(const std::string& Key)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetIntentExtrasBoolean, *Argument);
    }
    return Result;
}

int32_t AndroidThunkCpp_GetIntentExtrasInt(const std::string& Key)
{
    int32_t Result = 0;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetIntentExtrasInt, *Argument);
    }
    return Result;
}

std::string AndroidThunkCpp_GetIntentExtrasString(const std::string& Key)
{
    std::string Result = std::string("");
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, Key);
        Result = FJavaHelper::FStringFromLocalRef(Env, (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetIntentExtrasString, *Argument));
    }
    return Result;
}

void AndroidThunkCpp_SetSustainedPerformanceMode(bool bEnable)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetSustainedPerformanceMode, bEnable);
    }
}

void AndroidThunkCpp_PushSensorEvents()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_PushSensorEvents);
    }
}

bool AndroidThunkCpp_IsScreenCaptureDisabled()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsScreenCaptureDisabled);
    }
    return true;
}

void AndroidThunkCpp_DisableScreenCapture(bool bDisable)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_DisableScreenCapture, bDisable);
    }
}

void AndroidThunkCpp_ShowHiddenAlertDialog()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShowHiddenAlertDialog);
    }
}

// call out to JNI to see if the application was packaged for Oculus Mobile
bool AndroidThunkCpp_IsOculusMobileApplication()
{
    return false;
    //TODO::
    static int32_t IsOculusMobileApplication = -1;

    if (IsOculusMobileApplication == -1)
    {
        IsOculusMobileApplication = 0;
        if (FJavaWrapper::AndroidThunkJava_IsOculusMobileApplication)
        {
            if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
            {
                IsOculusMobileApplication = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsOculusMobileApplication) ? 1 : 0;
            }
        }
    }
    return IsOculusMobileApplication == 1;
}

// call optional remote notification registration
void AndroidThunkCpp_RegisterForRemoteNotifications()
{
    if (FJavaWrapper::AndroidThunkJava_RegisterForRemoteNotifications)
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_RegisterForRemoteNotifications);
        }
    }
}

// call optional remote notification unregistration
void AndroidThunkCpp_UnregisterForRemoteNotifications()
{
    if (FJavaWrapper::AndroidThunkJava_UnregisterForRemoteNotifications)
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_UnregisterForRemoteNotifications);
        }
    }
}

bool AndroidThunkCpp_IsAllowedRemoteNotifications()
{
    bool Result = false;
    if (FJavaWrapper::AndroidThunkJava_UnregisterForRemoteNotifications)
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsAllowedRemoteNotifications);
        }
    }

    return Result;
}

void AndroidThunkCpp_ShowConsoleWindow()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // figure out all the possible texture formats that are allowed
        std::vector<std::string> PossibleTargetPlatforms;
//        FPlatformMisc::GetValidTargetPlatforms(PossibleTargetPlatforms);

        // separate the format suffixes with commas
        std::string ConsoleText;
//        for (int32_t FormatIndex = 0; FormatIndex < PossibleTargetPlatforms.Num(); FormatIndex++)
//        {
//            const std::string& Format = PossibleTargetPlatforms[FormatIndex];
//            int32_t UnderscoreIndex;
//            if (Format.FindLastChar('_', UnderscoreIndex))
//            {
//                if (ConsoleText != TEXT(""))
//                {
//                    ConsoleText += ", ";
//                }
//
//                ConsoleText += Format.Mid(UnderscoreIndex + 1);
//            }
//        }

        // call the java side
        auto ConsoleTextJava = FJavaHelper::ToJavaString(Env, ConsoleText);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShowConsoleWindow, *ConsoleTextJava);
    }
}

//void AndroidThunkCpp_ShowVirtualKeyboardInputDialog(TSharedPtr<IVirtualKeyboardEntry> TextWidget, int32_t InputType, const std::string& Label, const std::string& Contents)
//{
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        // remember target widget for contents
//        VirtualKeyboardWidget = TextWidget;
//
//        // call the java side
//        auto LabelJava = FJavaHelper::ToJavaString(Env, Label);
//        auto ContentsJava = FJavaHelper::ToJavaString(Env, Contents);
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShowVirtualKeyboardInputDialog, InputType, *LabelJava, *ContentsJava);
//    }
//}
//
//void AndroidThunkCpp_HideVirtualKeyboardInputDialog()
//{
//    // Make sure virtual keyboard currently open
//    if (!VirtualKeyboardWidget.IsValid())
//    {
//        return;
//    }
//
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        // ignore anything it might return
//        VirtualKeyboardWidget.Reset();
//
//        // call the java side
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_HideVirtualKeyboardInputDialog);
//
//        if (FTaskGraphInterface::IsRunning())
//        {
//            FGraphEventRef VirtualKeyboardShown = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
//                                                                                                 {
//                                                                                                     AndroidApplication::Get()->OnVirtualKeyboardHidden().Broadcast();
//                                                                                                 }, TStatId(), NULL, ENamedThreads::GameThread);
//        }
//    }
//}
//
//// This is called from the ViewTreeObserver.OnGlobalLayoutListener in GameActivity
//JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardShown(JNIEnv* jenv, jobject thiz, jint left, jint top, jint right, jint bottom)
//{
//    FPlatformRect ScreenRect(left, top, right, bottom);
//
//    if (FTaskGraphInterface::IsRunning())
//    {
//        FGraphEventRef VirtualKeyboardShown = FFunctionGraphTask::CreateAndDispatchWhenReady([ScreenRect]()
//                                                                                             {
//                                                                                                 AndroidApplication::Get()->OnVirtualKeyboardShown().Broadcast(ScreenRect);
//                                                                                             }, TStatId(), NULL, ENamedThreads::GameThread);
//    }
//}
//
//void AndroidThunkCpp_HideVirtualKeyboardInput()
//{
//    // Make sure virtual keyboard currently open
//    if (!VirtualKeyboardWidget.IsValid())
//    {
//        return;
//    }
//
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        // ignore anything it might return
//        VirtualKeyboardWidget.Reset();
//
//        // call the java side
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_HideVirtualKeyboardInput);
//
//        if( FTaskGraphInterface::IsRunning() )
//        {
//            FGraphEventRef VirtualKeyboardShown = FFunctionGraphTask::CreateAndDispatchWhenReady( [&]()
//                                                                                                  {
//                                                                                                      AndroidApplication::Get()->OnVirtualKeyboardHidden().Broadcast();
//                                                                                                  }, TStatId(), NULL, ENamedThreads::GameThread );
//        }
//    }
//}
//
//void AndroidThunkCpp_ShowVirtualKeyboardInput(TSharedPtr<IVirtualKeyboardEntry> TextWidget, int32_t InputType, const std::string& Label, const std::string& Contents)
//{
//    //#jira UE-49139 Tapping in the same text box doesn't make the virtual keyboard disappear
//    if (VirtualKeyboardWidget.HasSameObject(TextWidget.Get()))
//    {
//        FPlatformMisc::LowLevelOutputDebugString(TEXT("[JNI] - AndroidThunkCpp_ShowVirtualKeyboardInput same control"));
//        AndroidThunkCpp_HideVirtualKeyboardInput();
//    }
//    else if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        // remember target widget for contents
//        VirtualKeyboardWidget = TextWidget;
//
//        // call the java side
//        auto LabelJava = FJavaHelper::ToJavaString(Env, Label);
//        auto ContentsJava = FJavaHelper::ToJavaString(Env, Contents);
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShowVirtualKeyboardInput, InputType, *LabelJava, *ContentsJava);
//    }
//}
//
////This function is declared in the Java-defined class, GameActivity.java: "public native void nativeVirtualKeyboardResult(bool update, String contents);"
//JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardResult(JNIEnv* jenv, jobject thiz, jboolean update, jstring contents)
//{
//    // update text widget with new contents if OK pressed
//    if (update == JNI_TRUE)
//    {
//        if (VirtualKeyboardWidget.IsValid())
//        {
//            std::string Contents = FJavaHelper::FStringFromParam(jenv, contents);
//
//            // call to set the widget text on game thread
//            if (FTaskGraphInterface::IsRunning())
//            {
//                FFunctionGraphTask::CreateAndDispatchWhenReady([Contents=Contents]()
//                                                               {
//                                                                   TSharedPtr<IVirtualKeyboardEntry> LockedKeyboardWidget(VirtualKeyboardWidget.Pin());
//                                                                   if (LockedKeyboardWidget.IsValid())
//                                                                   {
//                                                                       LockedKeyboardWidget->SetTextFromVirtualKeyboard(FText::FromString(Contents), ETextEntryType::TextEntryAccepted);
//                                                                   }
//
//                                                                   // release reference
//                                                                   VirtualKeyboardWidget.Reset();
//                                                               }, TStatId(), NULL, ENamedThreads::GameThread);
//            }
//            else
//            {
//                // release reference
//                VirtualKeyboardWidget.Reset();
//            }
//        }
//    }
//    else
//    {
//        // release reference
//        VirtualKeyboardWidget.Reset();
//    }
//}
//
////This function is declared in the Java-defined class, GameActivity.java: "public native void nativeVirtualKeyboardChanged(String contents);"
//JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardChanged(JNIEnv* jenv, jobject thiz, jstring contents)
//{
//    if (VirtualKeyboardWidget.IsValid())
//    {
//        std::string Contents = FJavaHelper::FStringFromParam(jenv, contents);
//
//        // call to set the widget text on game thread
//        if (FTaskGraphInterface::IsRunning())
//        {
//            FFunctionGraphTask::CreateAndDispatchWhenReady([Contents=Contents]()
//                                                           {
//                                                               TSharedPtr<IVirtualKeyboardEntry> LockedKeyboardWidget(VirtualKeyboardWidget.Pin());
//                                                               if (LockedKeyboardWidget.IsValid())
//                                                               {
//                                                                   LockedKeyboardWidget->SetTextFromVirtualKeyboard(FText::FromString(Contents), ETextEntryType::TextEntryUpdated);
//                                                               }
//                                                           }, TStatId(), NULL, ENamedThreads::GameThread);
//        }
//    }
//}
//
//JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardSendKey(JNIEnv* jenv, jobject thiz, jint keyCode)
//{
//    FDeferredAndroidMessage Message;
//
//    Message.messageType = MessageType_KeyDown;
//    Message.KeyEventData.keyId = keyCode;
//    FAndroidInputInterface::DeferMessage(Message);
//}
//
//JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeVirtualKeyboardSendSelection(JNIEnv* jenv, jobject thiz, jint selStart, jint selEnd)
//{
//    // call to set the widget selection on game thread
//    if (VirtualKeyboardWidget.IsValid())
//    {
//        if (FTaskGraphInterface::IsRunning())
//        {
//            FGraphEventRef SetWidgetSelection = FFunctionGraphTask::CreateAndDispatchWhenReady([&, selStart, selEnd]()
//                                                                                               {
//                                                                                                   TSharedPtr<IVirtualKeyboardEntry> LockedKeyboardWidget(VirtualKeyboardWidget.Pin());
//                                                                                                   if (LockedKeyboardWidget.IsValid())
//                                                                                                   {
//                                                                                                       LockedKeyboardWidget->SetSelectionFromVirtualKeyboard(selStart, selEnd);
//                                                                                                   }
//                                                                                               }, TStatId(), NULL, ENamedThreads::GameThread);
//        }
//    }
//}

void AndroidThunkCpp_LaunchURL(const std::string& URL)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto Argument = FJavaHelper::ToJavaString(Env, URL);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LaunchURL, *Argument);
    }
}

void AndroidThunkCpp_ResetAchievements()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_ResetAchievements);
    }
}

void AndroidThunkCpp_ShowAdBanner(const std::string& AdUnitID, bool bShowOnBottomOfScreen)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto AdUnitIDArg = FJavaHelper::ToJavaString(Env, AdUnitID);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_ShowAdBanner, *AdUnitIDArg, bShowOnBottomOfScreen);
    }
}

void AndroidThunkCpp_HideAdBanner()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_HideAdBanner);
    }
}

void AndroidThunkCpp_CloseAdBanner()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_CloseAdBanner);
    }
}

void AndroidThunkCpp_LoadInterstitialAd(const std::string& AdUnitID)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto AdUnitIDArg = FJavaHelper::ToJavaString(Env, AdUnitID);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_LoadInterstitialAd, *AdUnitIDArg);
    }
}

bool AndroidThunkCpp_IsInterstitialAdAvailable()
{
    bool bIsAdAvailable = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        bIsAdAvailable = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IsInterstitialAdAvailable);
    }

    return bIsAdAvailable;
}

bool AndroidThunkCpp_IsInterstitialAdRequested()
{
    bool bIsAdRequested = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        bIsAdRequested = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IsInterstitialAdRequested);
    }

    return bIsAdRequested;
}

void AndroidThunkCpp_ShowInterstitialAd()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_ShowInterstitialAd);
    }
}

std::string AndroidThunkCpp_GetAdvertisingId()
{
    std::string adIdResult = std::string("");

    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        adIdResult = FJavaHelper::FStringFromLocalRef(Env, (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_GetAdvertisingId));
    }
    return adIdResult;
}

std::string AndroidThunkCpp_GetAndroidId()
{
    std::string androidIdResult = std::string("");

    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        androidIdResult = FJavaHelper::FStringFromLocalRef(Env, (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetAndroidId));
    }
    return androidIdResult;
}

//void AndroidThunkCpp_ShareURL(const std::string& URL, const FText& Description, const FText& SharePrompt, int32_t LocationHintX, int32_t LocationHintY)
//{
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        auto URLArg = FJavaHelper::ToJavaString(Env, URL);
//        auto DescArg = FJavaHelper::ToJavaString(Env, Description.ToString());
//        auto PromptArg = FJavaHelper::ToJavaString(Env, SharePrompt.ToString());
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ShareURL, *URLArg, *DescArg, *PromptArg, LocationHintX, LocationHintY);
//    }
//}

bool AndroidThunkCpp_IsPackageInstalled(const std::string& PackageName)
{
    bool result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto PackageNameArg = FJavaHelper::ToJavaString(Env, PackageName);
        result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsPackageInstalled, *PackageNameArg);
    }
    return result;
}

bool AndroidThunkCpp_LaunchPackage(const std::string& PackageName, const std::string& ExtraKey, const std::string& ExtraValue)
{
    bool result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto PackageNameArg = FJavaHelper::ToJavaString(Env, PackageName);
        auto ExtraKeyArg = FJavaHelper::ToJavaString(Env, ExtraKey);
        auto ExtraValueArg = FJavaHelper::ToJavaString(Env, ExtraValue);
        result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LaunchPackage, *PackageNameArg, *ExtraKeyArg, *ExtraValueArg);
    }
    return result;
}

bool AndroidThunkCpp_SendBroadcast(const std::string& PackageName, const std::string& ExtraKey, const std::string& ExtraValue, bool bExit)
{
    bool result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto PackageNameArg = FJavaHelper::ToJavaString(Env, PackageName);
        auto ExtraKeyArg = FJavaHelper::ToJavaString(Env, ExtraKey);
        auto ExtraValueArg = FJavaHelper::ToJavaString(Env, ExtraValue);

        result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SendBroadcast, *PackageNameArg, *ExtraKeyArg, *ExtraValueArg, bExit);
    }
    return result;
}

void AndroidThunkCpp_GoogleClientConnect()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_GoogleClientConnect);
    }
}

void AndroidThunkCpp_GoogleClientDisconnect()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_GoogleClientDisconnect);
    }
}

namespace
{
    jobject GJavaAssetManager = NULL;
    AAssetManager* GAssetManagerRef = NULL;
}

jobject AndroidJNI_GetJavaAssetManager()
{
    if (!GJavaAssetManager)
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            auto local = NewScopedJavaObject(Env, FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetAssetManager));
            GJavaAssetManager = (jobject)Env->NewGlobalRef(*local);
        }
    }
    return GJavaAssetManager;
}

AAssetManager * AndroidThunkCpp_GetAssetManager()
{
    if (!GAssetManagerRef)
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            jobject JavaAssetMgr = AndroidJNI_GetJavaAssetManager();
            GAssetManagerRef = AAssetManager_fromJava(Env, JavaAssetMgr);
        }
    }

    return GAssetManagerRef;
}

void AndroidThunkCpp_Minimize()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_Minimize);
    }
}

void AndroidThunkCpp_ForceQuit()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ForceQuit);
    }
}

void AndroidThunkCpp_SetOrientation(int32_t Value)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetOrientation, Value);
    }
}

void AndroidThunkCpp_SetCellularPreference(int32_t Value)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetCellularPreference, Value);
    }
}

int32_t AndroidThunkCpp_GetCellularPreference()
{
    int32_t value = 0;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetCellularPreference);
    }
    return value;
}

bool AndroidThunkCpp_IsMusicActive()
{
    bool bIsActive = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        bIsActive = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_IsMusicActive);
    }

    return bIsActive;
}

void AndroidThunkCpp_Iap_SetupIapService(const std::string& InProductKey)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto ProductKey = FJavaHelper::ToJavaString(Env, InProductKey);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapSetupService, *ProductKey);
    }
}

bool AndroidThunkCpp_Iap_QueryInAppPurchases(const std::vector<std::string>& ProductIDs)
{
    //FPlatformMisc::LowLevelOutputDebugString(TEXT("[JNI] - AndroidThunkCpp_Iap_QueryInAppPurchases"));
    bool bResult = false;

    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapQueryInAppPurchases);

        // Populate some java types with the provided product information
        auto ProductIDArray = NewScopedJavaObject(Env, (jobjectArray)Env->NewObjectArray(ProductIDs.size(), FJavaWrapper::JavaStringClass, NULL));
        if (ProductIDArray)
        {
            for (uint32_t Param = 0; Param < ProductIDs.size(); Param++)
            {
                auto StringValue = FJavaHelper::ToJavaString(Env, ProductIDs[Param]);
                Env->SetObjectArrayElement(*ProductIDArray, Param, *StringValue);
            }

            // Execute the java code for this operation
            bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapQueryInAppPurchases, *ProductIDArray);
        }
    }

    return bResult;
}

bool AndroidThunkCpp_Iap_QueryInAppPurchases(const std::vector<std::string>& ProductIDs, const std::vector<bool>& bConsumable)
{
    //FPlatformMisc::LowLevelOutputDebugString(TEXT("AndroidThunkCpp_Iap_QueryInAppPurchases DEPRECATED, won't use consumables array"));
    return AndroidThunkCpp_Iap_QueryInAppPurchases(ProductIDs);
}

bool AndroidThunkCpp_Iap_BeginPurchase(const std::vector<std::string>& ProductIds, const std::string& AccountId)
{
   // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_BeginPurchase"));
    bool bResult = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapBeginPurchase);

        auto ProductIdsJava = FJavaHelper::ToJavaStringArray(Env, ProductIds);
        if (AccountId.empty())
        {
            bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapBeginPurchase, *ProductIdsJava, nullptr);
        }
        else
        {
            auto ObfuscatedAccountIdJava = FJavaHelper::ToJavaString(Env, AccountId);
            bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapBeginPurchase, *ProductIdsJava, *ObfuscatedAccountIdJava);
        }
    }

    return bResult;
}

bool AndroidThunkCpp_Iap_ConsumePurchase(const std::string& ProductToken)
{
    //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_ConsumePurchase %s"), *ProductToken);

    bool bResult = false;
    if (!ProductToken.empty())
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapConsumePurchase);

            auto ProductTokenJava = FJavaHelper::ToJavaString(Env, ProductToken);
            //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_ConsumePurchase BEGIN"));
            bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapConsumePurchase, *ProductTokenJava);
            //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_ConsumePurchase END"));
        }
    }

    return bResult;
}

bool AndroidThunkCpp_Iap_AcknowledgePurchase(const std::string& ProductToken)
{
    //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_AcknowledgePurchase %s"), *ProductToken);

    bool bResult = false;
    if (!ProductToken.empty())
    {
        if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
        {
            CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapAcknowledgePurchase);

            auto ProductTokenJava = FJavaHelper::ToJavaString(Env, ProductToken);
            //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_AcknowledgePurchase BEGIN"));
            bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapAcknowledgePurchase, *ProductTokenJava);
            //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_AcknowledgePurchase END"));
        }
    }

    return bResult;
}

bool AndroidThunkCpp_Iap_QueryExistingPurchases()
{
   // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_QueryExistingPurchases"));

    bool bResult = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapQueryExistingPurchases);

        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_QueryExistingPurchases BEGIN"));
        bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapQueryExistingPurchases);
        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("[JNI] - AndroidThunkCpp_Iap_QueryExistingPurchases END"));
    }

    return bResult;
}

bool AndroidThunkCpp_Iap_IsAllowedToMakePurchases()
{
   // FPlatformMisc::LowLevelOutputDebugString(TEXT("[JNI] - AndroidThunkCpp_Iap_IsAllowedToMakePurchases"));
    bool bResult = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        CHECK_JNI_METHOD(FJavaWrapper::AndroidThunkJava_IapIsAllowedToMakePurchases);

        bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GoogleServicesThis, FJavaWrapper::AndroidThunkJava_IapIsAllowedToMakePurchases);
    }
    return bResult;
}

void AndroidThunkCpp_UseSurfaceViewWorkaround()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_UseSurfaceViewWorkaround);
    }
}

void AndroidThunkCpp_SetDesiredViewSize(int32_t Width, int32_t Height)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetDesiredViewSize, Width, Height);
    }

//// #if BUILD_EMBEDDED_APP
//
//    // also send info to native wrapper around embedded
//    FEmbeddedCallParamsHelper Helper;
//    Helper.Command = TEXT("setueresolution");
//    Helper.Parameters = { {TEXT("width"), LexToString(Width)}, {TEXT("height"), LexToString(Height)} };
//    FEmbeddedDelegates::GetEmbeddedToNativeParamsDelegateForSubsystem(TEXT("native")).Broadcast(Helper);

// #endif
}

//int32_t AndroidThunkCpp_ScheduleLocalNotificationAtTime(const FDateTime& FireDateTime, bool LocalTime, const FText& Title, const FText& Body, const FText& Action, const std::string& ActivationEvent, int32_t IdOverride)
//{
//    //Convert FireDateTime to yyyy-MM-dd HH:mm:ss in order to pass to java
//    std::string FireDateTimeFormatted = std::string::FromInt(FireDateTime.GetYear()) + "-" + std::string::FromInt(FireDateTime.GetMonth()) + "-" + std::string::FromInt(FireDateTime.GetDay()) + " " + std::string::FromInt(FireDateTime.GetHour()) + ":" + std::string::FromInt(FireDateTime.GetMinute()) + ":" + std::string::FromInt(FireDateTime.GetSecond());
//
//    JNIEnv* Env = AndroidApplication::GetJavaEnv();
//    if (Env != NULL)
//    {
//        auto jFireDateTime = FJavaHelper::ToJavaString(Env, FireDateTimeFormatted);
//        auto jTitle = FJavaHelper::ToJavaString(Env, Title.ToString());
//        auto jBody = FJavaHelper::ToJavaString(Env, Body.ToString());
//        auto jAction = FJavaHelper::ToJavaString(Env, Action.ToString());
//        auto jActivationEvent = FJavaHelper::ToJavaString(Env, ActivationEvent);
//
//        return FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LocalNotificationScheduleAtTime, *jFireDateTime, LocalTime, *jTitle, *jBody, *jAction, *jActivationEvent, IdOverride);
//    }
//
//    return -1;
//}

void AndroidThunkCpp_GetLaunchNotification(bool& NotificationLaunchedApp, std::string& ActivationEvent, int32_t& FireDate)
{
    bool bIsOptional = false;

    NotificationLaunchedApp = false;
    ActivationEvent = "";
    FireDate = 0;

    JNIEnv* Env = AndroidApplication::GetJavaEnv();
    if (Env != NULL)
    {
        auto launchInfo = NewScopedJavaObject(Env, (jobject)Env->CallObjectMethod(FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LocalNotificationGetLaunchNotification));
        if (launchInfo && FJavaWrapper::LaunchNotificationUsed != 0)
        {
            NotificationLaunchedApp = (bool)Env->GetBooleanField(*launchInfo, FJavaWrapper::LaunchNotificationUsed);

            ActivationEvent = FJavaHelper::FStringFromLocalRef(Env, (jstring)Env->GetObjectField(*launchInfo, FJavaWrapper::LaunchNotificationEvent));

            FireDate = (int32_t)Env->GetIntField(*launchInfo, FJavaWrapper::LaunchNotificationFireDate);
        }
    }
}

void AndroidThunkCpp_ClearAllLocalNotifications()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LocalNotificationClearAll);
    }
}

bool AndroidThunkCpp_LocalNotificationExists(int32_t NotificationId)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LocalNotificationExists, NotificationId);
    }
    return Result;
}

bool AndroidThunkCpp_DestroyScheduledNotificationIfExists(int32_t NotificationId)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_LocalNotificationDestroyIfExists, NotificationId);
    }
    return Result;
}

int32_t AndroidThunkCpp_GetNetworkConnectionType()
{
    int32_t result = -1;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        result = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetNetworkConnectionType);
    }

    return result;
}

bool AndroidThunkCpp_AcquireWifiManagerMulticastLock()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_AcquireWifiManagerMulticastLock);
    }
    return false;
}

void AndroidThunkCpp_ReleaseWifiManagerMulticastLock()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ReleaseWifiManagerMulticastLock);
    }
}

//The JNI_OnLoad function is triggered by loading the game library from
//the Java source file.
//	static
//	{
//		System.loadLibrary("MyGame");
//	}
//
// Use the JNI_OnLoad function to map all the class IDs and method IDs to their respective
// variables. That way, later when the Java functions need to be called, the IDs will be ready.
// It is much slower to keep looking up the class and method IDs.

JNIEXPORT jint JNI_OnLoad(JavaVM* InJavaVM, void* InReserved)
{
    //FPlatformMisc::LowLevelOutputDebugString(TEXT("In the JNI_OnLoad function"));

    JNIEnv* Env = NULL;
    InJavaVM->GetEnv((void **)&Env, JNI_CURRENT_VERSION);

    // if you have problems with stuff being missing especially in distribution builds then it could be because proguard is stripping things from java
    // check proguard-project.txt and see if your stuff is included in the exceptions
    GJavaVM = InJavaVM;
    AndroidApplication::InitializeJavaEnv(GJavaVM, JNI_CURRENT_VERSION, FJavaWrapper::GameActivityThis);

    FJavaWrapper::FindClassesAndMethods(Env);

    // hook signals
  //  if (!FPlatformMisc::IsDebuggerPresent() || GAlwaysReportCrash)
    {
        // disable crash handler.. getting better stack traces from system for now
        //FPlatformMisc::SetCrashHandler(EngineCrashHandler);
    }

    // Cache path to external storage
    auto EnvClass = NewScopedJavaObject(Env, Env->FindClass("android/os/Environment"));
    jmethodID getExternalStorageDir = Env->GetStaticMethodID(*EnvClass, "getExternalStorageDirectory", "()Ljava/io/File;");
    auto externalStoragePath = NewScopedJavaObject(Env, Env->CallStaticObjectMethod(*EnvClass, getExternalStorageDir, nullptr));
    jmethodID getFilePath = Env->GetMethodID(Env->FindClass("java/io/File"), "getPath", "()Ljava/lang/String;");
    // Copy that somewhere safe
    GFilePathBase = FJavaHelper::FStringFromLocalRef(Env, (jstring)Env->CallObjectMethod(*externalStoragePath, getFilePath, nullptr));
    GOBBFilePathBase = GFilePathBase;
    GOBBMainFilePath = ("");
    GOBBPatchFilePath = ("");
    GOBBOverflow1FilePath = ("");
    GOBBOverflow2FilePath = ("");

    // then release...
   // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Path found as '%s'\n"), *GFilePathBase);

    // Get the system font directory
    GFontPathBase = FJavaHelper::FStringFromLocalRef(Env, (jstring)Env->CallStaticObjectMethod(FJavaWrapper::GameActivityClassID, FJavaWrapper::AndroidThunkJava_GetFontDirectory));
   // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Font Path found as '%s'\n"), *GFontPathBase);

//    // Wire up to core delegates, so core code can call out to Java
//    DECLARE_DELEGATE_OneParam(FAndroidLaunchURLDelegate, const std::string&);
//    extern  FAndroidLaunchURLDelegate OnAndroidLaunchURL;
//    OnAndroidLaunchURL = FAndroidLaunchURLDelegate::CreateStatic(&AndroidThunkCpp_LaunchURL);

    //FPlatformMisc::LowLevelOutputDebugString(TEXT("In the JNI_OnLoad function 5"));

    return JNI_CURRENT_VERSION;
}

//Native-defined functions

//This function is declared in the Java-defined class, GameActivity.java: "public native void naativeSetObbFilePaths();"
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetObbFilePaths(JNIEnv* jenv, jobject thiz, jstring OBBMainFilePath, jstring OBBPatchFilePath, jstring OBBOverflow1FilePath, jstring OBBOverflow2FilePath)
{
    GOBBMainFilePath = FJavaHelper::FStringFromParam(jenv, OBBMainFilePath);
    GOBBPatchFilePath = FJavaHelper::FStringFromParam(jenv, OBBPatchFilePath);
    GOBBOverflow1FilePath = FJavaHelper::FStringFromParam(jenv, OBBOverflow1FilePath);
    GOBBOverflow2FilePath = FJavaHelper::FStringFromParam(jenv, OBBOverflow2FilePath);
}

//This function is declared in the Java-defined class, GameActivity.java: "public native void nativeSetGlobalActivity();"
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeSetGlobalActivity(JNIEnv* jenv, jobject thiz, jboolean bUseExternalFilesDir, jboolean bPublicLogFiles, jstring internalFilePath, jstring externalFilePath, jboolean bOBBinAPK, jstring APKFilename /*, jobject googleServices*/)
{
    if (!FJavaWrapper::GameActivityThis)
    {
        GGameActivityThis = FJavaWrapper::GameActivityThis = jenv->NewGlobalRef(thiz);
        if (!FJavaWrapper::GameActivityThis)
        {
            //FPlatformMisc::LowLevelOutputDebugString(TEXT("Error setting the global GameActivity activity"));
            GE_ASSERT(false);
        }

        // This call is only to set the correct GameActivityThis
        AndroidApplication::InitializeJavaEnv(GJavaVM, JNI_CURRENT_VERSION, FJavaWrapper::GameActivityThis);

        // @todo split GooglePlay, this needs to be passed in to this function
        FJavaWrapper::GoogleServicesThis = FJavaWrapper::GameActivityThis;
        // FJavaWrapper::GoogleServicesThis = jenv->NewGlobalRef(googleServices);

        // Next we check to see if the OBB file is in the APK
        //jmethodID isOBBInAPKMethod = jenv->GetStaticMethodID(FJavaWrapper::GameActivityClassID, "isOBBInAPK", "()Z");
        //GOBBinAPK = (bool)jenv->CallStaticBooleanMethod(FJavaWrapper::GameActivityClassID, isOBBInAPKMethod, nullptr);
        GOBBinAPK = bOBBinAPK;

        GAPKFilename = FJavaHelper::FStringFromParam(jenv, APKFilename);
        GInternalFilePath = FJavaHelper::FStringFromParam(jenv, internalFilePath);
        GExternalFilePath = FJavaHelper::FStringFromParam(jenv, externalFilePath);

        if (bUseExternalFilesDir)
        {
#if UE_BUILD_SHIPPING
            GFilePathBase = GInternalFilePath;
			GOverrideAndroidLogDir = bPublicLogFiles;
#else
            GFilePathBase = GExternalFilePath;
#endif
           // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("GFilePathBase Path override to'%s'\n"), *GFilePathBase);
        }

        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("InternalFilePath found as '%s'\n"), *GInternalFilePath);
       // FPlatformMisc::LowLevelOutputDebugStringf(TEXT("ExternalFilePath found as '%s'\n"), *GExternalFilePath);
    }
}


JNI_METHOD bool Java_com_epicgames_unreal_GameActivity_nativeIsShippingBuild(JNIEnv* LocalJNIEnv, jobject LocalThiz)
{
#if UE_BUILD_SHIPPING
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeOnActivityResult(JNIEnv* jenv, jobject thiz, jobject activity, jint requestCode, jint resultCode, jobject data)
{
    //FJavaWrapper::OnActivityResultDelegate.Broadcast(jenv, thiz, activity, requestCode, resultCode, data);
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeHandleSensorEvents(JNIEnv* jenv, jobject thiz, jfloatArray tilt, jfloatArray rotation_rate, jfloatArray gravity, jfloatArray acceleration)
{
//    jfloat* tiltFloatValues = jenv->GetFloatArrayElements(tilt, 0);
//    FVector current_tilt(tiltFloatValues[0], tiltFloatValues[1], tiltFloatValues[2]);
//    jenv->ReleaseFloatArrayElements(tilt, tiltFloatValues, 0);
//
//    jfloat* rotation_rate_FloatValues = jenv->GetFloatArrayElements(rotation_rate, 0);
//    FVector current_rotation_rate(rotation_rate_FloatValues[0], rotation_rate_FloatValues[1], rotation_rate_FloatValues[2]);
//    jenv->ReleaseFloatArrayElements(rotation_rate, rotation_rate_FloatValues, 0);
//
//    jfloat* gravity_FloatValues = jenv->GetFloatArrayElements(gravity, 0);
//    FVector current_gravity(gravity_FloatValues[0], gravity_FloatValues[1], gravity_FloatValues[2]);
//    jenv->ReleaseFloatArrayElements(gravity, gravity_FloatValues, 0);
//
//    jfloat* acceleration_FloatValues = jenv->GetFloatArrayElements(acceleration, 0);
//    FVector current_acceleration(acceleration_FloatValues[0], acceleration_FloatValues[1], acceleration_FloatValues[2]);
//    jenv->ReleaseFloatArrayElements(acceleration, acceleration_FloatValues, 0);
//
//    FAndroidInputInterface::QueueMotionData(current_tilt, current_rotation_rate, current_gravity, current_acceleration);

}

void AndroidThunkCpp_ClipboardCopy(const std::string& Str)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        auto JStr = FJavaHelper::ToJavaString(Env, Str);
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ClipboardCopy, *JStr);
    }
}

std::string AndroidThunkCpp_ClipboardPaste()
{
    std::string PasteStringResult = std::string("");

    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        PasteStringResult = FJavaHelper::FStringFromLocalRef(Env, (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_ClipboardPaste));
    }
    return PasteStringResult;
}

std::vector<int32_t> AndroidThunkCpp_GetSupportedNativeDisplayRefreshRates()
{
    std::vector<int32_t> Result;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        jintArray JavaIntArray = (jintArray)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetSupportedNativeDisplayRefreshRates);
        if (JavaIntArray != nullptr)
        {
            jint* JavaInts = Env->GetIntArrayElements(JavaIntArray, 0);
            if (JavaInts)
            {
                jsize IntsLen = Env->GetArrayLength(JavaIntArray);
                Result.reserve(IntsLen);
                for (int32_t Idx = 0; Idx < IntsLen; Idx++)
                {
                    Result.push_back(JavaInts[Idx]);
                }
                Env->ReleaseIntArrayElements(JavaIntArray, JavaInts, 0);
            }
            Env->DeleteLocalRef(JavaIntArray);
        }
    }
    return Result;
}

void AndroidThunkJava_AddNetworkListener()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_AddNetworkListener);
    }
}

void AndroidThunkJava_RemoveNetworkListener()
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_RemoveNetworkListener);
    }
}

bool AndroidThunkCpp_SetNativeDisplayRefreshRate(int32_t RefreshRate)
{
    bool Result = false;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetNativeDisplayRefreshRate, RefreshRate);
    }
    return Result;
}

int32_t AndroidThunkCpp_GetNativeDisplayRefreshRate()
{
    int32_t Result = 60;
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        Result = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_GetNativeDisplayRefreshRate);
    }
    return Result;
}

void AndroidThunkCpp_EnableMotion(bool bEnable)
{
    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
    {
        // call the java side
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_EnableMotion, bEnable);
    }
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeOnSafetyNetAttestationSucceeded(JNIEnv* jenv, jobject thiz, jstring jwsData)
{
//    std::string JwsString = FJavaHelper::FStringFromParam(jenv, jwsData);
//
//    // call to OnSafetyNetAttestationResultDelegate on game thread
//    if (FTaskGraphInterface::IsRunning())
//    {
//        FGraphEventRef SafetyNetAttestationSucceeded = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
//                                                                                                      {
//                                                                                                          FJavaWrapper::OnSafetyNetAttestationResultDelegate.Broadcast(true, JwsString, 0);
//                                                                                                      }, TStatId(), NULL, ENamedThreads::GameThread);
//        FTaskGraphInterface::Get().WaitUntilTaskCompletes(SafetyNetAttestationSucceeded);
//    }
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeOnSafetyNetAttestationFailed(JNIEnv* jenv, jobject thiz, jint jwsValue)
{
    // call to OnSafetyNetAttestationResultDelegate on game thread
//    if (FTaskGraphInterface::IsRunning())
//    {
//        FGraphEventRef SafetyNetAttestationFailed = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
//                                                                                                   {
//                                                                                                       FJavaWrapper::OnSafetyNetAttestationResultDelegate.Broadcast(false, std::string(TEXT("")), jwsValue);
//                                                                                                   }, TStatId(), NULL, ENamedThreads::GameThread);
//        FTaskGraphInterface::Get().WaitUntilTaskCompletes(SafetyNetAttestationFailed);
//    }
}


static jmethodID ReplyMethod;
//
//void AndroidThunkCpp_OnNativeToEmbeddedReply(std::string ID, const FEmbeddedCommunicationMap& InReturnValues, std::string InError, std::string RoutingFunction)
//{
//    if (JNIEnv* Env = AndroidApplication::GetJavaEnv())
//    {
//        // marshall some data
//#if !UE_BUILD_SHIPPING
//        UE_LOG(LogInit, Display, TEXT("Java call Id: %s, Routing Function: %s, Error %s, Params:"), *ID, *RoutingFunction, *InError);
//#endif
//
//        // create a string array for the pairs
//        static auto StringClass = FJavaWrapper::FindClassGlobalRef(Env, "java/lang/String", false);
//        auto ReturnValues = NewScopedJavaObject(Env, Env->NewObjectArray(InReturnValues.size() * 2, StringClass, 0));
//        int32_t Index = 0;
//        for (auto It : InReturnValues)
//        {
//            auto KeyString = FJavaHelper::ToJavaString(Env, It.Key);
//            auto ValueString = FJavaHelper::ToJavaString(Env, It.Value);
//            Env->SetObjectArrayElement(*ReturnValues, Index++, *KeyString);
//            Env->SetObjectArrayElement(*ReturnValues, Index++, *ValueString);
//#if !UE_BUILD_SHIPPING
//            UE_LOG(LogInit, Display, TEXT("  %s : %s"), *It.Key, *It.Value);
//#endif
//        }
//        auto Error = FJavaHelper::ToJavaString(Env, InError);
//
//        // call back into java
//        auto IDReturn = FJavaHelper::ToJavaString(Env, ID);
//        auto RoutingFunctionReturn = FJavaHelper::ToJavaString(Env, RoutingFunction);
//
//        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, ReplyMethod, *IDReturn, *ReturnValues, *Error, *RoutingFunctionReturn);
//    }
//}



JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_CallNativeToEmbedded(JNIEnv* jenv, jobject thiz, jstring InID, jint Priority, jstring InSubsystem, jstring InCommand, jobjectArray InParams, jstring InRoutingFunction)
{
#if BUILD_EMBEDDED_APP
    auto Subsystem = FJavaHelper::FStringFromParam(jenv, InSubsystem);
	auto Command = FJavaHelper::FStringFromParam(jenv, InCommand);
	auto ID = FJavaHelper::FStringFromParam(jenv, InID);
	auto RoutingFunction = FJavaHelper::FStringFromParam(jenv, InRoutingFunction);

	FEmbeddedCallParamsHelper Helper;
	Helper.Command = Command;

	FName SubsystemName(*Subsystem);
	FName CommandName = *std::string::Printf(TEXT("%s_%s"), *Subsystem, *Command);

	// wake up UE
	FEmbeddedCommunication::KeepAwake(CommandName, false);

#if !UE_BUILD_SHIPPING
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("NativeToEmbeddedCall: Subsystem: %s, Command: %s, Params:"), *Subsystem, *Helper.Command);
#endif

	if (InParams != nullptr)
	{
		int32_t Count = jenv->GetArrayLength(InParams);
		int32_t Index = 0;
		while (Index < Count)
		{
			auto javaKey = FJavaHelper::FStringFromLocalRef(jenv, (jstring)(jenv->GetObjectArrayElement(InParams, Index++)));
			auto javaValue = FJavaHelper::FStringFromLocalRef(jenv, (jstring)(jenv->GetObjectArrayElement(InParams, Index++)));

			Helper.Parameters.Add(javaKey, javaValue);
#if !UE_BUILD_SHIPPING
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("  %s : %s"), *javaKey, *javaValue);
#endif
		}
	}

	Helper.OnCompleteDelegate = [CommandName, ID, RoutingFunction](const FEmbeddedCommunicationMap& InReturnValues, std::string InError)
	{
		// wake up UE
		FEmbeddedCommunication::AllowSleep(CommandName);

		AndroidThunkCpp_OnNativeToEmbeddedReply(ID, InReturnValues, InError, RoutingFunction);
	};

	FEmbeddedCommunication::RunOnGameThread(Priority, [SubsystemName, Helper]()
	{
		if (FEmbeddedDelegates::GetNativeToEmbeddedParamsDelegateForSubsystem(SubsystemName).IsBound())
		{
			FEmbeddedDelegates::GetNativeToEmbeddedParamsDelegateForSubsystem(SubsystemName).Broadcast(Helper);
		}
		else
		{
			Helper.OnCompleteDelegate({} , std::string::Printf(TEXT("No one is listening to subsystem %s"), *SubsystemName.ToString()));
		}
	});
#endif // BUILD_EMBEDDED_APP
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_SetNamedObject(JNIEnv* jenv, jobject thiz, jstring InName, jobject InObj)
{
    auto Name = FJavaHelper::FStringFromParam(jenv, InName);
   // FEmbeddedDelegates::SetNamedObject(Name, (void*)InObj);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_KeepAwake(JNIEnv* jenv, jobject thiz, jstring InRequester, jboolean bIsForRendering)
{
    auto Requester = FJavaHelper::FStringFromParam(jenv, InRequester);
    //FEmbeddedCommunication::KeepAwake(*Requester, bIsForRendering);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_AllowSleep(JNIEnv* jenv, jobject thiz, jstring InRequester)
{
    auto Requester = FJavaHelper::FStringFromParam(jenv, InRequester);
    //FEmbeddedCommunication::AllowSleep(*Requester);
}

#if !BUILD_EMBEDDED_APP
//DEFINE_LOG_CATEGORY_STATIC(LogJava, Log, All);
#endif

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_UELogError(JNIEnv* jenv, jobject thiz, jstring InString)
{
    const auto chars = jenv->GetStringUTFChars(InString, 0);
#if BUILD_EMBEDDED_APP
    FEmbeddedCommunication::UELogError(UTF8_TO_TCHAR(chars));
#else
//    if (GLog && UE_LOG_ACTIVE(LogJava, Error))
//    {
//        GLog->Log("LogJava", ELogVerbosity::Error, UTF8_TO_TCHAR(chars));
//    }
#endif
    jenv->ReleaseStringUTFChars(InString, chars);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_UELogWarning(JNIEnv* jenv, jobject thiz, jstring InString)
{
    const auto chars = jenv->GetStringUTFChars(InString, 0);
#if BUILD_EMBEDDED_APP
    FEmbeddedCommunication::UELogWarning(UTF8_TO_TCHAR(chars));
#else
//    if (GLog && UE_LOG_ACTIVE(LogJava, Warning))
//    {
//        GLog->Log("LogJava", ELogVerbosity::Warning, UTF8_TO_TCHAR(chars));
//    }
#endif
    jenv->ReleaseStringUTFChars(InString, chars);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_UELogLog(JNIEnv* jenv, jobject thiz, jstring InString)
{
    const auto chars = jenv->GetStringUTFChars(InString, 0);
#if BUILD_EMBEDDED_APP
    FEmbeddedCommunication::UELogLog(UTF8_TO_TCHAR(chars));
#else
//    if (GLog && UE_LOG_ACTIVE(LogJava, Log))
//    {
//        GLog->Log("LogJava", ELogVerbosity::Log, UTF8_TO_TCHAR(chars));
//    }
#endif
    jenv->ReleaseStringUTFChars(InString, chars);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_UELogVerbose(JNIEnv* jenv, jobject thiz, jstring InString)
{
    const auto chars = jenv->GetStringUTFChars(InString, 0);
#if BUILD_EMBEDDED_APP
    FEmbeddedCommunication::UELogVerbose(UTF8_TO_TCHAR(chars));
#else
//    if (GLog && UE_LOG_ACTIVE(LogJava, Verbose))
//    {
//        GLog->Log("LogJava", ELogVerbosity::Verbose, UTF8_TO_TCHAR(chars));
//    }
#endif
    jenv->ReleaseStringUTFChars(InString, chars);
}

void FJavaWrapper::SetupEmbeddedCommunication(JNIEnv* Env)
{
#if BUILD_EMBEDDED_APP
    ReplyMethod = FJavaWrapper::FindMethod(Env, GGameActivityClassID, "AndroidThunkJava_OnNativeToEmbeddedReply", "(Ljava/lang/Object;[Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);

	FEmbeddedDelegates::GetEmbeddedToNativeParamsDelegateForSubsystem(TEXT("webview")).AddLambda([](const FEmbeddedCallParamsHelper& InMessage)
	{
		UE_LOG(LogAndroid, Display, TEXT("Calling out from embedded to native for 'webview' subsystem:"));

		AndroidThunkCpp_OnNativeToEmbeddedReply(InMessage.Command, InMessage.Parameters, TEXT(""), TEXT("webviewnotification"));

		// @todo: Do we need to have results passed back? if so we will need a wrapper block that we pass in to all of the above handling things to call back
		// same for ios
		if (InMessage.OnCompleteDelegate != nullptr)
		{
			InMessage.OnCompleteDelegate(FEmbeddedCommunicationMap(), TEXT(""));
		}
	});

	FEmbeddedDelegates::GetEmbeddedToNativeParamsDelegateForSubsystem(TEXT("native")).AddLambda([](const FEmbeddedCallParamsHelper& InMessage)
	{
		UE_LOG(LogAndroid, Display, TEXT("Calling out from embedded to native for 'native' subsystem:"));

		AndroidThunkCpp_OnNativeToEmbeddedReply(InMessage.Command, InMessage.Parameters, TEXT(""), TEXT("native"));

		// @todo: Do we need to have results passed back? if so we will need a wrapper block that we pass in to all of the above handling things to call back
		// same for ios
		if (InMessage.OnCompleteDelegate != nullptr)
		{
			InMessage.OnCompleteDelegate(FEmbeddedCommunicationMap(), TEXT(""));
		}
	});
#endif
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_ForwardNotification(JNIEnv* jenv, jobject thiz, jstring payload)
{
    //
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCalls_RouteServiceIntent(JNIEnv* jenv, jobject thiz, jstring InAction, jstring InPayload)
{
    // call to OnSafetyNetAttestationResultDelegate on game thread
//    if (FTaskGraphInterface::IsRunning())
//    {
//        const char *nativeAction = jenv->GetStringUTFChars(InAction, 0);
//        std::string Action = std::string(nativeAction);
//        jenv->ReleaseStringUTFChars(InAction, nativeAction);
//
//        const char *nativePayload = jenv->GetStringUTFChars(InPayload, 0);
//        std::string Payload = std::string(nativePayload);
//        jenv->ReleaseStringUTFChars(InPayload, nativePayload);
//
//        FGraphEventRef RouteServiceIntentTask = FFunctionGraphTask::CreateAndDispatchWhenReady([Action, Payload]()
//                                                                                               {
//                                                                                                   FJavaWrapper::OnRouteServiceIntentDelegate.Broadcast(Action, Payload);
//                                                                                               }, TStatId(), NULL, ENamedThreads::GameThread);
//    }
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeOnThermalStatusChangedListener(JNIEnv* jenv, jobject thiz, jint Status)
{
//    FAndroidStats::OnThermalStatusChanged(Status);
//
//    FCoreDelegates::ETemperatureSeverity Severity;
//    switch (Status)
//    {
//        case 0:
//        case 1:
//            Severity = FCoreDelegates::ETemperatureSeverity::Good;
//            break;
//
//        case 2:
//            Severity = FCoreDelegates::ETemperatureSeverity::Bad;
//            break;
//
//        case 3:
//        case 4:
//            Severity = FCoreDelegates::ETemperatureSeverity::Serious;
//            break;
//
//        case 5:
//        case 6:
//            Severity = FCoreDelegates::ETemperatureSeverity::Critical;
//            break;
//
//        default:
//            Severity = FCoreDelegates::ETemperatureSeverity::Unknown;
//            break;
//    }
//
//    FCoreDelegates::OnTemperatureChange.Broadcast(Severity);
}

static void OnTrimMessage(int MemoryTrimValue)
{
    //FAndroidPlatformMemory::UpdateOSMemoryStatus(FAndroidPlatformMemory::EOSMemoryStatusCategory::OSTrim, MemoryTrimValue);
   // FAndroidStats::OnTrimMemory(MemoryTrimValue);
}

#if !UE_BUILD_SHIPPING
//FAutoConsoleCommand TestTrimMessage(
//        TEXT("android.TestTrimMessage"),
//        TEXT("testing only, android.TestTrimMessage int \n")
//TEXT("int value must match expected values from android OS (see ComponentCallbacks2 api)\n")
//TEXT("eg. android.TestTrimMessage 15")
//,
//FConsoleCommandWithArgsDelegate::CreateLambda([](const std::vector<std::string>& Args)
//{
//if(Args.size() > 0)
//{
//uint64 MemoryTrimValue = 0;
//LexFromString(MemoryTrimValue, *Args[0]);
//if(MemoryTrimValue)
//{
//OnTrimMessage(MemoryTrimValue);
//}
//}
//}));
#endif

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_nativeOnTrimMemory(JNIEnv* jenv, jobject thiz, jint MemoryTrimValue)
{
    OnTrimMessage(MemoryTrimValue);
}


//class FAndroidEmbeddedExec : public FSelfRegisteringExec
//{
//public:
//    FAndroidEmbeddedExec()
//            : FSelfRegisteringExec()
//    {
//
//    }
//
//    virtual bool Exec(UWorld* Inworld, const TCHAR* Cmd, FOutputDevice& Ar) override
//    {
//        if (FParse::Command(&Cmd, TEXT("Android")))
//        {
//            // commands to override and append commandline options for next boot (see FIOSCommandLineHelper)
//            if (FParse::Command(&Cmd, TEXT("HideWeb")))
//            {
//                AndroidThunkCpp_OnNativeToEmbeddedReply(TEXT("HideWeb"), {}, TEXT(""), TEXT("native"));
//            }
//            else if (FParse::Command(&Cmd, TEXT("ShowWeb")))
//            {
//                AndroidThunkCpp_OnNativeToEmbeddedReply(TEXT("ShowWeb"), {}, TEXT(""), TEXT("native"));
//            }
//        }
//
//        return false;
//    }
//} GAndroidEmbeddedExec;
//
//

#endif
}
//
// Created by DXT00 on 2025/4/11.
//
#pragma once

#include "BlackPearl/Application.h"

#if USE_ANDROID_JNI
#include "BlackPearl/Core/Android/AndroidJavaEnv.h"
#endif

namespace BlackPearl
{
    namespace FAndroidAppEntry
    {
        void PlatformInit();

        // if the native window handle has changed then the new handle is required.
        void ReInitWindow(void* NewNativeWindowHandle = nullptr);

        void ReleaseEGL();
        void OnPauseEvent();
    }

    class AndroidApplication : public Application
    {
    public:
#if USE_ANDROID_JNI
        // Returns the java environment
        static FORCEINLINE void InitializeJavaEnv(JavaVM* VM, jint Version, jobject GlobalThis)
        {
            AndroidJavaEnv::InitializeJavaEnv(VM, Version, GlobalThis);
        }
        static FORCEINLINE jobject GetGameActivityThis()
        {
            return AndroidJavaEnv::GetGameActivityThis();
        }
        static FORCEINLINE jobject GetClassLoader()
        {
            return AndroidJavaEnv::GetClassLoader();
        }
        static FORCEINLINE JNIEnv* GetJavaEnv(bool bRequireGlobalThis = true)
        {
            return AndroidJavaEnv::GetJavaEnv(bRequireGlobalThis);
        }
        static FORCEINLINE jclass FindJavaClass(const char* name)
        {
            return AndroidJavaEnv::FindJavaClass(name);
        }
        static FORCEINLINE jclass FindJavaClassGlobalRef(const char* name)
        {
            return AndroidJavaEnv::FindJavaClassGlobalRef(name);
        }
        static FORCEINLINE void DetachJavaEnv()
        {
            AndroidJavaEnv::DetachJavaEnv();
        }
        static FORCEINLINE bool CheckJavaException()
        {
            return AndroidJavaEnv::CheckJavaException();
        }

        static FORCEINLINE void OnWindowSizeChanged(){
           // bWindowSizeChanged = true;

        }
#endif

    };
}

#pragma once
//#include "pch.h"

#include "BlackPearl/Application.h"
#ifdef GE_PLATFORM_ANDROID
#include "BlackPearl/Log.h"
#include "BlackPearl/Luanch/Android/LuanchAndroid.h"
#include <android_native_app_glue.h>
#include "Core/CriticalSection.h"
#include "Core/PlatformMisc.h"
#include "Core/PlatformProc.h"
#endif

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>


#ifdef GE_PLATFORM_WINDOWS
#include <crtdbg.h>
#include <windows.h>

extern BlackPearl::Application* BlackPearl::CreateApplication(INSTANCE_HANDLE hInstance, int nShowCmd);

#ifdef GE_API_D3D12
int WINAPI WinMain(_In_ INSTANCE_HANDLE hInstance, _In_opt_ INSTANCE_HANDLE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	std::wstringstream wstr;
	wstr << L"entry\n";
	OutputDebugStringW(wstr.str().c_str());
#else
int main(_In_ INSTANCE_HANDLE hInstance, _In_opt_ INSTANCE_HANDLE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
#endif


	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//BlackPearl::Log::Init();
	//GE_CORE_WARN("Initialized Log!");
	BlackPearl::Application* app = BlackPearl::CreateApplication(hInstance, nShowCmd);
    app->Init();
    app->Run();
	delete app;

	_CrtDumpMemoryLeaks();
	return 0;
}
#elif defined GE_PLATFORM_ANDROID
#include "jni.h"

namespace BlackPearl{
    extern FCriticalSection GAndroidWindowLock;
    extern volatile bool  GWindowInit;

}

extern "C"{

//void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
//    // 手动初始化 android_app 结构体
//    struct android_app* app = new android_app();
//    memset(app, 0, sizeof(struct android_app));
//
//    app->activity = activity;
//    app->savedState = savedState;
//    app->savedStateSize = savedStateSize;
//    pthread_mutex_init(&app->mutex, nullptr);
//    pthread_cond_init(&app->cond, nullptr);
//
//    // 调用主函数
//    android_main(app);
//}
void android_main(struct android_app* state)
{
    LOGI("[dxt00] in android main!!");
    std::string mainThreadName = "BP_GameThread";
    pthread_setname_np(pthread_self(),mainThreadName.c_str() );

    // Make sure glue isn't stripped. (not needed in ndk-15)
#if PLATFORM_ANDROID_NDK_VERSION < 150000
    //app_dummy();
#endif
    BlackPearl::Application* app = BlackPearl::CreateApplication(0, 0, state);
    state->userData = &app;
    BlackPearl::LuanchAndroid::InitAndriodThread(state);
    BlackPearl::LuanchAndroid::InitJavaEnv(state);






    GE_CORE_INFO("Wait for GAndroidWindowLock.Lock()");
        // wait for a valid window
        // Lock GAndroidWindowLock to ensure no window destroy shenanigans occur between early phase of preinit and UnlockAndroidWindow
        // Note: this is unlocked after Android's PlatformCreateDynamicRHI when the RHI is then able to process window changes.
        // We don't wait for all of preinit to complete as PreLoadScreens will need to process events during preinit.

    while (!BlackPearl::GWindowInit)
    {
        BlackPearl::FPlatformProc::Sleep(0.01f);
        BlackPearl::FPlatformMisc::MemoryBarrier();
    }


    GE_CORE_INFO("PreInit android HW window lock.");
    BlackPearl::GAndroidWindowLock.Lock();



    app->Init();
    //TODO:: 减小锁粒度
    BlackPearl::GAndroidWindowLock.Unlock();

    app->Run();
    delete app;

    //@todo android: replace with native activity, main loop off of UI thread, etc.
    //AndroidMain(state);
}
};

#endif



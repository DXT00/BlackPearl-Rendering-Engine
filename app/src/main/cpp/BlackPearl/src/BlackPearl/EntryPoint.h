#pragma once
//#include "pch.h"

#include "BlackPearl/Application.h"
#ifdef GE_PLATFORM_ANDROID
#include "BlackPearl/Luanch/Android/LuanchAndroid.h"
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
	app->Run();
	delete app;

	_CrtDumpMemoryLeaks();
	return 0;
}
#elif defined GE_PLATFORM_ANDROID


void android_main(struct android_app* state)
{
    BlackPearl::LuanchAndroid::InitAndriodThread(state);
    // Make sure glue isn't stripped. (not needed in ndk-15)
#if PLATFORM_ANDROID_NDK_VERSION < 150000
    //app_dummy();
#endif
    BlackPearl::Application* app = BlackPearl::CreateApplication(0, 0, state);
    app->Run();
    delete app;

    //@todo android: replace with native activity, main loop off of UI thread, etc.
    //AndroidMain(state);
}


#endif



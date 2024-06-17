#pragma once
//#include "pch.h"

#include "BlackPearl/Application.h"


#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <windows.h>


#ifdef GE_PLATFORM_WINDOWS

extern BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd);

#ifdef GE_API_D3D12
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	std::wstringstream wstr;
	wstr << L"entry\n";
	OutputDebugStringW(wstr.str().c_str());
#else
int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
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
#endif



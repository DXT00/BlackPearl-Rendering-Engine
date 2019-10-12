#pragma once
//#include "pch.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Log.h"
#ifdef GE_PLATFORM_WINDOWS
extern BlackPearl::Application* BlackPearl::CreateApplication();
int main() {

	BlackPearl::Log::Init();
	GE_CORE_WARN("Initialized Log!");
	BlackPearl::Application* app = BlackPearl::CreateApplication();
	app->Run();




	delete app;
	

}
#endif



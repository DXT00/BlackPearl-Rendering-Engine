#include"pch.h"
#include"Log.h"
#include "Application.h"


int main() {

	Log::Init();
	GE_CORE_WARN("Initialized Log!");
	Application* app = new Application();
	app->Run();
	

}




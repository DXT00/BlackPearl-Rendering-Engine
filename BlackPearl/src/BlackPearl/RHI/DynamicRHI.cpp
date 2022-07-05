#include "pch.h"
#include "BlackPearl/Core.h"
#include "DynamicRHI.h"
#include "D3D12RHI/D3D12DynamicModule.h"
#include "OpenGLRHI/OpenGLDynamicModule.h"

namespace BlackPearl {
	//global RHI
	DynamicRHI* g_DynamicRHI = nullptr;

#ifdef D3D12RHI
	DynamicRHI::Type DynamicRHI::g_Type = DynamicRHI::Type::D3D12;
#else
	DynamicRHI::Type DynamicRHI::g_Type = DynamicRHI::Type::OpenGL;
#endif // D3D12RHI


	void DynamicRHIInit()
	{
		DynamicModule* dynamicModule;
		if (DynamicRHI::g_Type == DynamicRHI::Type::OpenGL) {
			dynamicModule = DBG_NEW OpenGLDynamicModule();
		}
		else if (DynamicRHI::g_Type == DynamicRHI::Type::D3D12) {
			dynamicModule = DBG_NEW D3D12DynamicModule();
		}
		g_DynamicRHI = dynamicModule->CreateRHI();
	}

	Window* RHIInitWindow()
	{
		return g_DynamicRHI->InitWindow();
	}

}

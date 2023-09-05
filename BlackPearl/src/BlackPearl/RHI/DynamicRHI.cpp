#include "pch.h"
#include "BlackPearl/Core.h"
#include "DynamicRHI.h"
#include "D3D12RHI/D3D12DynamicModule.h"
#include "OpenGLRHI/OpenGLDynamicModule.h"
#include "VulkanRHI/VkDynamicModule.h"


namespace BlackPearl {

	//global RHI
	DynamicRHI* g_DynamicRHI = nullptr;
	DynamicRHI::Type g_RHIType;
	void DynamicRHIInit(DynamicRHI::Type rhiType)
	{
		g_RHIType = rhiType;
		DynamicModule* dynamicModule;
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			dynamicModule = DBG_NEW OpenGLDynamicModule();
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			dynamicModule = DBG_NEW D3D12DynamicModule();
		}
		else if (g_RHIType == DynamicRHI::Type::Vulkan) {
			dynamicModule = DBG_NEW VkDynamicModule();
		}
		g_DynamicRHI = dynamicModule->CreateRHI();
	}

	Window* RHIInitWindow()
	{
		return g_DynamicRHI->InitWindow();
	}
	//TODO:添加Math库，区分glm与DirectXMath
	void RHIInitMathLib() {
		g_DynamicRHI->InitMathLib();
	}
	/*void RHIInitLogger() {
		g_DynamicRHI->InitLogger();
	}*/

	void RHIEngineExit() {
		g_DynamicRHI->EngineExit();
	}
}

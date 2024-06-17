#include "pch.h"
#include "BlackPearl/Core.h"
#include "DynamicRHI.h"
#include "D3D12RHI/D3D12DynamicModule.h"
#include "OpenGLRHI/OpenGLDynamicModule.h"
#include "VulkanRHI/VkDynamicModule.h"


namespace BlackPearl {

	//global RHI
	DynamicRHI* g_DynamicRHI = nullptr;
	DynamicRHI::Type DynamicRHI::g_RHIType = DynamicRHI::Type::Vulkan;
	void DynamicRHIInit(DynamicRHI::Type rhiType)
	{
		DynamicRHI::g_RHIType = rhiType;
		DynamicModule* dynamicModule;
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
#ifdef GE_API_OPENGL
            dynamicModule = DBG_NEW OpenGLDynamicModule();
#endif
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
			dynamicModule = DBG_NEW D3D12DynamicModule();
#endif
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
#ifdef GE_API_VULKAN
			dynamicModule = DBG_NEW VkDynamicModule();
#endif
		}
		g_DynamicRHI = dynamicModule->CreateRHI();
	}

	Window* RHIInitWindow()
	{
		return g_DynamicRHI->InitWindow();
	}
	//TODO:���Math�⣬����glm��DirectXMath
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

#include "pch.h"
#include "BlackPearl/Core.h"
#include "DynamicRHI.h"
#ifdef GE_API_D3D12
#include "D3D12RHI/D3D12DynamicModule.h"
#endif
#ifdef GE_API_OPENGL
#include "OpenGLRHI/OpenGLDynamicModule.h"
#endif

#ifdef GE_API_VULKAN
#include "VulkanRHI/VkDynamicModule.h"
#endif

#include "RHIShader.h"
#include "RHIGlobals.h"
#include "RHIDefinitions.h"

namespace BlackPearl {

	//global RHI
	DynamicRHI* g_DynamicRHI = nullptr;
#if GE_API_D3D12
	DynamicRHI::Type DynamicRHI::g_RHIType = DynamicRHI::Type::D3D12;

#elif GE_API_VULKAN
	DynamicRHI::Type DynamicRHI::g_RHIType = DynamicRHI::Type::Vulkan;

#else
	DynamicRHI::Type DynamicRHI::g_RHIType = DynamicRHI::Type::OpenGL;

#endif
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
		g_DynamicRHI->InitRHI();
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



	EShaderPlatform GMaxRHIShaderPlatform = SP_PCD3D_SM5;

	/** The maximum feature level supported on this machine */
	ERHIFeatureLevel::Type GMaxRHIFeatureLevel = ERHIFeatureLevel::SM5;

	bool IsRHIDeviceAMD()
	{
		assert(GRHIVendorId != 0);
		return GRHIVendorId == 0x1002;
	}

	bool IsRHIDeviceIntel()
	{
		assert(GRHIVendorId != 0);
		return GRHIVendorId == 0x8086;
	}

	bool IsRHIDeviceNVIDIA()
	{
		assert(GRHIVendorId != 0);
		return GRHIVendorId == 0x10DE;
	}

	bool IsRHIDeviceApple()
	{
		assert(GRHIVendorId != 0);
		return GRHIVendorId == (uint32_t)EGpuVendorId::Apple;
	}



}

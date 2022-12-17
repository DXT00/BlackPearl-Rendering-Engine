#pragma once
#include "BlackPearl/Window.h"
namespace BlackPearl{
	class Window;
	class DynamicModule;

	enum class RHIType {
		D3D12,
		OpenGL,
		Vulkan
	};

	//void    RHIInitLogger();
	Window* RHIInitWindow();
	void RHIEngineExit();

	class DynamicRHI
	{
	public:
		enum class Type {
			D3D12,
			OpenGL,
			Vulkan
		};
		
		virtual Window* InitWindow() = 0;
		//virtual void InitLogger() = 0;
		//virtual std::shared_ptr<Logger> GetCoreLogger() = 0;
		virtual void InitMathLib() {};
		virtual void EngineExit() {};
	};

	//DynamicRHI::Type g_RHIType;
	void DynamicRHIInit(DynamicRHI::Type rhiType);
}
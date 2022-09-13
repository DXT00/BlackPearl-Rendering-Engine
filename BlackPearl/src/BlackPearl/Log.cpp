#include"pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace BlackPearl {


	Log::Log()
	{
		if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger = std::make_shared<D3D12Logger>();
		}
		else if(g_RHIType == DynamicRHI::Type::OpenGL){
			m_OpenGLCoreLogger = spdlog::stdout_color_mt("Color");
			spdlog::set_pattern("%^[%T] %n:%v%$");
			m_OpenGLCoreLogger->set_level(spdlog::level::trace);
		}
	}



	

}
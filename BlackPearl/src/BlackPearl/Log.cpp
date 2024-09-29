#include"pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace BlackPearl {


	Log::Log()
	{
#ifdef	GE_API_D3D12
			mLogger = std::make_shared<D3D12Logger>();
		
		/** opengl and vulkan use the same logger: m_OpenGLCoreLogger */
#else
			mLogger = spdlog::stdout_color_mt("Color");
			spdlog::set_pattern("%^[%T] %n:%v%$");
			mLogger->set_level(spdlog::level::trace);
#endif 
	}



	

}
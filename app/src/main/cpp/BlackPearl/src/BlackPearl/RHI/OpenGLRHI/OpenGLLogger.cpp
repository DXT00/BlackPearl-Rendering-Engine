#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLLogger.h"


#define SPDLOG_HEADER_ONLY
#define SPDLOG_INLINE inline
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace BlackPearl {
	//std::shared_ptr<spdlog::logger> OpenGLLogger::s_pCoreLoggerSingle = nullptr;
    OpenGLLogger::OpenGLLogger(){



            spdlog::set_pattern("%^[%T] %n:%v%$");
            m_pCoreLoggerSingle = spdlog::stdout_color_mt("Color");
            m_pCoreLoggerSingle->set_level(spdlog::level::trace);


    }

}

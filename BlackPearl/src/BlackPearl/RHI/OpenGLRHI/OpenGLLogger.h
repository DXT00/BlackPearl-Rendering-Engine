#pragma once
#include "BlackPearl/Log.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace BlackPearl {
	class OpenGLLogger 
	{
	public:
		OpenGLLogger() {
			spdlog::set_pattern("%^[%T] %n:%v%$");
			m_pCoreLoggerSingle = spdlog::stdout_color_mt("Color");
			m_pCoreLoggerSingle->set_level(spdlog::level::trace);
		}
		virtual void trace(const char* fmt, const char* msg) {};
		virtual void info(const char* fmt, const char* msg) {};
		virtual void warn(const char* fmt, const char* msg) {};
		virtual void error(const char* fmt, const char* msg) {};
		virtual void fatal(const char* fmt, const char* msg) {};
		/*std::shared_ptr<spdlog::logger> GetCoreLogger() {
			return m_pCoreLoggerSingle;
		}*/
	private:
		std::shared_ptr<spdlog::logger> m_pCoreLoggerSingle;
	};

}


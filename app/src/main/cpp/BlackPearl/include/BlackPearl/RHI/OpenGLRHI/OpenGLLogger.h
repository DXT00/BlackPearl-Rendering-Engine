#pragma once
namespace spdlog{
    class logger;
}
namespace BlackPearl {
	class OpenGLLogger
	{
	public:
		OpenGLLogger();
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


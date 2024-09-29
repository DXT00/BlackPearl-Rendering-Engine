#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#ifdef	GE_API_D3D12
#include "BlackPearl/RHI/D3D12RHI/D3D12Logger.h"
#endif
namespace BlackPearl {
	extern class Log* g_Log;
	//extern DynamicRHI::Type g_RHIType;

	//TODO::通过类区分OpenGL和DirectX,由于有模板，难以使用继承，还需考虑其他方法
	class Log
	{
	public:
		Log();
		
		template<typename... Args>
		void Trace(const char* fmt, const Args &... args);
		template<typename... Args>
		void Info(const char* fmt, const Args &... args);
		template<typename... Args>
		void Warn(const char* fmt, const Args &... args);
		template<typename... Args>
		void Error(const char* fmt, const Args &... args);
		template<typename... Args>
		void Fatal(const char* fmt, const Args &... args);

		template<typename... Args>
		void Trace(const Args &... args);
		template<typename... Args>
		void Info(const Args &... args);
		template<typename... Args>
		void Warn(const Args &... args);
		template<typename... Args>
		void Error(const Args &... args);
		template<typename... Args>
		void Fatal(const Args &... args);

		//inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
	/*private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;*/
	private:
#ifdef	GE_API_D3D12
		using LoggerPtr = std::shared_ptr<D3D12Logger>;

#else
		using LoggerPtr = std::shared_ptr<spdlog::logger>;

#endif 
		LoggerPtr mLogger;
	};

	template<typename... Args>
	void Log::Trace(const char* fmt, const Args &... args)
	{
		mLogger->trace(fmt, args...);

	}

	template<typename... Args>
	void Log::Info(const char* fmt, const Args &... args)
	{
		mLogger->info(fmt, args...);
	}

	template<typename... Args>
	void Log::Warn(const char* fmt, const Args &... args)
	{
		mLogger->warn(fmt, args...);
	}

	template<typename... Args>
	void Log::Error(const char* fmt, const Args &... args)
	{
		mLogger->error(fmt, args...);
	}

	template<typename... Args>
	void Log::Fatal(const char* fmt, const Args &... args)
	{
		mLogger->fatal(fmt, args...);
	}

	template<typename ...Args>
	void Log::Trace(const Args & ...args)
	{
		mLogger->fatal(fmt, args...);
	}

	template<typename ...Args>
	void Log::Info(const Args & ...args)
	{
		mLogger->info(args...);
	}

	template<typename ...Args>
	void Log::Warn(const Args & ...args)
	{
		mLogger->warn(args...);
	}

	template<typename ...Args>
	void Log::Error(const Args & ...args)
	{
		mLogger->error(args...);
	}

	template<typename ...Args>
	void Log::Fatal(const Args & ...args)
	{
		mLogger->fatal(args...);
	}
}

//Core log macros
#define GE_CORE_TRACE(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Trace(__VA_ARGS__);}
#define GE_CORE_INFO(...)  { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Info(__VA_ARGS__);}
#define GE_CORE_WARN(...)  { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Warn(__VA_ARGS__);}
#define GE_CORE_ERROR(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Error(__VA_ARGS__);__debugbreak();}
#define GE_CORE_FATAL(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Fatal(__VA_ARGS__);}


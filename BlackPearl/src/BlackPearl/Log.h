#pragma once
#include "BlackPearl/RHI/D3D12RHI/D3D12Logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "BlackPearl/RHI/DynamicRHI.h"

namespace BlackPearl {
	extern class Log* g_Log;
	extern DynamicRHI::Type g_RHIType;

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

		std::shared_ptr<spdlog::logger> m_OpenGLCoreLogger;
		std::shared_ptr<D3D12Logger> m_D3D12CoreLogger;
	};

	template<typename... Args>
	void Log::Trace(const char* fmt, const Args &... args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->trace(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->trace(fmt, args...);
		}
	}

	template<typename... Args>
	void Log::Info(const char* fmt, const Args &... args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->info(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->info(fmt, args...);
		}
	}

	template<typename... Args>
	void Log::Warn(const char* fmt, const Args &... args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->warn(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->warn(fmt, args...);
		}

	}

	template<typename... Args>
	void Log::Error(const char* fmt, const Args &... args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->error(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->error(fmt, args...);
		}

	}

	template<typename... Args>
	void Log::Fatal(const char* fmt, const Args &... args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->fatal(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->fatal(fmt, args...);
		}

	}

	template<typename ...Args>
	void Log::Trace(const Args & ...args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->fatal(fmt, args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->fatal(fmt, args...);
		}
	}

	template<typename ...Args>
	void Log::Info(const Args & ...args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->info(args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->info(args...);
		}
	}

	template<typename ...Args>
	void Log::Warn(const Args & ...args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->warn(args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->warn(args...);
		}
	}

	template<typename ...Args>
	void Log::Error(const Args & ...args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->error(args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->error(args...);
		}
	}

	template<typename ...Args>
	void Log::Fatal(const Args & ...args)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_OpenGLCoreLogger->fatal(args...);
		}
		else if (g_RHIType == DynamicRHI::Type::D3D12) {
			m_D3D12CoreLogger->fatal(args...);
		}
	}
}

//Core log macros
#define GE_CORE_TRACE(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Trace(__VA_ARGS__);}
#define GE_CORE_INFO(...)  { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Info(__VA_ARGS__);}
#define GE_CORE_WARN(...)  { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Warn(__VA_ARGS__);}
#define GE_CORE_ERROR(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Error(__VA_ARGS__);__debugbreak();}
#define GE_CORE_FATAL(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Fatal(__VA_ARGS__);}


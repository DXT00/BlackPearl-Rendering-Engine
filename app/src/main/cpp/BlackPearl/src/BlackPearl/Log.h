#pragma once
#include "BlackPearl/RHI/D3D12RHI/D3D12Logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "BlackPearl/RHI/DynamicRHI.h"


namespace BlackPearl {
	extern class Log* g_Log;
	//extern DynamicRHI::Type g_RHIType;

	//TODO::ͨ��������OpenGL��DirectX,������ģ�壬����ʹ�ü̳У����迼����������
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
        //TODO:: 统一成一个
#ifdef GE_API_D3D12
		std::shared_ptr<D3D12Logger> m_D3D12CoreLogger;
#endif
	};

	template<typename... Args>
	void Log::Trace(const char* fmt, const Args &... args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->trace(fmt, args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->trace(fmt, args...);
#endif
		}
	}

	template<typename... Args>
	void Log::Info(const char* fmt, const Args &... args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->info(fmt, args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
			m_D3D12CoreLogger->info(fmt, args...);
#endif

        }
	}

	template<typename... Args>
	void Log::Warn(const char* fmt, const Args &... args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->warn(fmt, args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
			m_D3D12CoreLogger->warn(fmt, args...);
#endif

		}

	}

	template<typename... Args>
	void Log::Error(const char* fmt, const Args &... args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->error(fmt, args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->error(fmt, args...);
#endif

		}

	}

	template<typename... Args>
	void Log::Fatal(const char* fmt, const Args &... args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->critical(fmt, args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->fatal(fmt, args...);
#endif

		}

	}

	template<typename ...Args>
	void Log::Trace(const Args & ...args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->critical(args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->fatal(args...);
#endif

		}
	}

	template<typename ...Args>
	void Log::Info(const Args & ...args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->info(args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->info(args...);
#endif

		}
	}

	template<typename ...Args>
	void Log::Warn(const Args & ...args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->warn(args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->warn(args...);
#endif

		}
	}

	template<typename ...Args>
	void Log::Error(const Args & ...args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->error(args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->error(args...);
#endif

		}
	}

	template<typename ...Args>
	void Log::Fatal(const Args & ...args)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL || DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
			m_OpenGLCoreLogger->critical(args...);
		}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
#ifdef GE_API_D3D12
            m_D3D12CoreLogger->fatal(args...);
#endif
		}
	}
}
extern class BlackPearl::Log* BlackPearl::g_Log;
//Core log macros
#define GE_CORE_TRACE(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Trace(__VA_ARGS__);}
#define GE_CORE_INFO(...)  { BlackPearl::g_Log->Info(__VA_ARGS__);}
#define GE_CORE_WARN(...)  { BlackPearl::g_Log->Warn(__VA_ARGS__);}
#ifdef GE_PLATFORM_WINDOWS
#define GE_CORE_ERROR(...) { BlackPearl::g_Log->Error(__VA_ARGS__);__debugbreak();}
#else
#define GE_CORE_ERROR(...) { BlackPearl::g_Log->Error(__VA_ARGS__);}
#endif
#define GE_CORE_FATAL(...) { extern class BlackPearl::Log* BlackPearl::g_Log; BlackPearl::g_Log->Fatal(__VA_ARGS__);}


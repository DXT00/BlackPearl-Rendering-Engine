#pragma once
#ifndef BP_LOG_H
#define BP_LOG_H
#include "spdlog/spdlog.h"

#ifdef	GE_API_D3D12
#include "BlackPearl/RHI/D3D12RHI/D3D12Logger.h"
#endif


#ifdef GE_PLATFORM_ANDROID
#include <android/log.h>
#define LOG_TAG "Native-BlackPearl"

#define _LOG(priority, fmt, ...) \
  ((void)__android_log_print((priority), (LOG_TAG), (fmt)__VA_OPT__(, ) __VA_ARGS__));

#define LOGE(fmt, ...) _LOG(ANDROID_LOG_ERROR, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#define LOGW(fmt, ...) _LOG(ANDROID_LOG_WARN, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#define LOGI(fmt, ...) _LOG(ANDROID_LOG_INFO, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#else
#define LOGW(fmt, ...) _LOG(ANDROID_LOG_WARN, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#define LOGI(fmt, ...) _LOG(ANDROID_LOG_INFO, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#endif



namespace spdlog{
    class logger;
}
namespace BlackPearl {
	extern class Log* g_Log;
	//extern DynamicRHI::Type g_RHIType;

	//TODO::ͨ��������OpenGL��DirectX,������ģ�壬����ʹ�ü̳У����迼����������
	class Log
	{
	public:
		Log();
		
//		template<typename... Args>
//		inline void Trace(const char* fmt, const Args &... args);
//		template<typename... Args>
//        inline void Info(const char* fmt, const Args &... args);
//		template<typename... Args>
//        inline void Warn(const char* fmt, const Args &... args);
//		template<typename... Args>
//        inline void Error(const char* fmt, const Args &... args);
//		template<typename... Args>
//        inline void Fatal(const char* fmt, const Args &... args);
//
//		template<typename... Args>
//        inline void Trace(const Args &... args);
//		template<typename... Args>
//        inline void Info(const Args &... args);
//		template<typename... Args>
//        inline void Warn(const Args &... args);
//		template<typename... Args>
//        inline void Error(const Args &... args);
//		template<typename... Args>
//        inline void Fatal(const Args &... args);

        template<typename... Args>
        inline void Trace(const char* fmt, const Args &... args)
        {
            mLogger->trace(fmt, args...);

        }

        template<typename... Args>
        inline void Info(const char* fmt, const Args &... args)
        {
            mLogger->info(fmt, args...);
        }

        template<typename... Args>
        inline void Warn(const char* fmt, const Args &... args)
        {
            mLogger->warn(fmt, args...);
        }

        template<typename... Args>
        inline void Error(const char* fmt, const Args &... args)
        {
            mLogger->error(fmt, args...);
        }

        template<typename... Args>
        inline void Fatal(const char* fmt, const Args &... args)
        {
            mLogger->error(fmt, args...);
        }

        template<typename ...Args>
        inline void Trace(const Args & ...args)
        {
            mLogger->error(args...);
        }

        template<typename ...Args>
        inline void Info(const Args & ...args)
        {
            mLogger->info(args...);
        }

        template<typename ...Args>
        inline void Warn(const Args & ...args)
        {
            mLogger->warn(args...);
        }

        template<typename ...Args>
        inline void Error(const Args & ...args)
        {
            mLogger->error(args...);
        }

        template<typename ...Args>
        inline void Fatal(const Args & ...args)
        {
            mLogger->error(args...);
        }
	private:
#ifdef	GE_API_D3D12
		using LoggerPtr = std::shared_ptr<D3D12Logger>;

#else
		using LoggerPtr = std::shared_ptr<spdlog::logger>;

#endif 
		LoggerPtr mLogger;
	};


}

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else//Core log macros
#error "Unsupported compiler"
#endif



#ifdef GE_PLATFORM_WINDOWS
#define GE_CORE_TRACE(fmt,...) { g_Log->Trace(fmt,__VA_ARGS__);}
#define GE_CORE_INFO(fmt,...)  { g_Log->Info(fmt,__VA_ARGS__);}
#define GE_CORE_WARN(fmt,...)  { g_Log->Warn(fmt,__VA_ARGS__);}
#define GE_CORE_ERROR(fmt,...) { g_Log->Error(fmt,__VA_ARGS__); DEBUG_BREAK();}
#define GE_CORE_FATAL(fmt,...) { g_Log->Fatal(fmt,__VA_ARGS__);}

//#define GE_CORE_TRACE(...) { g_Log->Trace(__VA_ARGS__);}
//#define GE_CORE_INFO(...)  { g_Log->Info(__VA_ARGS__);}
//#define GE_CORE_WARN(...)  { g_Log->Warn(__VA_ARGS__);}
//#define GE_CORE_ERROR(...) { g_Log->Error(__VA_ARGS__); DEBUG_BREAK();}
//#define GE_CORE_FATAL(...) { g_Log->Fatal(__VA_ARGS__);}


#elif defined(GE_PLATFORM_ANDROID)
#define GE_CORE_TRACE(fmt, ...)  LOGE(fmt, __VA_ARGS__)
#define GE_CORE_INFO(fmt, ...)   LOGI(fmt, __VA_ARGS__)
#define GE_CORE_WARN(fmt, ...)   LOGW(fmt, __VA_ARGS__)
#define GE_CORE_ERROR(fmt, ...)  {LOGE(fmt, __VA_ARGS__); DEBUG_BREAK();}
#define GE_CORE_FATAL(fmt, ...)  LOGE(fmt, __VA_ARGS__);

#define GE_CORE_TRACE_VOID(...)  LOGI("%s", __VA_ARGS__)
#define GE_CORE_INFO_VOID(...)   LOGI("%s", __VA_ARGS__)
#define GE_CORE_WARN_VOID(...)   LOGW("%s", __VA_ARGS__)
#define GE_CORE_ERROR_VOID(...)  {LOGE("%s", __VA_ARGS__); DEBUG_BREAK();}
#define GE_CORE_FATAL_VOID(...)  LOGE("%s", __VA_ARGS__);
#endif


#endif
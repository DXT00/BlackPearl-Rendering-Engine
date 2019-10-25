#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
namespace BlackPearl {

	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;

	};
}
	//Core log macros
#define GE_CORE_TRACE(...) BlackPearl::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define GE_CORE_INFO(...) BlackPearl::Log::GetCoreLogger()->info(__VA_ARGS__);
#define GE_CORE_WARN(...)  BlackPearl::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define GE_CORE_ERROR(...) BlackPearl::Log::GetCoreLogger()->error(__VA_ARGS__);
#define GE_CORE_FATAL(...) BlackPearl::Log::GetCoreLogger()->fatal(__VA_ARGS__);


#pragma once
#include <string>
namespace BlackPearl {

	class D3D12Logger
	{

	public:
		enum level_enum
		{
			Trace = 0,
			Info,
			Warn,
			Error,
			Fatal,
			Off 
		};

		D3D12Logger() {}

		//static std::shared_ptr<D3D12Logger>& GetCoreLogger() {
		//	static std::shared_ptr<D3D12Logger> s_Logger(DBG_NEW D3D12Logger());
		//	return s_Logger;
		//}
		template<typename ...Args>
		void trace(const Args &... args);
		template<typename ...Args>
		void info(const Args &... args);
		template<typename ...Args>
		void warn(const Args &... args);
		template<typename ...Args>
		void error(const Args &... args);
		template<typename ...Args>
		void fatal(const Args &... args);

		//TODO:: ��δʵ��
		template<typename... Args>
		void trace(const char* fmt, const Args &... args) {};
		template<typename... Args>
		void info(const char* fmt, const Args &... args) {};
		template<typename... Args>
		void warn(const char* fmt, const Args &... args) {};
		template<typename... Args>
		void error(const char* fmt, const Args &... args) {};
		template<typename... Args>
		void fatal(const char* fmt, const Args &... args) {};

		template<typename... Args>
		std::string FormatPrint(Args... args);
	private:
		void DebugPrint(level_enum level, std::string msg);
		std::string LevelToStr(level_enum level);


		template<typename T>
		std::string format(T t);


	};

	template<typename T>
	std::string D3D12Logger::format(T t) {
		std::stringstream ss;
		ss << t ;
		return ss.str();
	}

	template<typename... Args>
	std::string D3D12Logger::FormatPrint(Args... args)
	{
		//(std::cout << ... << format(args)) << std::endl;
		return  format(args...);
	}

	template<typename ...Args>
	void D3D12Logger::trace(const Args &... args)
	{
		DebugPrint(level_enum::Trace, FormatPrint(args...));
	}
	template<typename ...Args>
	void D3D12Logger::info(const Args &... args)
	{
		DebugPrint(level_enum::Info, FormatPrint(args...));
	}
	template<typename ...Args>
	void D3D12Logger::warn(const Args &... args)
	{
		DebugPrint(level_enum::Warn, FormatPrint(args...));
	}
	template<typename ...Args>
	void D3D12Logger::error(const Args &... args)
	{
		DebugPrint(level_enum::Error, FormatPrint(args...));
	}
	template<typename ...Args>
	void D3D12Logger::fatal(const Args &... args)
	{
		DebugPrint(level_enum::Fatal, FormatPrint(args...));
	}

}

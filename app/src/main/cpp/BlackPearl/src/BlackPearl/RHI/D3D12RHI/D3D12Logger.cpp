#include "pch.h"
#include "D3D12Logger.h"
//#include <debugapi.h>
#ifdef GE_API_D3D12
namespace BlackPearl {

	std::string D3D12Logger::LevelToStr(level_enum level) {
		switch (level) {

		case level_enum::Trace:
			return "trace";
		case level_enum::Info:
			return "info";
		case level_enum::Warn:
			return "warn";
		case level_enum::Error:
			return "error";
		case level_enum::Fatal:
			return "fatal";
		default:
				return "";
		}
	}

	void D3D12Logger::DebugPrint(level_enum level, std::string msg) {
		std::string printStr = "[" + LevelToStr(level) + "] " + msg + "\n";
		//OutputDebugStringA(printStr.c_str());

	}


}

#endif
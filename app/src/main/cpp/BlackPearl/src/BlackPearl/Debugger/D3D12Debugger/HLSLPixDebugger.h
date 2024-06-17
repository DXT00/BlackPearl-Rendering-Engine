#pragma once
#ifdef GE_PLATFORM_WINDOWS
//*********************************************************
// Dubugger: Pix on Windows
// Demo from document: https://devblogs.microsoft.com/pix/taking-a-capture/
//
//*********************************************************
namespace BlackPearl {
	class HLSLPixDebugger
	{
	public:
		static std::wstring GetLatestWinPixGpuCapturerPath_Cpp17();

	};

}

#endif

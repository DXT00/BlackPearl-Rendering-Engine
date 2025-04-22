#pragma once
#include "Windows.h"
#include "../Generic/GenericPlatformProc.h"

#include "../Platform.h"
namespace BlackPearl {

	class FWindowsPlatformProc  : public FGenericPlatformProc{

	public:
		static void* GetDllExport(void* DllHandle, const char* ProcName);
		static void* GetDllHandle(const char* ProcName);
		static void FreeDllHandle(void* DllHandle);

        //Threads

    };
	typedef FWindowsPlatformProc FPlatformProc;

}

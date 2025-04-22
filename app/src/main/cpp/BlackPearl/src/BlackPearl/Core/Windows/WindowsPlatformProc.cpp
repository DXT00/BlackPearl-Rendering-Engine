#include "pch.h"
#include "Windows.h"
#include "Core/Windows/WindowsPlatformProc.h"

namespace BlackPearl {


	void* FWindowsPlatformProc::GetDllExport(void* DllHandle, const char* ProcName)
	{
		return (void*)::GetProcAddress((HMODULE)DllHandle, ProcName);

	}

	void* FWindowsPlatformProc::GetDllHandle(const char* ProcName)
	{
		return LoadLibraryA(ProcName);
	}

	void FWindowsPlatformProc::FreeDllHandle(void* DllHandle) {
		::FreeLibrary((HMODULE)DllHandle);
	}
}
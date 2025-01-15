
#include "Windows.h"

namespace BlackPearl {

	class FWindowsPlatformProc {

	public:
		static void* GetDllExport(void* DllHandle, const char* ProcName);
		static void* GetDllHandle(const char* ProcName);
		static void FreeDllHandle(void* DllHandle);

	};
	typedef FWindowsPlatformProc FPlatformProc;

}

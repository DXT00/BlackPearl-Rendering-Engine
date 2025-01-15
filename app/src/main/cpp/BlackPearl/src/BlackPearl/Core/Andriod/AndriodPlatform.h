
#include <dlfcn.h> 
#include <time.h>


namespace BlackPearl {

	class FAndriodPlatformProc {

	public:
		static void* GetDllExport(void* DllHandle, const char* ProcName);
		static void* GetDllHandle(const char* ProcName);
		static void FreeDllHandle(void* DllHandle);

	};
	typedef FAndriodPlatformProc FPlatformProc;

}


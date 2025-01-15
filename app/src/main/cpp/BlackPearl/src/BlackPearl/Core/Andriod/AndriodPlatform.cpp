#include "AndriodPlatform.h"

#include <dlfcn.h> 
#include <time.h>


namespace BlackPearl {

	void* FAndriodPlatformProc::GetDllExport(void* DllHandle, const char* ProcName)
	{

		return dlsym(DllHandle, ProcName);

	}

	void* FAndriodPlatformProc::GetDllHandle(const char* ProcName)
	{
		void* dp = dlopen(ProcName, RTLD_LAZY);
		if (dp == NULL) {
			printf("%s\n", dlerror());
		}
		return dp;
	}

	void FAndriodPlatformProc::FreeDllHandle(void* DllHandle) {
		dlclose(DllHandle);
	}
}
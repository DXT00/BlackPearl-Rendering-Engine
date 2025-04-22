#pragma once
#ifdef GE_PLATFORM_ANDROID
#include "../Generic/GenericPlatformProc.h"
#include "../Platform.h"
#include <dlfcn.h> 

#include <pthread.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>
#include <unistd.h>


static const uint32_t InvalidTlsSlot = 0xFFFFFFFF;

namespace BlackPearl {

	class FAndriodPlatformProc :public FGenericPlatformProc {

	public:
		static void* GetDllExport(void* DllHandle, const char* ProcName);
		static void* GetDllHandle(const char* ProcName);
		static void FreeDllHandle(void* DllHandle);

	};
	typedef FAndriodPlatformProc FPlatformProc;



/**
 * Android implementation of the TLS OS functions
 */
    struct FAndroidTLS
    {
        /**
         * Returns the currently executing thread's id
         */
        static FORCEINLINE uint32_t GetCurrentThreadId(void)
        {
            static_assert(sizeof(pid_t) <= sizeof(uint32_t), "pid_t is larger than uint32_t, reconsider implementation of GetCurrentThreadId()");
            return static_cast<uint32_t>(gettid());
//		return pthread_self();
        }

        /**
         * Allocates a thread local store slot
         */
        static FORCEINLINE uint32_t AllocTlsSlot(void)
        {
            // allocate a per-thread mem slot
            pthread_key_t Key = 0;
            if (pthread_key_create(&Key, NULL) != 0)
            {
                Key = InvalidTlsSlot;  // matches the Windows TlsAlloc() retval //@todo android: should probably check for this below, or assert out instead
            }
            return Key;
        }

        /**
         * Sets a value in the specified TLS slot
         *
         * @param SlotIndex the TLS index to store it in
         * @param Value the value to store in the slot
         */
        static FORCEINLINE void SetTlsValue(uint32_t SlotIndex,void* Value)
        {
            pthread_setspecific((pthread_key_t)SlotIndex, Value);
        }

        /**
         * Reads the value stored at the specified TLS slot
         *
         * @return the value stored in the slot
         */
        static FORCEINLINE void* GetTlsValue(uint32_t SlotIndex)
        {
            return pthread_getspecific((pthread_key_t)SlotIndex);
        }

        /**
         * Frees a previously allocated TLS slot
         *
         * @param SlotIndex the TLS index to store it in
         */
        static FORCEINLINE void FreeTlsSlot(uint32_t SlotIndex)
        {
            pthread_key_delete((pthread_key_t)SlotIndex);
        }
    };

    typedef FAndroidTLS FPlatformTLS;


}

#endif
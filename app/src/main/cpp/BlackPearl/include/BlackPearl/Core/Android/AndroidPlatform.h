//
// Created by DXT00 on 2025/4/13.
//
#pragma once
#define PLATFORM_HAS_BSD_TIME						1


#define ANDROID_MAX_PATH 256

// DLL export and import definitions
#define DLLEXPORT			__attribute__((visibility("default")))
#define DLLIMPORT			__attribute__((visibility("default")))
#define JNI_METHOD			__attribute__ ((visibility ("default"))) extern "C"

#if _DEBUG
#define FORCEINLINE	inline									/* Easier to debug */
#else
#define FORCEINLINE inline __attribute__ ((always_inline))	/* Force code to be inline */
#endif

#define INSTANCE_HANDLE int

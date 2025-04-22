//
// Created by DXT00 on 2025/4/13.
//
#pragma once
#define PLATFORM_HAS_BSD_TIME						0

#if _DEBUG
#define FORCEINLINE	inline									/* Easier to debug */
#else
#define FORCEINLINE	 									    /* Easier to debug */
#endif

#define INSTANCE_HANDLE HINSTANCE

#pragma once
#pragma once
#include "stdint.h"
#include "../Platform.h"

#include <string.h>
namespace BlackPearl {

	struct GenericPlatformMemory
	{
		static FORCEINLINE void* Memmove(void* Dest, const void* Src, size_t Count)
		{
			return memmove(Dest, Src, Count);
		}

		static FORCEINLINE int32_t Memcmp(const void* Buf1, const void* Buf2, size_t Count)
		{
			return memcmp(Buf1, Buf2, Count);
		}

		static FORCEINLINE void* Memset(void* Dest, uint8_t Char, size_t Count)
		{
			return memset(Dest, Char, Count);
		}

		static FORCEINLINE void* Memzero(void* Dest, size_t Count)
		{
			return memset(Dest, 0, Count);
		}

		static FORCEINLINE void* Memcpy(void* Dest, const void* Src, size_t Count)
		{
			return memcpy(Dest, Src, Count);
		}




	};
}

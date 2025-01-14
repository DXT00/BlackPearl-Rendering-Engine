


#include "BlackPearl/Core.h"

#include COMPILED_PLATFORM_HEADER(Memory.h)

namespace BlackPearl {


struct FMemory
{
	/** Some allocators can be given hints to treat allocations differently depending on how the memory is used, it's lifetime etc. */
	enum AllocationHints
	{
		None = -1,
		Default,
		Temporary,
		SmallPool,

		Max
	};


	/** @name Memory functions (wrapper for FPlatformMemory) */

	static FORCEINLINE void* Memmove(void* Dest, const void* Src, size_t Count)
	{
		return FPlatformMemory::Memmove(Dest, Src, Count);
	}

	static FORCEINLINE int32_t Memcmp(const void* Buf1, const void* Buf2, size_t Count)
	{
		return FPlatformMemory::Memcmp(Buf1, Buf2, Count);
	}

	static FORCEINLINE void* Memset(void* Dest, uint8_t Char, size_t Count)
	{
		return FPlatformMemory::Memset(Dest, Char, Count);
	}

	template< class T >
	static FORCEINLINE void Memset(T& Src, uint8_t ValueToSet)
	{
		static_assert(!TIsPointer<T>::Value, "For pointers use the three parameters function");
		Memset(&Src, ValueToSet, sizeof(T));
	}

	static FORCEINLINE void* Memzero(void* Dest, size_t Count)
	{
		return FPlatformMemory::Memzero(Dest, Count);
	}

	/** Returns true if memory is zeroes, false otherwise. */
	static FORCEINLINE bool MemIsZero(const void* Ptr, size_t Count)
	{
		// first pass implementation
		uint8_t* Start = (uint8_t*)Ptr;
		uint8_t* End = Start + Count;
		while (Start < End)
		{
			if ((*Start++) != 0)
			{
				return false;
			}
		}

		return true;
	}

	template< class T >
	static FORCEINLINE void Memzero(T& Src)
	{
		Memzero(&Src, sizeof(T));
	}

	static FORCEINLINE void* Memcpy(void* Dest, const void* Src, size_t Count)
	{
		return FPlatformMemory::Memcpy(Dest, Src, Count);
	}

	template< class T >
	static FORCEINLINE void Memcpy(T& Dest, const T& Src)
	{
		static_assert(!TIsPointer<T>::Value, "For pointers use the three parameters function");
		Memcpy(&Dest, &Src, sizeof(T));
	}


};

}
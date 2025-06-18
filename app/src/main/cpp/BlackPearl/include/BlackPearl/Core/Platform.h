
#pragma once

#include "BlackPearl/Core.h"


// Whether the CPU is x86/x64 (i.e. both 32 and 64-bit variants)
#ifndef PLATFORM_CPU_X86_FAMILY
#if (defined(_M_IX86) || defined(__i386__) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__)) && !defined(_M_ARM64EC)
#define PLATFORM_CPU_X86_FAMILY	1
#else
#define PLATFORM_CPU_X86_FAMILY	0
#endif
#endif

// Whether the CPU is AArch32/AArch64 (i.e. both 32 and 64-bit variants)
#ifndef PLATFORM_CPU_ARM_FAMILY
#if (defined(__arm__) || defined(_M_ARM) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC))
#define PLATFORM_CPU_ARM_FAMILY	1
#else
#define PLATFORM_CPU_ARM_FAMILY	0
#endif
#endif

//---------------------------------------------------------------------
// Utility for automatically setting up the pointer-sized integer type
//---------------------------------------------------------------------

template<typename T32BITS, typename T64BITS, int PointerSize>
struct SelectIntPointerType
{
    // nothing here are is it an error if the partial specializations fail
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 8>
{
    // Select the 64 bit type.
    typedef T64BITS TIntPointer;
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 4>
{
    // Select the 32 bit type.
    typedef T32BITS TIntPointer;
};

// Unsigned int. The same size as a pointer.
typedef SelectIntPointerType<uint32_t, uint64_t, sizeof(void*)>::TIntPointer UPTRINT;

// Signed int. The same size as a pointer.
typedef SelectIntPointerType<int32_t, int64_t, sizeof(void*)>::TIntPointer PTRINT;


#include COMPILED_PLATFORM_HEADER(Platform.h)

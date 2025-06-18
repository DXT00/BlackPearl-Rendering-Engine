#pragma once
// reference form UE 5.3
// Copyright Epic Games, Inc. All Rights Reserved.
#include "Core/Codec/Crc.h"
#include "Core/Platform.h"

#include <stdint.h>
#include <type_traits>


namespace UE
{
    namespace Private
    {
        FORCEINLINE uint32_t MurmurFinalize32(uint32_t Hash)
        {
            Hash ^= Hash >> 16;
            Hash *= 0x85ebca6b;
            Hash ^= Hash >> 13;
            Hash *= 0xc2b2ae35;
            Hash ^= Hash >> 16;
            return Hash;
        }
    }
}

namespace BlackPearl {


/**
 * Combines two hash values to get a third.
 * Note - this function is not commutative.
 *
 * This function cannot change for backward compatibility reasons.
 * You may want to choose HashCombineFast for a better in-memory hash combining function.
 */
inline uint32_t HashCombine(uint32_t A, uint32_t C)
{
    uint32_t B = 0x9e3779b9;
    A += B;

    A -= B; A -= C; A ^= (C >> 13);
    B -= C; B -= A; B ^= (A << 8);
    C -= A; C -= B; C ^= (B >> 13);
    A -= B; A -= C; A ^= (C >> 12);
    B -= C; B -= A; B ^= (A << 16);
    C -= A; C -= B; C ^= (B >> 5);
    A -= B; A -= C; A ^= (C >> 3);
    B -= C; B -= A; B ^= (A << 10);
    C -= A; C -= B; C ^= (B >> 15);

    return C;
}

/**
 * Combines two hash values to get a third.
 * Note - this function is not commutative.
 *
 * WARNING!  This function is subject to change and should only be used for creating
 *           combined hash values which don't leave the running process,
 *           e.g. GetTypeHash() overloads.
 */
inline uint32_t HashCombineFast(uint32_t A, uint32_t B)
{
    return A ^ (B + 0x9e3779b9 + (A << 6) + (A >> 2));
}

inline uint32_t PointerHash(const void* Key)
{
    // Ignoring the lower 4 bits since they are likely zero anyway.
    // Higher bits are more significant in 64 bit builds.
    const UPTRINT PtrInt = reinterpret_cast<UPTRINT>(Key) >> 4;
    return UE::Private::MurmurFinalize32((uint32_t)PtrInt);
}

inline uint32_t PointerHash(const void* Key, uint32_t C)
{
    // we can use HashCombineFast here because pointers are non-persistent
    return HashCombineFast(PointerHash(Key), C);
}


//
// Hash functions for common types.
//
// WARNING!  GetTypeHash result values are not expected to leave the running process.
//           Do not persist them to disk, send them to another running process or
//           expect them to be consistent across multiple runs.
//

template <
    typename ScalarType,
    std::enable_if_t<std::is_scalar_v<ScalarType> && !std::is_same_v<ScalarType, char*> && !std::is_same_v<ScalarType, const char*>>* = nullptr
>
inline uint32_t GetTypeHash(ScalarType Value)
{
    if constexpr (std::is_integral_v<ScalarType>)
    {
        if constexpr (sizeof(ScalarType) <= 4)
        {
            return Value;
        }
        else if constexpr (sizeof(ScalarType) == 8)
        {
            return (uint32_t)Value + ((uint32_t)(Value >> 32) * 23);
        }
        else if constexpr (sizeof(ScalarType) == 16)
        {
            const uint64_t Low = (uint64_t)Value;
            const uint64_t High = (uint64_t)(Value >> 64);
            return GetTypeHash(Low) ^ GetTypeHash(High);
        }
        else
        {
            static_assert(sizeof(ScalarType) == 0, "Unsupported integral type");
        }
    }
    else if constexpr (std::is_floating_point_v<ScalarType>)
    {
        if constexpr (std::is_same_v<ScalarType, float>)
        {
            return *(uint32_t*)&Value;
        }
        else if constexpr (std::is_same_v<ScalarType, double>)
        {
            return GetTypeHash(*(uint64_t*)&Value);
        }
        else
        {
            static_assert(sizeof(ScalarType) == 0, "Unsupported floating point type");
        }
    }
    else if constexpr (std::is_enum_v<ScalarType>)
    {
        return GetTypeHash((__underlying_type(ScalarType))Value);
    }
    else if constexpr (std::is_pointer_v<ScalarType>)
    {
        // Once the char* deprecations below are removed, we want to prevent accidental string hashing, so this static_assert should be commented back in
        //static_assert(!TIsCharType<std::remove_pointer_t<ScalarType>>::Value, "Pointers to string types should use a PointerHash() or FCrc::Stricmp_DEPRECATED() call depending on requirements");

        return PointerHash(Value);
    }
    else
    {
        static_assert(sizeof(ScalarType) == 0, "Unsupported scalar type");
    }
}

template <
    typename T,
    uint32_t N,
    std::enable_if_t<!std::is_same_v<const T, const char>>* = nullptr
>
//UE_DEPRECATED(all, "Hashing arrays is deprecated - use PointerHash() instead to force a conversion to a pointer or GetArrayHash to hash the array contents")
inline uint32_t GetTypeHash(T(&Array)[N])
{
    return PointerHash(Array);
}

template <
    typename T,
    std::enable_if_t<std::is_same_v<const T, const char>>* = nullptr
>
//UE_DEPRECATED(5.3, "Hashing char arrays is deprecated - use PointerHash() to force a conversion to a pointer or FCrc::Strihash_DEPRECATED to do a string hash, or use TStringPointerSetKeyFuncs_DEPRECATED or TStringPointerMapKeyFuncs_DEPRECATED as keyfuncs for a TSet or TMap respectively")
inline uint32_t GetTypeHash(T* Value)
{
    // Hashing a char* array differently from a void* is dangerous and is deprecated.
    // When removing these overloads post-deprecation, comment in the related static_assert in the std::is_pointer_v block of the GetTypeHash overload above.
    return FCrc::Strihash_DEPRECATED(Value);
}

template <typename T>
inline uint32_t GetArrayHash(const T* Ptr, uint64_t Size, uint32_t PreviousHash = 0)
{
    uint32_t Result = PreviousHash;
    while (Size)
    {
        Result = HashCombineFast(Result, GetTypeHash(*Ptr));
        ++Ptr;
        --Size;
    }

    return Result;
}

// Use this when inside type that has GetTypeHash() (no in-parameters) implemented. It makes GetTypeHash dispatch in global namespace
template <typename T>
FORCEINLINE uint32_t GetTypeHashHelper(const T& V) { return GetTypeHash(V); }
}
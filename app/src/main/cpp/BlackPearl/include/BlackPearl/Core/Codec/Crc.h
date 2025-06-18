#pragma once

#include <cstdint>
#include "Core/Platform.h"
#include "Core/Templates/EnableIf.h"

namespace BlackPearl {


    // Defines the intrinsic version if available (faster). Do not use directly. Use BYTESWAP_ORDERxx() or ByteSwap() functions
#if defined(_MSC_VER)
#define GE_BYTESWAP_INTRINSIC_PRIVATE_16(Val) _byteswap_ushort(Val);
#define GE_BYTESWAP_INTRINSIC_PRIVATE_32(Val) _byteswap_ulong(Val);
#define GE_BYTESWAP_INTRINSIC_PRIVATE_64(Val) _byteswap_uint64(Val);
#elif defined(__clang__)
#if (__has_builtin(__builtin_bswap16))
#define GE_BYTESWAP_INTRINSIC_PRIVATE_16(Val) __builtin_bswap16(Val);
#endif
#if (__has_builtin(__builtin_bswap32))
#define GE_BYTESWAP_INTRINSIC_PRIVATE_32(Val) __builtin_bswap32(Val);
#endif
#if (__has_builtin(__builtin_bswap64))
#define GE_BYTESWAP_INTRINSIC_PRIVATE_64(Val) __builtin_bswap64(Val);
#endif
#endif

    // These macros are not safe to use unless data is UNSIGNED!
#define BYTESWAP_ORDER16_unsigned(x) ((((x) >> 8) & 0xff) + (((x) << 8) & 0xff00))
#define BYTESWAP_ORDER32_unsigned(x) (((x) >> 24) + (((x) >> 8) & 0xff00) + (((x) << 8) & 0xff0000) + ((x) << 24))

// Encapsulate Byte swapping generic versions for benchmarking purpose (compare the generic vs intrinsic performance: See ByteSwapTest.cpp).
    namespace Internal
    {
        static FORCEINLINE uint16_t ByteSwapGeneric16(uint16_t Value)
        {
            return (BYTESWAP_ORDER16_unsigned(Value));
        }

        static FORCEINLINE uint32_t ByteSwapGeneric32(uint32_t Value)
        {
            return (BYTESWAP_ORDER32_unsigned(Value));
        }

        static FORCEINLINE uint64_t ByteSwapGeneric64(uint64_t Value)
        {
            Value = ((Value << 8) & 0xFF00FF00FF00FF00ULL) | ((Value >> 8) & 0x00FF00FF00FF00FFULL);
            Value = ((Value << 16) & 0xFFFF0000FFFF0000ULL) | ((Value >> 16) & 0x0000FFFF0000FFFFULL);
            return (Value << 32) | (Value >> 32);
        }
    } // namespace Internal


    FORCEINLINE uint32_t BYTESWAP_ORDER32(uint32_t Val)
    {
#if defined(GE_BYTESWAP_INTRINSIC_PRIVATE_32)
        return GE_BYTESWAP_INTRINSIC_PRIVATE_32(Val);
#else
        return Internal::ByteSwapGeneric32(Val);
#endif
    }

/**
 * CRC hash generation for different types of input data
 **/
struct FCrc
{
    /** lookup table with precalculated CRC values - slicing by 8 implementation */
    static  uint32_t CRCTablesSB8[8][256];

    /** initializes the CRC lookup table. Must be called before any of the
        CRC functions are used. */
    static  void Init();

    /** generates CRC hash of the memory area */
    typedef uint32_t(*MemCrc32Functor)(const void* Data, int32_t Length, uint32_t CRC);
    static  MemCrc32Functor MemCrc32Func;
    static FORCEINLINE uint32_t MemCrc32(const void* Data, int32_t Length, uint32_t CRC = 0)
    {
        return MemCrc32Func(Data, Length, CRC);
    }

    /** generates CRC hash of the element */
    template <typename T>
    static uint32_t TypeCrc32(const T& Data, uint32_t CRC = 0)
    {
        return MemCrc32(&Data, sizeof(T), CRC);
    }

    /** String CRC. */
    template <typename CharType>
    static typename TEnableIf<sizeof(CharType) != 1, uint32_t>::Type StrCrc32(const CharType* Data, uint32_t CRC = 0)
    {
        // We ensure that we never try to do a StrCrc32 with a CharType of more than 4 bytes.  This is because
        // we always want to treat every CRC as if it was based on 4 byte chars, even if it's less, because we
        // want consistency between equivalent strings with different character types.
        static_assert(sizeof(CharType) <= 4, "StrCrc32 only works with CharType up to 32 bits.");

        CRC = ~CRC;
        while (CharType Ch = *Data++)
        {
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
            Ch >>= 8;
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
            Ch >>= 8;
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
            Ch >>= 8;
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
        }
        return ~CRC;
    }

    template <typename CharType>
    static typename TEnableIf<sizeof(CharType) == 1, uint32_t>::Type StrCrc32(const CharType* Data, uint32_t CRC = 0)
    {
        /* Overload for when CharType is a byte, which causes warnings when right-shifting by 8 */
        CRC = ~CRC;
        while (CharType Ch = *Data++)
        {
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
            CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
        }
        return ~CRC;
    }

    /**
     * DEPRECATED
     * These tables and functions are deprecated because they're using tables and implementations
     * which give values different from what a user of a typical CRC32 algorithm might expect.
     */

     /** lookup table with precalculated CRC values */
    static  uint32_t CRCTable_DEPRECATED[256];
    /** lookup table with precalculated CRC values - slicing by 8 implementation */
    static  uint32_t CRCTablesSB8_DEPRECATED[8][256];

    /** String CRC. */
    template <typename CharType>
    static inline uint32_t StrCrc_DEPRECATED(const CharType* Data)
    {
        // make sure table is initialized
        GE_ASSERT(CRCTable_DEPRECATED[1] != 0);

        uint32_t CRC = 0xFFFFFFFF;
        while (*Data)
        {
            CharType C = *Data++;
            int32_t CL = (C & 255);
            CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CL];
            int32_t CH = (C >> 8) & 255;
            CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CH];
        }
        return ~CRC;
    }

    /** String CRC. */
    template <typename CharType>
    static inline uint32_t StrCrc_DEPRECATED(const int32_t DataLen, const CharType* Data)
    {
        // make sure table is initialized
        GE_ASSERT(CRCTable_DEPRECATED[1] != 0);

        uint32_t CRC = 0xFFFFFFFF;
        for (int32_t i = 0; i < DataLen; i++)
        {
            CharType C = *Data++;
            int32_t CL = (C & 255);
            CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CL];
            int32_t CH = (C >> 8) & 255;
            CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CH];
        }
        return ~CRC;
    }

    /** Case insensitive string hash function. */
    template <typename CharType> static inline uint32_t Strihash_DEPRECATED(const CharType* Data);
    template <typename CharType> static inline uint32_t Strihash_DEPRECATED(const int32_t DataLen, const CharType* Data);

    /** generates CRC hash of the memory area */
    static  uint32_t MemCrc_DEPRECATED(const void* Data, int32_t Length, uint32_t CRC = 0);
};

}

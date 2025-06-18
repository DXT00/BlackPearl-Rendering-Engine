#pragma once

#include <cstdint>
#include "Core/Platform.h"
namespace BlackPearl {

    /**
     * Includes a function in an overload set if the predicate is true.  It should be used similarly to this:
     *
     * // This function will only be instantiated if SomeTrait<T>::Value is true for a particular T
     * template <typename T>
     * typename TEnableIf<SomeTrait<T>::Value, ReturnType>::Type Function(const T& Obj)
     * {
     *     ...
     * }
     *
     * ReturnType is the real return type of the function.
     */
    template <bool Predicate, typename Result = void>
    class TEnableIf;

    template <typename Result>
    class TEnableIf<true, Result>
    {
    public:
        using type = Result;
        using Type = Result;
    };

    template <typename Result>
    class TEnableIf<false, Result>
    {
    };


    template <typename T>
    FORCEINLINE typename TEnableIf<std::is_same_v<T, uint32_t>, T>::Type ReverseBits(T Bits)
    {
        Bits = (Bits << 16) | (Bits >> 16);
        Bits = ((Bits & 0x00ff00ff) << 8) | ((Bits & 0xff00ff00) >> 8);
        Bits = ((Bits & 0x0f0f0f0f) << 4) | ((Bits & 0xf0f0f0f0) >> 4);
        Bits = ((Bits & 0x33333333) << 2) | ((Bits & 0xcccccccc) >> 2);
        Bits = ((Bits & 0x55555555) << 1) | ((Bits & 0xaaaaaaaa) >> 1);
        return Bits;
    }


}

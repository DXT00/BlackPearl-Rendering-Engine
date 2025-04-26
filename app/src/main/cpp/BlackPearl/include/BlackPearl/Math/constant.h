//
// Created by DXT00 on 2025/4/9.
//

#ifndef BLACKPEARL_ENGINE_CONSTANT_H
#define BLACKPEARL_ENGINE_CONSTANT_H
namespace BlackPearl::math
{

    constexpr float PI_f = 3.141592654f;
    constexpr double PI_d = 3.14159265358979323;

    // Convenient float constants
    constexpr float _epsilon = 1e-6f;		// A reasonable general-purpose epsilon

    // Generic min/max/abs/clamp/saturate
#ifdef GE_PLATFORM_WINDOWS
#ifdef min
#undef min
#endif
#endif
    template <typename T>
    constexpr T min(T a, T b) { return (a < b) ? a : b; }
#ifdef GE_PLATFORM_WINDOWS
#ifdef max
#undef max
#endif
#endif

    template <typename T>
    constexpr T max(T a, T b) { return (a < b) ? b : a; }
}
#endif //BLACKPEARL_ENGINE_CONSTANT_H

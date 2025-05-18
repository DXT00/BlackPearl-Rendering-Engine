//
// Created by DXT00 on 2025/5/14.
//

#pragma once
#include <chrono>
using namespace std::chrono;
namespace BlackPearl {
    class SystemTime {
    public:

        static void Start();
        static milliseconds GetCurentTimeMs();

        static double GetRuntimeFromStartMs();


        static std::chrono::milliseconds StartTimeMs;
    };
}
//
// Created by DXT00 on 2025/5/16.
//
#pragma once
#include "pch.h"
#include "RHI/Common/FormatInfo.h"
namespace BlackPearl{
    //TODO::区分
    struct PlatformGbufferConfig
    {
        bool hasVelocity;
        bool hasTangent;
        bool bHighPresision;

        PlatformGbufferConfig(){
            hasVelocity = false;
            hasTangent = false;
            bHighPresision = false;
        }
        // bHighPresision:
        // true: use float16 bit
        // false: use float8/10 bit
    };



    struct GbufferTarget{
        Format format;
        bool isSrgb;

        GbufferTarget()
        {
            format = Format::UNKNOWN;
            isSrgb = false;
        }
    };

    struct GbufferInfo {
        static const int maxTarget = 8;
        int numTargets;
        GbufferTarget targets[maxTarget];


    };


    static GbufferInfo GetGbufferInfo(const PlatformGbufferConfig& config);


}



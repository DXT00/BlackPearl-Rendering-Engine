//
// Created by DXT00 on 2025/5/16.
//
#pragma once
#include "pch.h"
#include <string>
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
        std::string name;

        GbufferTarget()
        {
            format = Format::UNKNOWN;
            isSrgb = false;
            name = "";
        }
    };

    struct GbufferInfo {
        static const int maxTarget = 8;
        int numTargets;
        GbufferTarget targets[maxTarget];


    };

    GbufferTarget GetTargetByName(const GbufferInfo& info, const std::string& name);
    GbufferInfo GetGbufferInfo(const PlatformGbufferConfig& config);


}



#pragma once
#include "RHI/RHITexture.h"
#include "Object/Object.h"
#include "Math/vector.h"

namespace BlackPearl {

    constexpr uint32_t IrradianceOctSize = 8;
    constexpr uint32_t DepthOctSize = 16;

    namespace RaytraceScale
    {
        enum Id : int32_t
        {
            Full,
            Half,
            Quarter,
            Length
        };

        static const char* Names[] =
        {
            "Full",
            "Half",
            "Quarter",
            nullptr };
    }        // namespace RaytraceScale

    

    struct IrradianceVolume
    {
        float4  startPos;
        float   probeDistance = 1.5f;
        bool    infiniteBounce = true;
        int32_t raysPerProbe = 256;
        float   hysteresis = 0.98f;
        float   intensity = 1.0f;
        float   normalBias = 0.1f;
        float   depthSharpness = 50.f;
        float   ddgiGamma = 5.f;

        float             width;
        float             height;
        float             depth;

        RaytraceScale::Id scale = RaytraceScale::Full;
        bool              enable = true;


        TextureHandle currentIrrdance;
        TextureHandle currentDepth;


        Object* lightProbeGrid;

       // DescriptorSet::Ptr ddgiCommon;// set 5.
    };
  


}

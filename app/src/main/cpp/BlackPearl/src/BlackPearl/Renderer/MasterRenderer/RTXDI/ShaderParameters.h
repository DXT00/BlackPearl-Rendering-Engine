#pragma once
#include "BlackPearl/Math/vector.h"
namespace BlackPearl {
    using namespace math;
    // Stores shared light information (type) and specific light information
    // See PolymorphicLight.hlsli for encoding format
    struct PolymorphicLightInfo
    {
        // uint4[0]
        float3 center;
        uint colorTypeAndFlags; // RGB8 + uint8 (see the kPolymorphicLight... constants above)

        // uint4[1]
        uint direction1; // oct-encoded
        uint direction2; // oct-encoded
        uint scalars; // 2x float16
        uint logRadiance; // uint16 | empty slot 

        // uint4[2] -- optional, contains only shaping data
        uint iesProfileIndex;
        uint primaryAxis; // oct-encoded
        uint cosConeAngleAndSoftness; // 2x float16
        uint padding;
    };


    struct PrepareLightsTask
    {
        uint instanceAndGeometryIndex; // low 12 bits are geometryIndex, mid 19 bits are instanceIndex, high bit is TASK_PRIMITIVE_LIGHT_BIT
        uint triangleCount;
        uint lightBufferOffset;
        int previousLightBufferOffset; // -1 means no previous data
    };
}

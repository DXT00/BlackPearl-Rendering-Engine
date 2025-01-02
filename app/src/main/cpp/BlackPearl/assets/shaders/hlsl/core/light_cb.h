/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#ifndef LIGHT_CB_H
#define LIGHT_CB_H
#include "align.h"
#include "light_types.h"
//#ifdef GE_SHADERCOMPILE
//
//#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
//#endif

//#if COMPILE_SHADER
//
//#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
//#endif
struct ShadowConstants
{
    ALIGN(16) float4x4 matWorldToUvzwShadow;

    ALIGN(8) float2 shadowFadeScale;
    ALIGN(8) float2 shadowFadeBias;

    ALIGN(8) float2 shadowMapCenterUV;
    ALIGN(4) float shadowFalloffDistance;
    ALIGN(4) int shadowMapArrayIndex;

    ALIGN(8) float2 shadowMapSizeTexels;
    ALIGN(8) float2 shadowMapSizeTexelsInv;
};

struct LightConstants
{
    ALIGN(16) float3 direction;
    ALIGN(4) int lightType;

    ALIGN(16) float3 position;
    ALIGN(4) float radius;

    ALIGN(16) float3 color;
    ALIGN(4) float intensity; // illuminance (lm/m2) for directional lights, luminous intensity (lm/sr) for positional lights

    ALIGN(4) float angularSizeOrInvRange;   // angular size for directional lights, 1/range for spot and point lights
    ALIGN(4) float innerAngle;
    ALIGN(4) float outerAngle;
    ALIGN(4) float outOfBoundsShadow;

    ALIGN(4) int4 shadowCascades;
    ALIGN(4) int4 perObjectShadows;

    ALIGN(4) int4 shadowChannel;
};

struct LightProbeConstants
{
    ALIGN(4) float diffuseScale;
    ALIGN(4) float specularScale;
    ALIGN(4) float mipLevels;
    ALIGN(4) float padding1;

    ALIGN(4) uint diffuseArrayIndex;
    ALIGN(4) uint specularArrayIndex;
    ALIGN(8) uint2 padding2;

    ALIGN(16) float4 frustumPlanes[6];
};

#endif // LIGHT_CB_H
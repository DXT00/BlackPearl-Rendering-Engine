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

#ifndef LIGHT_PROBE_CB_H
#define LIGHT_PROBE_CB_H

#include "align.h"
//#ifdef GE_SHADERCOMPILE
//
//#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
//#endif

//struct LightProbeConstants
//{
//    uint sampleCount;
//    float lodBias;
//    float roughness;
//    float inputCubeSize;
//};
const int PT_DIFFUSE_PROBE = 0;
const int PT_REFLECTION_PROBE = 1;

struct LightProbeConstants // ALIGN(16)
{
    /*   ALIGN(4) float diffuseScale;
       ALIGN(4) float specularScale;
       ALIGN(4) float mipLevels;
       ALIGN(4) float padding1;

       ALIGN(4) uint diffuseArrayIndex;
       ALIGN(4) uint specularArrayIndex;
       ALIGN(8) uint2 padding2;

       ALIGN(16) float4 frustumPlanes[6];*/
    ALIGN(4)  int probeType;
    ALIGN(16) float3 pos;
    //每个 SHCoeffs[i] 必须是 16 字节对齐, 使用 float4
    ALIGN(16) float4 SHCoeffs[9];


};
#endif // LIGHT_PROBE_CB_H
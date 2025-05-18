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

#ifndef FORWARD_CB_H
#define FORWARD_CB_H

#include "light_cb.h"
#include "view_cb.h"
//#ifdef GE_SHADERCOMPILE
//
//#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
//#endif
#define FORWARD_MAX_LIGHTS 16
#define FORWARD_MAX_SHADOWS 16
#define FORWARD_MAX_LIGHT_PROBES 16
#include "align.h"
struct ForwardShadingViewConstants
{
   // PlanarViewConstants view;
    ALIGN(8) float2      viewportOrigin;
    ALIGN(8) float2      viewportSize;

    //float2      viewportSizeInv;
    //float2      pixelOffset;

    //float2      clipToWindowScale;
    //float2      clipToWindowBias;

    //float2      windowToClipScale;
    //float2      windowToClipBias;

    //float4      cameraDirectionOrPosition;

    ALIGN(16) float4x4 matProjectionView;
    ALIGN(16) float4x4 matView;
    ALIGN(16) float4x4 matProjection;
    ALIGN(16) float3 cameraPos;
    ALIGN(16) float3 cameraRot;
};

struct ForwardShadingLightConstants
{
    ALIGN(8) float2      shadowMapTextureSize;
    ALIGN(8) float2      shadowMapTextureSizeInv;
    ALIGN(16) float4      ambientColorTop;
    ALIGN(16) float4      ambientColorBottom;

    ALIGN(8) uint2       padding;
    ALIGN(4) uint        numLights;
    ALIGN(4) uint        numLightProbes;

    LightConstants lights[FORWARD_MAX_LIGHTS];
    ShadowConstants shadows[FORWARD_MAX_SHADOWS];
    LightProbeConstants lightProbes[FORWARD_MAX_LIGHT_PROBES];
};

#endif // FORWARD_CB_H
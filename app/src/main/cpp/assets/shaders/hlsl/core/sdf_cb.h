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

#ifndef SDF_CB_H
#define SDF_CB_H
#include "align.h"
#include "light_types.h"


#define MAX_CLIP_NUM 4

struct DFObjectConstants
{
    ALIGN(16) float3 extend; //extend
    ALIGN(16) float3 center; //center
};


struct GlobalSDFConstants
{
    ALIGN(4) float voxelSize;
    ALIGN(16) float3 clipmapCenter;
    ALIGN(16) float3 clipmapDimension;

    ALIGN(4) int objsCnt;
};





struct GlobalSDFTraceConstants
{
    ALIGN(16) float4 clipPosDistance[MAX_CLIP_NUM]; //clipCenter -> xyz / distance -> clipExtend * 0.5
    ALIGN(16) ALIGN(16)float4 clipVoxelSize; //clip0 - clip3 voxel size
    ALIGN(4) uint clipCount;
    ALIGN(4) float dimension;
   /* float nearPlane;
    float farPlane;*/
};


#endif // SDF_CB_H
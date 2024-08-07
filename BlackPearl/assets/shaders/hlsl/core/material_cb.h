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

#pragma once

//
//#ifdef GE_SHADERCOMPILE
//
//#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
//#endif


static const int MaterialDomain_Opaque                   = 0;
static const int MaterialDomain_AlphaTested              = 1;
static const int MaterialDomain_AlphaBlended             = 2;
static const int MaterialDomain_Transmissive             = 3;
static const int MaterialDomain_TransmissiveAlphaTested  = 4;
static const int MaterialDomain_TransmissiveAlphaBlended = 5;

static const int MaterialFlags_UseSpecularGlossModel            = 0x00000001;
static const int MaterialFlags_DoubleSided                      = 0x00000002;
static const int MaterialFlags_UseMetalRoughOrSpecularTexture   = 0x00000004;
static const int MaterialFlags_UseBaseOrDiffuseTexture          = 0x00000008;
static const int MaterialFlags_UseEmissiveTexture               = 0x00000010;
static const int MaterialFlags_UseNormalTexture                 = 0x00000020;
static const int MaterialFlags_UseOcclusionTexture              = 0x00000040;
static const int MaterialFlags_UseTransmissionTexture           = 0x00000080;
static const int MaterialFlags_ThinSurface                      = 0x00000100;
static const int MaterialFlags_PSDExclude                       = 0x00000200;
static const int MaterialFlags_UseHeightMapTexture              = 0x00000400;
static const int MaterialFlags_UseCubeMapTexture                = 0x00000800;
static const int MaterialFlags_UseDepthTexture                  = 0x00001000;
static const int MaterialFlags_UseSpecularTexture               = 0x00002000;
static const int MaterialFlags_UseRoughnessTexture              = 0x00004000;
static const int MaterialFlags_UseMetalTexture                  = 0x00008000;
static const int MaterialFlags_UseOpacityTexture                = 0x00010000;

static const int MaterialFlags_NestedPriorityMask               = 0xF0000000;
static const int MaterialFlags_NestedPriorityShift              = 28;

static const int MaterialFlags_PSDDominantDeltaLobeP1Mask       = 0x0F000000;
static const int MaterialFlags_PSDDominantDeltaLobeP1Shift      = 24;

// NOTE: adjust LoadMaterialConstants(...) in bindless.h when changing this structure



struct Props {
    //Enable texture
    float shininess;
    float refractIndex; //电解质系数
    bool  isBinnLight;
    int  isPBRTextureSample;//是否使用纹理-->包括 ao,normal,metalllic,roughness
    int  isDiffuseTextureSample;//是否使用纹理
    int  isSpecularTextureSample;//是否使用纹理
    int  isHeightTextureSample;//是否使用纹理
    int  isEmissionTextureSample;//是否使用纹理
    int isRefractMaterial;
    int isDoubleSided;

 /*   Props() {
        shininess = (64.0f),
            refractIndex = (1.5),
            isBinnLight = (false),
            isPBRTextureSample = (0),
            isDiffuseTextureSample = (0),
            isSpecularTextureSample = (0),
            isEmissionTextureSample = (0),
            isHeightTextureSample = (0),
            isRefractMaterial = (0);
    
    
    
    }*/

};



// using https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_volume#attenuation convention
struct VolumeConstants
{
    float3  attenuationColor;
    float   attenuationDistance;
};

struct MaterialConstants
{
    //float3  baseOrDiffuseColor;
    //float3  specularColor;
    //float3  emissiveColor;

    unsigned int flags;

    int     materialID;

    int     domain;

    float   opacity;
    //float   roughness;
    //float   metalness;
    float   normalTextureScale;

    //float   occlusionStrength;
    float   alphaCutoff;
    float   transmissionFactor;
    uint    baseOrDiffuseTextureIndex;

    uint    metalRoughOrSpecularTextureIndex;
    uint    emissiveTextureIndex;
    uint    normalTextureIndex;
    uint    occlusionTextureIndex;

    uint    transmissionTextureIndex;
    float   ior;
    float   thicknessFactor;
    float   diffuseTransmissionFactor;

    float   shadowNoLFadeout;
    uint    padding0;
    uint    padding1;
    uint    padding2;

    VolumeConstants volume;


    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    float3 emissiveColor;
    float roughnessValue;
    float metalnessValue;
    float aoValue;
  
    float shininess;
    float specularDiffusion;
    float diffuseReflectivity;
    float specularReflectivity;
    float transparency;
    float emissivity;

    float refractiveIndex;

    Props props;

};

//#endif // MATERIAL_CB_H

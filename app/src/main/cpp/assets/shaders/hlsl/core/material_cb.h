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
#include "align.h"
//
#ifdef GE_SHADERCOMPILE

#else
//#include "BlackPearl/Math/Math.h"
//using namespace BlackPearl::math;
#endif


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


static const int ShadingModel_Unlit = 0;
static const int ShadingModel_DefaultLit = 1;
static const int ShadingModel_Disney = 2;


// NOTE: adjust LoadMaterialConstants(...) in bindless.h when changing this structure



struct Props {
    //Enable texture
    float shininess;
    float refractIndex;
    bool  isBinnLight;
    int  isPBRTextureSample;//�Ƿ�ʹ������-->���� ao,normal,metalllic,roughness
    int  isDiffuseTextureSample;
    int  isSpecularTextureSample;
    int  isHeightTextureSample;
    int  isEmissionTextureSample;
    int isRefractMaterial;
    int isDoubleSided;



};

// material deal with texture sample, use for shading
struct MaterialSample {
    ALIGN(4) float3  shadingNormal;


    ALIGN(4) unsigned int flags;
    ALIGN(4) int     materialID;
    ALIGN(4) int     shadingModelID;
    ALIGN(4) int     domain;
    ALIGN(4) float   opacity;
    ALIGN(4) float   alphaThreshold; // for alpha tested materials
    //for bindless texture
    //  ALIGN(4) uint    baseOrDiffuseTextureIndex;
    //  ALIGN(4) uint    metalRoughOrSpecularTextureIndex;
    //  ALIGN(4) uint    emissiveTextureIndex;
    //  ALIGN(4) uint    normalTextureIndex;
    //  ALIGN(4) uint    occlusionTextureIndex;
    //  ALIGN(4) uint    transmissionTextureIndex;
    ALIGN(4) float roughness;
    ALIGN(4) float metalness;
    ALIGN(4) float specular;
    ALIGN(4) float ao;
    ALIGN(16) float3 albedo;
    ALIGN(16) float3 emissive;
    ALIGN(16) float3 transmission;
    ALIGN(4) float   ior;

};

// material deal with texture sample , use for shading
struct DisneyMaterialSample {
    ALIGN(4) float3  shadingNormal;

    ALIGN(4) unsigned int flags;
    ALIGN(4) int     materialID;

    ALIGN(16) float3 baseColor;
    ALIGN(4) float  subsurface;
    ALIGN(16) float3 subsurfaceRadius;
    ALIGN(16) float3 subsurfaceColor;
    ALIGN(4) float metallic;
    ALIGN(4) float specular;
    ALIGN(4) float specularTint;
    ALIGN(4) float roughness;
    ALIGN(4) float anisotropic;
    ALIGN(4) float anisotropicRotation;
    ALIGN(4) float sheen;
    ALIGN(4) float sheenTint;
    ALIGN(4) float clearcoat;
    ALIGN(4) float clearcoatRoughness;
    ALIGN(4) float ior;
    ALIGN(4) float transmission;
    ALIGN(4) float transmissionRoughness;
    ALIGN(16) float3 emissive;
    ALIGN(4) float alpha;

};


//Cook-Torrancce
struct MaterialConstants
{
    ALIGN(4) unsigned int flags;
    ALIGN(4) int     materialID;
    ALIGN(4) int     shadingModelID;
    ALIGN(4) int     domain;
    ALIGN(4) float   opacity;
    ALIGN(4) float   alphaThreshold; // for alpha tested materials
//for bindless texture
//  ALIGN(4) uint    baseOrDiffuseTextureIndex;
//  ALIGN(4) uint    metalRoughOrSpecularTextureIndex;
//  ALIGN(4) uint    emissiveTextureIndex;
//  ALIGN(4) uint    normalTextureIndex;
//  ALIGN(4) uint    occlusionTextureIndex;
//  ALIGN(4) uint    transmissionTextureIndex;
    ALIGN(4) float roughness;
    ALIGN(4) float metalness;
    ALIGN(4) float specular;
    ALIGN(4) float ao;
    ALIGN(16) float3 albedo;
    ALIGN(16) float3 emissive;
    ALIGN(16) float3 transmission;
    ALIGN(4) float   ior;

};


// Material properties structure
//https://docs.blender.org/manual/en/2.80/render/shader_nodes/shader/principled.html
struct DisneyMaterialConstant {

    ALIGN(4) unsigned int flags;
    ALIGN(4) int     materialID;

    ALIGN(16) float3 baseColor;
    ALIGN(4) float  subsurface;
    ALIGN(16) float3 subsurfaceRadius;
    ALIGN(16) float3 subsurfaceColor;
    ALIGN(4) float metallic;
    ALIGN(4) float specular;
    ALIGN(4) float specularTint;
    ALIGN(4) float roughness;
    ALIGN(4) float anisotropic;
    ALIGN(4) float anisotropicRotation;
    ALIGN(4) float sheen;
    ALIGN(4) float sheenTint;
    ALIGN(4) float clearcoat;
    ALIGN(4) float clearcoatRoughness;
    ALIGN(4) float ior;
    ALIGN(4) float transmission;
    ALIGN(4) float transmissionRoughness;
    ALIGN(16) float3 emissive;
    ALIGN(4) float alpha;
};


//#endif // MATERIAL_CB_H

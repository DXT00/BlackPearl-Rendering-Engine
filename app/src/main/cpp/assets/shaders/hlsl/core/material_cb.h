

#ifndef MATERIAL_CB_H
#define MATERIAL_CB_H
#include "align.h"

const int MaterialDomain_Opaque                   = 0;
const int MaterialDomain_AlphaTested              = 1;
const int MaterialDomain_AlphaBlended             = 2;
const int MaterialDomain_Transmissive             = 3;
const int MaterialDomain_TransmissiveAlphaTested  = 4;
const int MaterialDomain_TransmissiveAlphaBlended = 5;

const int MaterialFlags_UseSpecularGlossModel            = 0x00000001;
const int MaterialFlags_DoubleSided                      = 0x00000002;
const int MaterialFlags_UseMetalRoughOrSpecularTexture   = 0x00000004;
const int MaterialFlags_UseBaseOrDiffuseTexture          = 0x00000008;
const int MaterialFlags_UseEmissiveTexture               = 0x00000010;
const int MaterialFlags_UseNormalTexture                 = 0x00000020;
const int MaterialFlags_UseOcclusionTexture              = 0x00000040;
const int MaterialFlags_UseTransmissionTexture           = 0x00000080;
const int MaterialFlags_ThinSurface                      = 0x00000100;
const int MaterialFlags_PSDExclude                       = 0x00000200;
const int MaterialFlags_UseHeightMapTexture              = 0x00000400;
const int MaterialFlags_UseCubeMapTexture                = 0x00000800;
const int MaterialFlags_UseDepthTexture                  = 0x00001000;
const int MaterialFlags_UseSpecularTexture               = 0x00002000;
const int MaterialFlags_UseRoughnessTexture              = 0x00004000;
const int MaterialFlags_UseMetalTexture                  = 0x00008000;
const int MaterialFlags_UseOpacityTexture                = 0x00010000;

const int MaterialFlags_NestedPriorityMask               = 0xF0000000;
const int MaterialFlags_NestedPriorityShift              = 28;

const int MaterialFlags_PSDDominantDeltaLobeP1Mask       = 0x0F000000;
const int MaterialFlags_PSDDominantDeltaLobeP1Shift      = 24;


const int ShadingModel_Unlit = 0;
const int ShadingModel_DefaultLit = 1;
const int ShadingModel_Disney = 2;





struct Props {
    //Enable texture
    float shininess;
    float refractIndex;
    bool  isBinnLight;
    int  isPBRTextureSample;
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


    ALIGN(4) int flags;
    ALIGN(4) int     materialID;
    ALIGN(4) int     shadingModelID;
    ALIGN(4) int     domain;
    ALIGN(4) float   opacity;
    ALIGN(4) float   alphaThreshold; // for alpha tested materials
    ALIGN(4) float roughness;
    ALIGN(4) float metallic;
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

    ALIGN(4) int flags;
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
    ALIGN(4) int flags;
    ALIGN(4) int     materialID;
    ALIGN(4) int     shadingModelID;
    ALIGN(4) int     domain;
    ALIGN(4) float   opacity;
    ALIGN(4) float   alphaThreshold; // for alpha tested materials
    ALIGN(4) float roughness;
    ALIGN(4) float metallic;
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

    ALIGN(4) int flags;
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


#endif // MATERIAL_CB_H
